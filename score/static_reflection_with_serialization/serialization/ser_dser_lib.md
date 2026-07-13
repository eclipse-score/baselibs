# Serialization & deserialization library

## Prerequisite

To understand the ser/deser library code, one needs to be familiar with C++ template metaprogramming.
Good tutorials can be found on youtube:

* Template Metaprogramming: Type Traits (part 1 of 2) - Jody Hagins - [CppCon 2020]( https://www.youtube.com/watch?v=tiAVWcjIF6o)
* Template Metaprogramming: Practical Application - Jody Hagins - [CppCon 2021](https://www.youtube.com/watch?v=4YC6_77-iEY)

## Overview

Serialization & deserialization is used by datarouter for both verbose and non-verbose DLT messages logging.
The implementation resides under the following locations:

* <broken_link_g/swh/safe-posix-platform/tree/master/score/static_reflection_with_serialization/serialization>
* <broken_link_g/swh/safe-posix-platform/tree/master/score/static_reflection_with_serialization/visitor>

C++ does not support reflection e.g. like Java or C#; so in order do inspect the content of a structure the
implementation is using macros and templates to generate ser/dser code. Only simple linear memory types are
supported (pointer dependent data structures, e.g. linked lists are not supported).

## How to ser/dser a structure

To mark structure for ser/dser, the structure needs to be put in the `SCORE_STRUCT_VISITABLE` macro.
The following code snippet is for overview only. Please check unit tests in implementation directories.

```c++

struct UptimeTick {
    uint32_t counter;
    timestamp_t uptime;
    char for_measurements[6];
};
//helper structure
//that is providing type information for offset and subsize type
//e.g string will be serialized with following layout
//[offset_info][subsize_info][std::string data]
struct real_alloc_t {
    using offset_t = uint32_t;   /*offset type, this is used for storing offset infos e.g. string data is at offset 70*/
    using subsize_t = uint16_t;  /*size type holding the info about size of each element, e.g. len of string is 2*/
};

SCORE_STRUCT_VISITABLE(UptimeTick, counter, uptime, for_measurements);

int main(int argc, char** argv) {
    using namespace ::score::common::visitor;
    using s = serializer_t<real_alloc_t>;
    UptimeTick dataTo_Ser{};
    UptimeTick dataTo_DSer{};
    uint8_t buffer[1024];
    auto retValSer = s::serialize(dataTo_Ser, buffer, sizeof(buffer));
    auto retValDser = s::deserialize(buffer, sizeof(buffer, sizeof(buffer), UptimeTick dataTo_DSer);
    return 0;
}
```

## High level overview of code

### Macros

By calling `SCORE_STRUCT_VISITABLE` the following further macros as are being called:

* `SCORE_STRUCT_VISITABLE_START` --> macro generates template code for inspecting the names of structure members, it is based on
_PRETTY_FUNCTION_.
* `SCORE_STRUCT_VISITABLE_FIELDNAMES(...)` --> macro generates the code that creates array of structure field names
* `SCORE_STRUCT_VISITABLE_FIELDS(...)` --> The _most_ important macro the code generated here is used for ser/dser
* `SCORE_STRUCT_VISITABLE_END` --> generates some helper code.

The macro based code can only support a structure with up to 50 members.

### Templates

In a high level overview the following things happen:

1: Structure parameters are unpacked for ser/dser with following code snippets:

```c++

template <typename A, typename S, typename T>
inline void serialize_parameter_pack(serializer_helper<A>& a, S& s, const T& t) {
    serialize(t, a, s);
}
template <typename A, typename S1, typename S2, typename T1, typename T2, typename... Args>
inline void serialize_parameter_pack(serializer_helper<A>& a, pair_serialized<S1, S2>& s, const T1& t1, const T2& t2,
                                     const Args&... args) {
    serialize(t1, a, s.first);
    serialize_parameter_pack(a, s.second, t2, args...);
}
template <typename A, typename S, typename T>
inline void deserialize_parameter_pack(const S& s, deserializer_helper<A>& a, T& t) {
    deserialize(s, a, t);
}
template <typename A, typename S1, typename S2, typename T1, typename T2, typename... Args>
inline void deserialize_parameter_pack(const pair_serialized<S1, S2>& s, deserializer_helper<A>& a, T1& t1, T2& t2,
                                       Args&... args) {
    deserialize(s.first, a, t1);
    deserialize_parameter_pack(s.second, a, t2, args...);
}
```

2: Template metaprogramming is used to determine the type of a structure member and call corresponding ser/dser function.
   Most of the template metaprogramming code is here to implement this step. Going into details is beyond the scope of
   this documentation. The template metaprogramming code can be broadly divide into two categories:

* type handling and checking compile time code, that is used to correctly select appropriate ser/dser method
    e.g. code snippet that checks if the type is vector like

```c++

template <typename T>
struct is_vector_serializable<
    T, detail::void_t<typename T::value_type, decltype(std::declval<T>().begin()), decltype(std::declval<T>().cbegin()),
                      decltype(std::declval<T>().size()),
                      decltype(std::declval<T>().push_back(std::declval<typename T::value_type>())),
                      typename std::enable_if<is_resizeable<T>::value>::type>> : std::true_type {};
```

* template code that performs run-time ser/dser, that are type specific
    e.g., code snippet for some of ser/dser types with some explanation comments

```c++
/* pair ser/dser*/
template <typename A, typename S1, typename S2, typename T1, typename T2>
inline void serialize(const std::pair<T1, T2>& t, serializer_helper<A>& a, pair_serialized<S1, S2>& s) {
    serialize(t.first, a, s.first);   //correct serialize overload must be found depending on t.first and t.second type
    serialize(t.second, a, s.second);
}

template <typename A, typename S1, typename S2, typename T1, typename T2>
inline void deserialize(const pair_serialized<S1, S2>& s, deserializer_helper<A>& a, std::pair<T1, T2>& t) {
    deserialize(s.first, a, t.first);
    deserialize(s.second, a, t.second);
}

/* array ser/dser*/
template <typename A, typename S, std::size_t N, typename T>
inline void serialize(const T& t, serializer_helper<A>& a, array_serialized<S, N>& s) {
    for (std::size_t i = 0; i != N; ++i) {
        // TODO: use gsl::span or equivalent here
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        serialize(t[i], a, s.arr[i]); /* serialize elements */
    }
}

template <typename A, typename S, std::size_t N, typename T>
inline void deserialize(const array_serialized<S, N>& s, deserializer_helper<A>& a, T& t) {
    for (std::size_t i = 0; i != N; ++i) {
        // TODO: use gsl::span or equivalent here
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        deserialize(s.arr[i], a, t[i]);
    }
}

/* string ser/dser */
//string will be serialized with following layout
//offset_info holds offset where subsize info is located in buffer, after subsize data, string data is stored
//[offset_info][subsize_info][std::string data]
template <typename A, typename T>
inline void serialize(const T& t, serializer_helper<A>& a, string_serialized<A>& s) {
    using subsize_s_t = subsize_serialized<A>;
    auto n = t.size() + 1;                                         /*get size of the string  */
    const auto offset = a.advance(sizeof(subsize_s_t) + n);        /*try to reserve a space for string size type and data */
    serialize(offset, a, s.offset);
    if (offset != 0) {                                             /*if offset == 0 we do not have enough space to serialize  */
        auto subsize = static_cast<typename A::subsize_t>(n);
        serialize(subsize, a, *a.template address<subsize_s_t>(offset));  /*save string size*/
        /*string_size_location should be called string_address like in deserialize */
        char* string_size_location =
            a.template address<char>(static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t)));
        std::memcpy(string_size_location, t.data(), n);
    }
}

template <typename A, typename T>
inline void deserialize(const string_serialized<A>& s, deserializer_helper<A>& a, T& t) {
    using subsize_s_t = const subsize_serialized<A>;
    typename A::offset_t offset;
    deserialize(s.offset, a, offset);
    if (offset == 0) {
        a.setZeroOffset();
        t.resize(0);
        return;
    }
    /*string_size_location is proper name here*/
    const auto string_size_location = a.template address<subsize_s_t>(offset);
    if (string_size_location == nullptr) {
        // error condition already set by a.address()
        t.resize(0);
        return;
    }

    typename A::subsize_t subsize{0};
    deserialize(*string_size_location, a, subsize);
    const std::size_t n = subsize;
    if (n == 0) {
        a.setInvalidFormat();
        t.resize(0);
        return;
    }

    const auto string_content_offset = static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t));
    const char* string_address = a.template address<const char>(string_content_offset, n);
    if (string_address == nullptr) {
        // error condition already set by a.address()
        t.resize(0);
        return;
    }
    t.assign(string_address, n - 1);
}

/* vector ser/dser */
//serialized vector<std::string> will have following layout
//first offset info will be serialized, each member holding offset in the buffer to serialized std::string
//[offset_info_0]...[offset_info_n][serialized std::string_0].....[serialized std::string_n]
template <typename A, typename S, typename T>
inline void serialize(const T& t, serializer_helper<A>& a, vector_serialized<A, S>& s) {
    using subsize_s_t = subsize_serialized<A>;
    auto n = t.size();
    auto offset = a.advance(sizeof(subsize_s_t) + n * sizeof(S)); /*get space for the vector offset data*/
    serialize(offset, a, s.offset);
    if (offset != 0) {
        auto subsize = static_cast<typename A::subsize_t>(n * sizeof(S));
        serialize(subsize, a, *a.template address<subsize_s_t>(offset));
        S* string_size_location =
            a.template address<S>(static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t)));
        for (std::size_t i = 0; i != n; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            serialize(*(t.cbegin() + static_cast<std::ptrdiff_t>(i)), a, string_size_location[i]);
        }
    }
}

template <typename A, typename S, typename T>
inline void deserialize(const vector_serialized<A, S>& s, deserializer_helper<A>& a, T& t) {
    using subsize_s_t = const subsize_serialized<A>;
    typename A::offset_t offset;
    deserialize(s.offset, a, offset);
    if (offset == 0) {
        a.setZeroOffset();
        detail::clear(t);
        return;
    }
    const auto vector_size_location = a.template address<subsize_s_t>(offset);
    if (vector_size_location == nullptr) {
        // error condition already set by a.address()
        detail::clear(t);
        return;
    }
    typename A::subsize_t subsize;
    deserialize(*vector_size_location, a, subsize);
    const std::size_t n = subsize / sizeof(S);

    const auto vector_contents_offset = static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t));
    const S* vector_contents_address = a.template address<const S>(vector_contents_offset, n);
    if (vector_contents_address == nullptr) {
        detail::clear(t);
        return;
    }
    detail::resize(t, n);
    for (std::size_t i = 0; i != n; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        deserialize(vector_contents_address[i], a, *(t.begin() + static_cast<std::ptrdiff_t>(i)));
    }
}

```

3: memcpy() is called to do copy the member data to a buffer or from a buffer to a member.

## Non-verbose logging

To get non-verbose fibex data for the structures that are being serialized, please check this link:

* [Non-verbose Logging](broken_link_cf/spaces/psp/pages/334671839/Non-verbose+logging)

## Debugging

Ser/dser library is a header only so the best way to debug issues is to create a small test application and try the unit test examples.
The metaprogramming compiler errors are hard to read so try to simplify the code as much as possible.
If the ser/dser code breaks while running, the error message can be long (50k or more), first thing is do not panic :)
Since ser/dser code is in heavy use, so the bugs are rare and usually they are due to incomplete data that cannot be deserialized.
So, first thing is to check if the serialized data is big enough.
There are compile time assertions in place that should minimize these issues.
