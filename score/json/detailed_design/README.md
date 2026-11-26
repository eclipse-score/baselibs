# Detailed Design

You can find the design as below:

<img alt="Class diagram" src="https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/eclipse-score/baselibs/refs/heads/main/score/json/detailed_design/class_diagram.puml">

The main idea is to split the library into two parts, one user facing and one
implementation detail (the vaJson parser and the output formatting).
The implementation detail is not selectable.

Basically we offer to the user the actual JSON data structure as also a parser
to create this data structure from a file or a buffer and a writer to write it
as compact or pretty-printed JSON.

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
