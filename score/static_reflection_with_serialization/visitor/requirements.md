# visitor requirements

## Rationale

The visitor is a header-only library defining a framework to provide an uniform way of traversal of heterogeneous C++ data structures with focus on compile-time guarantees for finiteness (when needed), static type safety, and efficiency of compiler optimizations.

The main intended use of the framework is object serialization/deserialization for interprocess communications, persistent storage, and diagnostics.

The list of requirements below is supposed to describe the library functionality in a testable way.

## Requirements list

- VISIT-AS: A call to `::score::common::visitor::visit(v, t)` shall be redirected to `visit_as(v, t)`, if the latter is unambiguously (in terms of C++ lookup) defined for the given argument types. The return type and value should match the return type and value of the latter function. The parameters are forwarded using C++ "perfect forwarding" idiom.

- VISIT-ADL: C++ argument-dependent lookup shall be used to find the target function for `visit_as(v, t)` call. The names from the namespaces associated with both arguments shall be considered (including namespaces of template type parameters of template-typed arguments). However, additional unqualified names available in the local context can (and likely will) be ignored.

- VISIT-OVERLOAD: As a result of argument-dependent lookup, if multiple `visit_as()` function candidates are found, the normal C++ rules for overload resolution shall be working. In particular, non-template functions shall have a priority over template specializations, and more specialized templates shall have a priority over less specialized ones. SFINAE shall be available for additional control over overload resolution.

- VISIT-CONVERSION: A possibility of C++ implicit conversions shall be considered as available for the purpose of overload resolution for `visit_as(v, t)` (while it is not always a desired feature, there is little we could do to prevent it from being available; static code analyzers might be employed to detect the instances of its abuse in the actual use).

- VISIT-NONVISITABLE: If there is no definition of `visit_as(v, t)` found, a conversion operator from `t` to `::score::common::visitor::visitable_type` shall be called (with the rationale to simplify reading compiler errors if `visit_as()` is not properly defined for the given types).

- VISIT-STRUCT: To simplify using visitor with structure/class types, a macro `STRUCT_VISITABLE(structname, fieldnames...)` shall be available. An unqualified class name is used as `structname`; the `STRUCT_VISITABLE()` macro instantiation is placed in the namespace containing `structname` definition. The macro shall define a helper class type aliased as a `::score::common::visitor::struct_visitable<S>` template, (with `S` being a proper typename in the context of the template use). The number of fields supported is from 1 to 20.

- VISIT-STRUCT-VISITABLE: The `::score::common::visitor::struct_visitable<S>` helper class shall contain the following static helper constant: `fields`, the number of fields defined, as well as the following static helper functions: `name()`, returning the fully qualified class name; `field_name(i)`, returning the name of the i-th field (starting from zero); `visit(v, t)`, calling a (separately defined, found with argument-dependent lookup) `visit_as_struct(v, s, fieldnames)` function and returning its result.

- VISIT-STRUCT-AS: Additionally, the `STRUCT_VISITABLE(structname, fieldnames...)` macro shall define the `visit_as(v, structname)` function that calls the `visit(v, t)` function from the helper class. This function should be compatible with the `::score::common::visitor::visit(v, t)` function.

- VISIT-STRUCT-NAMESPACE: The `STRUCT_VISITABLE()` macros with the same `structname` placed in the different namespaces correspond to the different classes from the corresponding namespaces.

- VISIT-OSTREAM: As an example for using visitor functionality, serialization to a C++ `std::ostream` shall be implemented. At least the following fundamental C++ types should be supported: `char`, `int`, `double`. At least the following non-recursive compound C++ types shall be supported: references, `std::string`. At least the following potentially recursive compound C++ types shall be supported: C-style arrays, `std::array<>`, `std::vector<>`, `std::tuple<>`, `std::pair<>`, structs (with the help of `STRUCT_VISITABLE()` macro).

- VISIT-OSTREAM-COMPOUND: For potentially recursive compound types, the ability to traverse at least one extra level of static (compile-time) recursive type shall be demonstrated for C-style arrays, `std::array<>`, `std::vector<>`, `std::tuple<>`, `std::pair<>`, structs (with the help of `STRUCT_VISITABLE()` macro). Any claims about dynamic recursion (such as supporting linked lists through pointers or references to the objects of the same type) are out of scope of this requirement.
