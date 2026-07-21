# serialization requirements

## Rationale

The serializer is a header-only library for binary serialization, deserialization, and compile-time type reflection of heterogenuous C++ data structures with focus on compile-time safety and efficiency of serialization, as well as efficiency of filtering by content during deserialization.

The main intended use of the library is object serialization/deserialization for interprocess communications, persistent storage, and diagnostics, with some degree of compatibility with nonverbose DLT messages for the serialized data.

The list of requirements below is supposed to describe the library functionality in a testable way.

## Requirements list

- SERIALIZE-SERIALIZED: The typename (possibly alias) template `::score::common::visitor::serialized_t<A, T>` shall be a standard-layout, byte-aligned C++ type for any C++ type `T` constructed using the following C++ types
  - Fundamental types: `char`; `uintN_t` and `intN_t` for `N` being 8, 16, 32, and 64; `float` and `double`;
  - Compound types: `std::string`, C-style arrays, `std::array<>`, `std::vector<>`, `std::tuple<>`, `std::pair<>`, structs (with the help of `STRUCT_VISITABLE()` macro);
  - References shall be supported as long as they refer to a complete static type and the referred object's dynamic type is equivalent to the static type for the purpose of serialization. Using incomplete or dynamic types for the purpose of making the depth of the reference path not computable at compile time is not supported.

- SERIALIZE-ALLOCATOR: The `A` typename parameter of the `serialized_t<A, T>` template defines the types used for handling data which serialization size is not calculated at compile time (in particular, the `std::string` and `std::vector<>` types). It contains the following type definitions:
  - `A::offset_t` is a `uintN_t` type not larger than `std::size_t`. It is used to contain the offset from the beginning of the `serialized_t<>` object to the start of the memory area designated to keep the dynamically sized serialization information. The size of the type should be enough to keep the longest allowed length for the serialized object. For typical applications, etiher `std::size_t` or `uint32_t` will be used.
  - `A::subsize_t` is a `uintN_t` type not larger than `A::offset_t`. It is used to contain the size of the dynamically sized serialization information. The size of the type should be enough to keep the longest allowed length for a serialized `std::string` or `std::vector` object from the serialized data structure. If binary compatibility of the serialized format with nonverbose DLT messages is desired, the type should be `uint16_t`.

- SERIALIZE-LAYOUT: The size and content of `::score::common::visitor::serialized_t<A, T>` shall be defined as follows:
  - for each fundamental type, the size and the content of the serialized object is the size and the content of the object being serialized. The byte order of serialized data is the host byte order (currently Intel);
  - for the `std::string` and `std::vector<>` types, the `serialized<>` contains one `A::offset_t` serialized field. The actual serialized data is not a part of the `serialized<>` object itself; the `A::offset_t` serialized field contains the offset to the beginning of the data from the beginning of the outermost `serialized<>` object for the serialized data structure. The serialized data consists of an `A::subside_t` header specifying the length of the payload (not including the header itself), then the payload data.
  - for the `std::string` type, the payload is the sequence of bytes representing the string in the host locale, zero terminating byte included (for compatibility with DLT format).
  - for the `std::vector<>` type, the payload is equivalent to the serialized C-style array payload of the equivalent size and value type.
  - for C-style arrays, `std::array<>`, `std::tuple<>`, `std::pair<>`, structs (with the help of `STRUCT_VISITABLE()` macro) the serialized object consists of the concatenated (without padding) serialized objects of their subjects traversed in their natural order (for structs, the "natural" order is the order defined in their coddesponding `STRUCT_VISITABLE()` macro).

- SERIALIZE-SERIALIZER: There shall be an template class `::score::common::visitor::serializer_t<A>` having the following static function templates implementing serialization/deserialization:
  - `A::offset_t serialize<T>(const T& t, char* data, A::offset_t size)`
  - `void deserialize<T>(const char* data, A::offset_t size, T& t)`
