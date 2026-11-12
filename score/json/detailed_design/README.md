# Detailed Design

You can find the design as below:

<img alt="Class diagram" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/json/detailed_design/class_diagram.puml">

The main idea is to split the library into two parts, one user facing and one
implementation detail which can be swapped out if necessary.

Basically we offer to the user the actual JSON data structure as also free
functions to create this data structure from a file.

JSON supports different value types: object, list, string, null, boolean,
integer and float. JSON objects and lists can be interpreted as trees, where
inner items can be nested in an arbitrary order. In order to reduce heap
allocation during runtime, we build up a custom tree in heap using respective
types. This custom tree is then handed-over to the user; and if used as
read-only, no further heap allocation is performed.

In order to cope with the issue that we don't know how the tree looks like in
sense of data types, we introduce `score::json::Any` which is a wrapper around
`std::variant` that can hold all of the mentioned types. Since JSON objects and
lists can again hold `Any`, we have a cyclic dependency, which can only be
broken by using forward declarations and pointers.

Since we don't want to expose pointers to the user, we always return references
(since it's guaranteed that the pointers are not null).

## Parser Nesting Policy

`JsonParser` applies a fixed maximum nesting depth of 64, defined as a constant in `json_parser.cpp`.
Each `FromBuffer` or `FromFile` call constructs its own `AnyTreeBuilder`.
The builder's `hierarchy_` contains pointers to open arrays and objects, so its size counts container depth without counting pending keys or scalar values.
`StartContainer` compares this size with the limit before storing the new container or pushing its pointer.
If the size already equals the limit, the callback returns a backend parse error, mapped to the existing public `Error::kParsingError`.
`EndContainer` pops the pointer, making depth available to siblings.

The grammar parser updates its iterative grammar state before invoking the builder callback, so it can transiently hold one additional container when the builder rejects input.
The rejected container is not inserted into the partial `Any` tree.
The local builder and grammar state are destroyed on return; later calls do not retain nesting state.
Partial and accepted trees contain at most 64 nested containers.

The model owns nested containers through pointers and destroys, clones and compares them recursively; serialization also traverses nested values recursively.
The integrator must budget stack space for these operations at the maximum depth, including rejection cleanup.
The limit is not a measured or platform-qualified resource bound, and the policy does not constrain manually built trees or other input resources.
