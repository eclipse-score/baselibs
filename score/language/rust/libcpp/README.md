# ABI-dependent definitions

## Problem description

The C++ specification defines only the behavior that implementations should support,
not the internal layout of objects, for example, below we see how two popular C++
implementations represent the `std::string_view` type:

```
libstdc++ (used by GCC): [len][data]
libc++ (used by clang): [data][len]
```

That means that any FFI integration between Rust and C++ has to be aware of which
implementation of the standard lib is being used by the C++ code, otherwise mapping
would not match, and that could lead to Undefined Behavior (UB).

This crate aims to handle these ABI differences for stdlib types.

## Second-order problem definition

Imagine that one would have many of such types, and would like to support a wide range of implementations.
Clearly, the amount of work to maintain the compatibility would be non-trivial.

Therefore there's a second-order problem:
How to maintain the compatibility if the underlying ABIs change?

For tackling this problem, the following proposals have emerged:
1. Propose to the Rust community a sibling library to `libc`. This library currently exposes
   common C types to Rust, in a binary-compatible way. We could propose the creation of,
   say, `libcpp`, that would do the same for C++. A possible technical hurdle, though, is
   that such a lib would most likely need the functionality proposed for
   [`std::is_trivially_relocatable_v<T>`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1144r6.html), that was not yet accepted, not even for C++26.
   * Pros: Maintenance burden is shared with open-source community.
   * Cons: Some SCORE-internal types might still need an extra layer of maintenance.
1. Introspect somehow into generated code using a particular version of the lib, and
   automatically detect it's layout, and then use code generation tools to create
   the glue code on compiling time.
   * Pros: Would avoid manual work for maintaining changes.
   * Cons: Requires investment to create and test such tooling (and tooling maintenance).
1. Before adopting a new lib, performing an ABI compliance check (see section [How to Detect ABI Breaks](#How_to_Detect_ABI_Breaks)), and assess the impact on possible internally-maintained
   mapping. Then, given the impact, decide if we would like to make the necessary changes
   in-house or hire external workforce.
1. Collaborating new types to [CXX project](https://cxx.rs/) project directly, as this is
   the glueing layer that we are using for the FFI interoperability. However, Markus commented
   that the process can take too much time, as he received feeback about a proposal he made
   to this project only after one and a half year.
