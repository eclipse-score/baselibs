# Utils detailed design

This document helps contributors understand the component boundaries and the ownership, lifetime, and concurrency constraints of the public C++ utilities in `score/utils`.
It describes design intent, not a complete API reference or usage guide.

## Component boundary

The Utils component groups independent utilities that do not form one runtime subsystem.
Each utility has a separate Bazel target so consumers can depend on only the functionality they use:

- `//score/utils:base64`
- `//score/utils:static_destruction_guard`
- `//score/utils/meyer_singleton:meyer_singleton`
- `//score/utils/string_registry:string_registry`
- `//score/utils/string_registry:registry_string_view`

The string registry depends on the registry string view.
The other utilities do not depend on each other.

## Base64 encoding and decoding

The Base64 utility is stateless.
`EncodeBase64` converts a `std::vector<std::uint8_t>` into a padded Base64 `std::string`.
`DecodeBase64` converts Base64 text into a `std::vector<std::uint8_t>`.

## Thread-safe singleton

`MeyerSingleton<Object>` creates one function-local static `Object` instance on first access.
`Object` must not be copyable or movable.

Callers initialize an object type through either `GetInstance`, which forwards constructor arguments, or `GetInstanceInitializedWithCallable`, which invokes an initialization callable.
Callers must use only one initialization method for each object type.
Mixing the methods violates a precondition.

An atomic pointer acts as a compiler fence around singleton access.
This design preserves the intended static construction and destruction order when multiple threads access the singleton.

## Static destruction guard

`StaticDestructionGuard<T>` implements the nifty-counter idiom for a static object that another static object's destructor needs to access.
The first guard constructs `T`, and the final guard destroys it.

`GetStorage` returns the single guarded instance.
The returned reference is valid only while at least one guard remains alive.
The guard controls static lifetime but does not synchronize ordinary concurrent access to `T`.

## String registry

`StringRegistry` deduplicates strings by content and returns stable `RegistryStringView` instances.
A mutex serializes insertion.

The `std::string_view` and `std::string&&` overloads store owned strings in the registry.
The `LiteralTag` overload stores the supplied view without copying its content, so the referenced storage must outlive the registry.

All returned views depend on registry-owned or caller-owned storage and must not outlive the registry.

## Registry string view

`RegistryStringView` provides immutable access to a registered string.
Equality and hashing use the backing address, so equal text from different registries does not have the same identity.
Relational operators compare string content.
Use `StringView` when content-based comparison with `std::string_view` is required.
