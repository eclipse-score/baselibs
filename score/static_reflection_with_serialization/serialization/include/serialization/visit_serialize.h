/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SERIALIZE_H
#define SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SERIALIZE_H

#include "serialization/visit_size.h"
#include "serialization/visit_type_traits.h"
#include "visitor/visit.h"
#include "visitor/visit_as_struct.h"

#include "score/assert.hpp"
#include "score/optional.hpp"
#include "score/span.hpp"

#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstring>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace score
{

namespace common
{

namespace visitor
{

/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Char used as a byte representation. */
using OneByte = char;
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC: Char used as a byte representation. */

namespace payload_tags
{

struct bytes
{
};
struct text
{
};
struct signed_le
{
};
struct unsigned_le
{
};
struct ieee754_float_le
{
};
struct array
{
};
struct bitset
{
};
struct pack
{
};
struct string
{
};
struct vector
{
};
struct optional
{
};

}  // namespace payload_tags

namespace details
{
template <typename S, typename T>
auto cast_to_source_serializable_data_span(const T* data, size_t size) -> score::cpp::span<const S>
{
    static_assert(sizeof(S) == sizeof(T), "Size value does not match");
    /*
            Deviation from Rule M5-2-8:
            - An object with integer type or pointer to void type shall not be converted
          to an object with pointer type.
            Justification:
            - cast needed to fulfill basic purpose of serialization to convert user types to stream of bytes
    */
    // coverity[autosar_cpp14_m5_2_8_violation]
    return score::cpp::span<const S>{static_cast<const S*>(static_cast<const void*>(data)),
                              static_cast<typename score::cpp::span<const S>::size_type>(size)};
}
template <typename S, typename T>
auto cast_to_destination_serializable_data_span(T* data, size_t size) -> score::cpp::span<S>
{
    static_assert(sizeof(S) == sizeof(T), "Size value does not match");
    /*
            Deviation from Rule M5-2-8:
            - An object with integer type or pointer to void type shall not be converted
          to an object with pointer type.
            Justification:
            - cast needed to fulfill basic purpose of serialization to convert user types to stream of bytes
    */
    // coverity[autosar_cpp14_m5_2_8_violation]
    return score::cpp::span<S>{static_cast<S*>(static_cast<void*>(data)),
                        static_cast<typename score::cpp::span<const S>::size_type>(size)};
}
}  //  namespace details

template <typename T>
struct is_serialized_type : public std::false_type
{
};

/* KW_SUPPRESS_START:MISRA.LOGIC.OPERATOR.NOT_BOOL: False positive: all boolean expressions are correct. */
/* KW_SUPPRESS_START:AUTOSAR.CAST.REINTERPRET:Reinterpret needs to be used to cast to ptr. */
/* KW_SUPPRESS_START:MISRA.FUNC.UNUSEDPAR: False positive: all templates arguments are used. */
/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST: False positive for some cases: Variables in templates are modified. */
/* KW_SUPPRESS_START:AUTOSAR.FUNC.TMPL.EXPLICIT_SPEC: False positive: no specialization is used. */

template <typename A>
class serializer_helper
{
  public:
    using offset_t = typename A::offset_t;
    using subsize_t = typename A::subsize_t;
    static_assert(sizeof(offset_t) <= sizeof(size_t), "offset_t too large");
    static_assert(sizeof(offset_t) >= sizeof(subsize_t), "subsize_t too large");

    serializer_helper(std::uint8_t* base, offset_t maxSize, offset_t startSize)
        : base_(base), maxSize_(maxSize), curSize_(startSize)
    {
        /* KW_SUPPRESS_START: MISRA.USE.EXPANSION: Macro for assertion is tolerated by decision. */
        SCORE_LANGUAGE_FUTURECPP_ASSERT(base != nullptr);
        SCORE_LANGUAGE_FUTURECPP_ASSERT(startSize > static_cast<offset_t>(0));
        SCORE_LANGUAGE_FUTURECPP_ASSERT(startSize <= maxSize);
        /* KW_SUPPRESS_END: MISRA.USE.EXPANSION */
    }

    offset_t advance(std::size_t size)
    {
        if (size > std::numeric_limits<subsize_t>::max())
        {
            // too big value requested
            return 0UL;
        }
        const auto remaining_space = maxSize_ - curSize_;
        if (size > remaining_space)
        {
            return 0UL;
        }
        SCORE_LANGUAGE_FUTURECPP_ASSERT(curSize_ <= (std::numeric_limits<offset_t>::max() - static_cast<offset_t>(size)));
        curSize_ += static_cast<offset_t>(size);
        return curSize_ - static_cast<offset_t>(size);
    }

    offset_t total() const
    {
        return curSize_;
    }

    template <typename T>
    T* address(offset_t size) const
    {
        static_assert(std::is_standard_layout<T>::value, "attempt to serialize to a non-StandardLayout type");
        static_assert(alignof(T) == 1, "attempt to serialize to a not byte aligned type");
        /* KW_SUPPRESS_START:MISRA.PTR.ARITH:Needed to get offset from this location */
        // reinterpret cast need to cast to different pointer type
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) same as KW
        score::cpp::span<std::uint8_t> dataSpan{base_,
                                         static_cast<typename score::cpp::span<std::uint8_t* const>::size_type>(maxSize_)};
        /*
                Deviation from Rule A5-2-4:
                - reinterpret_cast shall not be used.
                Justification:
                - Reinterpret_cast shall be used to convert from pointer
            inside base_ into type specified (T).
        */
        // coverity[autosar_cpp14_a5_2_4_violation]
        return reinterpret_cast<T*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) jusified
            dataSpan.subspan(static_cast<typename score::cpp::span<std::uint8_t* const>::size_type>(size)).data());
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) same as KW
        /* KW_SUPPRESS_END:MISRA.PTR.ARITH:Needed to get offset from this location */
    }

    /* KW_SUPPRESS_START: MISRA.USE.EXPANSION: False positive: it is not macro. */
  private:
    /* KW_SUPPRESS_END: MISRA.USE.EXPANSION: False positive: it is not macro. */
    /* KW_SUPPRESS_START: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
    /* KW_SUPPRESS_START: MISRA.VAR.HIDDEN: False positive: it is struct member. */
    std::uint8_t* const base_;
    const offset_t maxSize_;
    /* KW_SUPPRESS_END: MISRA.VAR.HIDDEN: False positive: it is struct member. */
    offset_t curSize_;
    /* KW_SUPPRESS_END: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
};

template <typename A>
class deserializer_helper
{
  public:
    using offset_t = typename A::offset_t;
    using subsize_t = typename A::subsize_t;
    static_assert(sizeof(offset_t) <= sizeof(size_t), "offset_t too large");
    static_assert(sizeof(offset_t) >= sizeof(subsize_t), "subsize_t too large");

    deserializer_helper(const std::uint8_t* base, offset_t maxSize)
        : base_(base), maxSize_(maxSize), invalid_format_{false}, zero_offset_{false}, out_of_bounds_{false}
    {
    }

    template <typename T>
    T* address(offset_t size, std::size_t n = 1UL)
    {
        static_assert(std::is_standard_layout<T>::value, "attempt to deserialize from a non-StandardLayout type");
        static_assert(alignof(T) == 1, "attempt to deserialize from a not byte aligned type");
        /* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
        /*
                Deviation from Rule A4-7-1:
                - An integer expression shall not lead to data loss.
                Justification:
                - Checking for overflow.
            - Can not change or use SCORE_LANGUAGE_FUTURECPP_ASSERT, because uinttest depends on it.
        */
        // coverity[autosar_cpp14_a4_7_1_violation]
        if (size + sizeof(T) * n > maxSize_)
        {
            /* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
            /* KW_SUPPRESS_START: MISRA.ONEDEFRULE.VAR: False positive. */
            out_of_bounds_ = true;
            /* KW_SUPPRESS_END: MISRA.ONEDEFRULE.VAR: False positive. */
            return nullptr;
        }
        /* KW_SUPPRESS_START:MISRA.PTR.ARITH:Needed to get offset from this location */
        /* KW_SUPPRESS_START: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) justified
        score::cpp::span<const std::uint8_t> dataSpan{
            base_, static_cast<typename score::cpp::span<const std::uint8_t* const>::size_type>(maxSize_)};
        /*
                Deviation from Rule A5-2-4:
                - reinterpret_cast shall not be used.
                Justification:
                - Reinterpret_cast shall be used to convert from pointer
            inside base_ into type specified (T).
        */
        // coverity[autosar_cpp14_a5_2_4_violation]
        return reinterpret_cast<T*>(  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) justified
            dataSpan.subspan(static_cast<typename score::cpp::span<std::uint8_t* const>::size_type>(size)).data());
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) justified
        /* KW_SUPPRESS_END: AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: False positive. */
        /* KW_SUPPRESS_END:MISRA.PTR.ARITH:Needed to get offset from this location */
    }

    bool getInvalidFormat() const
    {
        return invalid_format_;
    }
    bool getZeroOffset() const
    {
        return zero_offset_;
    }
    bool getOutOfBounds() const
    {
        return out_of_bounds_;
    }

    void setZeroOffset()
    {
        zero_offset_ = true;
    }

    void setInvalidFormat()
    {
        invalid_format_ = true;
    }

    /* KW_SUPPRESS_START: MISRA.USE.EXPANSION: False positive: it is not macro. */
  private:
    /* KW_SUPPRESS_END: MISRA.USE.EXPANSION: False positive: it is not macro. */
    /* KW_SUPPRESS_START: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
    /* KW_SUPPRESS_START: MISRA.VAR.HIDDEN: False positive: it is struct member. */
    const std::uint8_t* const base_;
    const offset_t maxSize_;
    /* KW_SUPPRESS_END: MISRA.VAR.HIDDEN: False positive: it is struct member. */
    /* KW_SUPPRESS_START: MISRA.ONEDEFRULE.VAR: False positive. */
    bool invalid_format_;
    bool zero_offset_;
    /* KW_SUPPRESS_START: MISRA.OBJ.TYPE.COMPAT: This is struct field and constness of another struct's field has no
     * meaning. */
    /* KW_SUPPRESS_START: MISRA.OBJ.TYPE.IDENT: This is struct field and constness of another struct's field has no
     * meaning. */
    /* KW_SUPPRESS_START: MISRA.LINKAGE.EXTERN: False positive. */
    bool out_of_bounds_;
    /* KW_SUPPRESS_END: MISRA.LINKAGE.EXTERN: False positive. */
    /* KW_SUPPRESS_END: MISRA.OBJ.TYPE.IDENT: This is struct field and constness of another struct's field has no
     * meaning. */
    /* KW_SUPPRESS_END: MISRA.OBJ.TYPE.COMPAT: This is struct field and constness of another struct's field has no
     * meaning. */
    /* KW_SUPPRESS_END: MISRA.ONEDEFRULE.VAR: False positive. */
    /* KW_SUPPRESS_END: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
};

// memcpy_serialized

template <std::size_t N>
struct memcpy_serialized
{
    std::array<OneByte, N> arr;
};

template <std::size_t N>
struct is_serialized_type<memcpy_serialized<N>> : public std::true_type
{
};

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
template <typename A, typename T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const T& t, serializer_helper<A>& /*unused*/, memcpy_serialized<sizeof(T)>& serial)
{
    // NOLINTNEXTLINE(score-banned-function) tolerated per design
    std::ignore = std::memcpy(serial.arr.data(), &t, sizeof(T));
}

template <typename A, typename T>
/*
        Deviation from Rule M3-2-2:
        - The One Definition Rule shall not be violated
        Justification:
        - This is false positive, Overload signatures are different.
*/
// coverity[autosar_cpp14_m3_2_2_violation: FALSE]
inline void deserialize(const memcpy_serialized<sizeof(T)>& serial, deserializer_helper<A>& /*unused*/, T& t)
{
    // NOLINTBEGIN(score-banned-function) tolerated per design
    // Suppress "AUTOSAR C++14 A12-0-2" The rule states: "Bitwise operations and operations that assume data
    // representation in memory shall not be performed on objects." Serialization and deserialization operations involve
    // copying raw memory using std::memcpy. The object being deserialized (std::array<unsigned char, 16>) is
    // TriviallyCopyable, and thus, safe to copy with memcpy.The source buffer is just raw bytes and this low-level
    // operation is necessary for performance optimization in handling raw data.
    // coverity[autosar_cpp14_a12_0_2_violation]
    std::ignore = std::memcpy(&t, serial.arr.data(), sizeof(T));
    // NOLINTEND(score-banned-function) tolerated per design
}
/* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */

// array_serialized

template <typename S, std::size_t N>
struct array_serialized
{
    static_assert(is_serialized_type<std::remove_cv_t<S>>::value, "element type is not serialized type");
    std::array<S, N> arr;
};

template <typename S, std::size_t N>
struct is_serialized_type<array_serialized<S, N>> : public std::true_type
{
};

template <typename A, typename S, std::size_t N, typename T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const T& t, serializer_helper<A>& a, array_serialized<S, N>& serial)
{
    for (std::size_t i = 0UL; i != N; ++i)
    {
        // TODO: use gsl::span or equivalent here
        serialize(t[i], a, serial.arr[i]);
    }
}

template <typename A, typename S, std::size_t N, typename T>
inline void deserialize(const array_serialized<S, N>& serial, deserializer_helper<A>& a, T& t)
{
    for (std::size_t i = 0UL; i != N; ++i)
    {
        // TODO: use gsl::span or equivalent here
        deserialize(serial.arr[i], a, t[i]);
    }
}

// pair_serialized

template <typename S1, typename S2>
struct pair_serialized
{
    static_assert(is_serialized_type<std::remove_cv_t<S1>>::value, "element1 type is not serialized type");
    static_assert(is_serialized_type<std::remove_cv_t<S2>>::value, "element2 type is not serialized type");
    S1 first;
    S2 second;
};

template <typename S1, typename S2>
struct is_serialized_type<pair_serialized<S1, S2>> : public std::true_type
{
};

template <typename A, typename S1, typename S2, typename T1, typename T2>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const std::pair<T1, T2>& t, serializer_helper<A>& a, pair_serialized<S1, S2>& serial)
{
    serialize(t.first, a, serial.first);
    serialize(t.second, a, serial.second);
}

template <typename A, typename S1, typename S2, typename T1, typename T2>
inline void deserialize(const pair_serialized<S1, S2>& serial, deserializer_helper<A>& a, std::pair<T1, T2>& t)
{
    deserialize(serial.first, a, t.first);
    deserialize(serial.second, a, t.second);
}

template <typename A, typename P, typename T1, typename T2>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const std::pair<T1, T2>& t, serializer_helper<A>& a, P& serial)
{
    serialize(t.first, a, serial.pack.first);
    serialize(t.second, a, serial.pack.second);
}

template <typename A, typename P, typename T1, typename T2>
inline void deserialize(const P& serial, deserializer_helper<A>& a, std::pair<T1, T2>& t)
{
    deserialize(serial.pack.first, a, t.first);
    deserialize(serial.pack.second, a, t.second);
}

// optional_serialized

template <typename A, typename S, typename T1, typename T2>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const score::cpp::optional<S>& t, serializer_helper<A>& a, pair_serialized<T1, T2>& s)
{
    serialize(t.has_value(), a, s.first);
    if (t.has_value())
    {
        serialize(t.value(), a, s.second);
    }
    else
    {
        S empty_value{};
        serialize(empty_value, a, s.second);
    }
}

template <typename A, typename S, typename T1, typename T2>
inline void deserialize(const pair_serialized<T1, T2>& s, deserializer_helper<A>& a, score::cpp::optional<S>& t)
{
    t.reset();
    bool has_value{false};
    deserialize(s.first, a, has_value);

    if (has_value)
    {
        S value;
        deserialize(s.second, a, value);
        t = value;
    }
}

template <typename A, typename P, typename S>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const score::cpp::optional<S>& t, serializer_helper<A>& a, P& s)
{
    serialize(t, a, s.pack);
}

template <typename A, typename P, typename S>
inline void deserialize(const P& s, deserializer_helper<A>& a, score::cpp::optional<S>& t)
{
    deserialize(s.pack, a, t);
}

// vector_serialized

template <typename A>
using offset_serialized = memcpy_serialized<sizeof(typename A::offset_t)>;

template <typename A>
using subsize_serialized = memcpy_serialized<sizeof(typename A::subsize_t)>;

template <typename A, typename S>
struct vector_serialized
{
    static_assert(is_serialized_type<std::remove_cv_t<S>>::value, "element type is not serialized type");
    offset_serialized<A> offset;
};

template <typename A, typename S>
struct is_serialized_type<vector_serialized<A, S>> : public std::true_type
{
};

namespace detail
{

/* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */
template <typename T, std::enable_if_t<has_clear<T>::value, std::int32_t> = 0>
inline void clear(T& t)
{
    t.clear();
}

template <typename T, std::enable_if_t<(has_resize<T>::value) && (!has_clear<T>::value), std::int32_t> = 0>
inline void clear(T& t)
{
    t.resize(0);
}

template <typename T, std::enable_if_t<(!has_resize<T>::value) && (!has_clear<T>::value), std::int32_t> = 0>
inline void clear(T& t)
{
    t = T{};
}

template <typename T, std::enable_if_t<has_resize<T>::value, std::int32_t> = 0>
inline void resize(T& t, size_t n)
{
    t.resize(n);
}

template <typename T, std::enable_if_t<!has_resize<T>::value, std::int32_t> = 0>
inline void resize(T& t, size_t n)
{
    if (t.size() > n)
    {
        detail::clear(t);
    }
    while (t.size() < n)
    {
        t.push_back(typename T::value_type{});
    }
}
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */
/* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

}  // namespace detail

template <typename A, typename S, typename T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const T& t, serializer_helper<A>& a, vector_serialized<A, S>& serial)
{
    static_assert(sizeof(S) <= std::numeric_limits<std::size_t>::max(), "S is too large");
    using subsize_s_t = subsize_serialized<A>;
    const auto n = t.size();
    constexpr size_t max_n = (std::numeric_limits<typename A::offset_t>::max() - sizeof(subsize_s_t)) / sizeof(S);
    // We can't achieve the TRUE case for the below condition due to:
    // 1- The above assertion.
    // 2- Even if we wrote a test case it will be useless and without any expectation or assertion because
    //    the function will return gracefully without any return values or even without a failure.
    // LCOV_EXCL_START
    if (n > max_n)
    {
        // Possible overflow
        return;
    }
    // LCOV_EXCL_STOP
    const auto offset = a.advance(sizeof(subsize_s_t) + n * sizeof(S));
    serialize(offset, a, serial.offset);
    if (offset != 0U)
    {
        // Suppress AUTOSAR C++14 A4-7-1 rule findings. This rule stated: "An integer expression shall not lead to data
        // loss."
        // Justification: the comparison with max_n, ensure no data overflow
        // coverity[autosar_cpp14_a4_7_1_violation]
        const auto subsize = static_cast<typename A::subsize_t>(n * sizeof(S));
        serialize(subsize, a, *a.template address<subsize_s_t>(offset));
        S* string_size_location =
            a.template address<S>(static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t)));
        for (std::size_t i = 0UL; i != n; ++i)
        {
            /* KW_SUPPRESS_START:MISRA.PTR.ARITH:Needed to get offset from this location */
            // Suppress AUTOSAR C++14 M5-0-15 rule findings. This rule stated: "indexing shall be the only
            // form of pointer arithmetic"
            // False positive, no pointer arithmetic applied to pointer string_size_location
            // coverity[autosar_cpp14_m5_0_15_violation]
            serialize(*(t.cbegin() + static_cast<std::ptrdiff_t>(i)), a, string_size_location[i]);
            /* KW_SUPPRESS_END:MISRA.PTR.ARITH:Needed to get offset from this location */
        }
    }
}

template <typename A, typename S, typename T, std::enable_if_t<(std::is_integral<T>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const std::vector<T>& t, serializer_helper<A>& a, vector_serialized<A, S>& serial)
{
    static_assert(sizeof(S) <= std::numeric_limits<std::size_t>::max(), "S is too large");
    using subsize_s_t = subsize_serialized<A>;
    const auto n = t.size();
    static_assert(std::numeric_limits<typename A::offset_t>::max() >= sizeof(subsize_s_t), "Type limits error");
    constexpr size_t max_n = (std::numeric_limits<typename A::offset_t>::max() - sizeof(subsize_s_t)) / sizeof(S);
    // We can't achieve the TRUE case for the below condition due to:
    // 1- The above assertions.
    // 2- Even if we wrote a test case it will be useless and without any expectation or assertion because
    //    the function will return gracefully without any return values or even without a failure.
    // LCOV_EXCL_START
    if (n > max_n)
    {
        // Possible overflow
        return;
    }
    // LCOV_EXCL_STOP
    const auto offset = a.advance(sizeof(subsize_s_t) + n * sizeof(S));
    serialize(offset, a, serial.offset);
    if (offset != 0UL)
    {
        /*
        Deviation from Rule A4-7-1:
        - An integer expression shall not lead to data loss.
        Justification:
        - we already checked the overflow before statement and,
        - if it happens so there is a data loss and we'll return.
        - This will depend on passing the size of vector and which data type we use inside it.
        */
        // coverity[autosar_cpp14_a4_7_1_violation]
        const auto subsize = static_cast<typename A::subsize_t>(n * sizeof(S));
        serialize(subsize, a, *a.template address<subsize_s_t>(offset));
        S* string_size_location =
            a.template address<S>(static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t)));
        if (n > 0UL)
        {  //  prevents some warnings for a sanity about accessing zero sized container
            auto destination = score::cpp::span<S>{string_size_location, static_cast<typename score::cpp::span<S>::size_type>(n)};
            const auto source = details::cast_to_source_serializable_data_span<S>(t.data(), n);
            std::ignore = std::copy(source.begin(), source.end(), destination.begin());
        }
    }
}

template <typename A, typename S, typename T>
inline void deserialize(const vector_serialized<A, S>& serial, deserializer_helper<A>& a, T& t)
{
    using subsize_s_t = const subsize_serialized<A>;
    typename A::offset_t offset;
    deserialize(serial.offset, a, offset);
    if (offset == 0)
    {
        a.setZeroOffset();
        detail::clear(t);
        return;
    }
    const auto vector_size_location = a.template address<subsize_s_t>(offset);
    if (vector_size_location == nullptr)
    {
        // error condition already set by a.address()
        detail::clear(t);
        return;
    }
    typename A::subsize_t subsize;
    deserialize(*vector_size_location, a, subsize);
    const std::size_t n = subsize / sizeof(S);

    const auto vector_contents_offset = static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t));
    const S* const vector_contents_address = a.template address<const S>(vector_contents_offset, n);
    if (vector_contents_address == nullptr)
    {
        detail::clear(t);
        return;
    }
    if (t.max_size() < n)
    {
        a.setInvalidFormat();
        detail::clear(t);
        return;
    }
    detail::resize(t, n);
    for (std::size_t i = 0; i != n; ++i)
    {
        /* KW_SUPPRESS_START:MISRA.PTR.ARITH:Needed to get offset from this location */
        deserialize(vector_contents_address[i], a, *(t.begin() + static_cast<std::ptrdiff_t>(i)));
        /* KW_SUPPRESS_END:MISRA.PTR.ARITH:Needed to get offset from this location */
    }
}

// Vector specialization to optimize for integral types
template <typename A, typename S, typename T, std::enable_if_t<(std::is_integral<T>::value), std::int32_t> = 0>
/*
        Deviation from Rule M3-2-2:
        - The One Definition Rule shall not be violated
        Justification:
        - This is false positive, Overload signatures are different.
*/
// coverity[autosar_cpp14_m3_2_2_violation: FALSE]
inline void deserialize(const vector_serialized<A, S>& serial, deserializer_helper<A>& a, std::vector<T>& t)
{
    using subsize_s_t = const subsize_serialized<A>;
    typename A::offset_t offset;
    deserialize(serial.offset, a, offset);
    if (offset == 0UL)
    {
        a.setZeroOffset();
        detail::clear(t);
        return;
    }
    const auto vector_size_location = a.template address<subsize_s_t>(offset);
    if (vector_size_location == nullptr)
    {
        // error condition already set by a.address()
        detail::clear(t);
        return;
    }
    typename A::subsize_t subsize;
    deserialize(*vector_size_location, a, subsize);
    const std::size_t n = static_cast<std::size_t>(subsize) / sizeof(S);

    const auto vector_contents_offset = static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t));
    const auto vector_contents_address = a.template address<const S>(vector_contents_offset, n);
    if (vector_contents_address == nullptr)
    {
        detail::clear(t);
        return;
    }
    detail::resize(t, n);
    // 'n' variable is a result of dividing 'subsize' bt sizeof(S). Since the numerator can't be zero, the result
    // can't be zero. And, since neither the numerator or the denominator can be negative, the result can't
    // be negative. So, the False condition can't be tested.
    if (n > 0UL)  // LCOV_EXCL_BR_LINE
    {
        //  const cast to suggest that source data shall not be modified
        //  static cast to conform with score::cpp::span type
        const auto source = score::cpp::span<const S>{const_cast<const S*>(vector_contents_address),
                                               static_cast<typename score::cpp::span<S>::size_type>(n)};
        const auto destination = details::cast_to_destination_serializable_data_span<S>(t.data(), n);
        std::ignore = std::copy(source.begin(), source.end(), destination.begin());
    }
}
// string_serialized

template <typename A>
struct string_serialized
{
    offset_serialized<A> offset;
};

template <typename A>
struct is_serialized_type<string_serialized<A>> : public std::true_type
{
};

template <typename A, typename T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const T& t, serializer_helper<A>& a, string_serialized<A>& serial)
{
    using subsize_s_t = subsize_serialized<A>;
    auto n = t.size();
    constexpr auto max_n =
        std::numeric_limits<typename A::offset_t>::max() - static_cast<typename A::offset_t>(sizeof(subsize_s_t) - 1UL);
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure. Also, it's not easy to write unit test to meet this condition.
    // LCOV_EXCL_START
    if (n > max_n)
    {
        // Possible overflow
        return;
    }
    // LCOV_EXCL_STOP
    n += 1UL;  // +1 for null terminator for string
    const auto offset = a.advance(sizeof(subsize_s_t) + n);
    serialize(offset, a, serial.offset);
    if (offset != 0UL)
    {
        const auto subsize = static_cast<typename A::subsize_t>(n);
        serialize(subsize, a, *a.template address<subsize_s_t>(offset));
        OneByte* const string_size_location =
            a.template address<OneByte>(static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t)));
        std::ignore = std::memcpy(string_size_location, t.data(), n);
    }
}

template <typename A, typename T>
inline void deserialize(const string_serialized<A>& serial, deserializer_helper<A>& a, T& t)
{
    using subsize_s_t = const subsize_serialized<A>;
    typename A::offset_t offset;
    deserialize(serial.offset, a, offset);
    if (offset == 0UL)
    {
        a.setZeroOffset();
        t.resize(0UL);
        return;
    }

    const auto string_size_location = a.template address<subsize_s_t>(offset);
    if (string_size_location == nullptr)
    {
        // error condition already set by a.address()
        t.resize(0UL);
        return;
    }

    typename A::subsize_t subsize{0UL};
    deserialize(*string_size_location, a, subsize);
    const std::size_t n = subsize;
    if (n == 0UL)
    {
        a.setInvalidFormat();
        t.resize(0UL);
        return;
    }

    const auto string_content_offset = static_cast<typename A::offset_t>(offset + sizeof(subsize_s_t));
    const OneByte* const string_address = a.template address<const OneByte>(string_content_offset, n);
    if (string_address == nullptr)
    {
        // error condition already set by a.address()
        t.resize(0UL);
        return;
    }
    std::ignore = t.assign(string_address, n - 1U);
}

// serializing parameter packs

template <typename S>
struct pack_serialized
{
    static_assert(is_serialized_type<std::remove_cv_t<S>>::value, "encapsulated type is not serialized type");
    S pack;
};

template <typename S>
struct is_serialized_type<pack_serialized<S>> : public std::true_type
{
};

template <typename A, typename S, typename T>
inline void serialize_parameter_pack(serializer_helper<A>& a, S& serial, const T& t)
{
    serialize(t, a, serial);
}

template <typename A, typename S1, typename S2, typename T1, typename T2, typename... Args>
inline void serialize_parameter_pack(serializer_helper<A>& a,
                                     pair_serialized<S1, S2>& serial,
                                     const T1& t1,
                                     const T2& t2,
                                     const Args&... args)
{
    serialize(t1, a, serial.first);
    serialize_parameter_pack(a, serial.second, t2, args...);
}

template <typename A, typename S, typename T>
inline void deserialize_parameter_pack(const S& serial, deserializer_helper<A>& a, T& t)
{
    deserialize(serial, a, t);
}

template <typename A, typename S1, typename S2, typename T1, typename T2, typename... Args>
inline void deserialize_parameter_pack(const pair_serialized<S1, S2>& serial,
                                       deserializer_helper<A>& a,
                                       T1& t1,
                                       T2& t2,
                                       Args&... args)
{
    deserialize(serial.first, a, t1);
    deserialize_parameter_pack(serial.second, a, t2, args...);
}

// serializing tuples

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
template <typename A, typename S, typename... Args, std::size_t... I>
inline void serialize_tuple_start(const std::tuple<Args...>& t,
                                  serializer_helper<A>& a,
                                  S& serial,
                                  std::index_sequence<I...> /*unused*/)
{
    serialize_parameter_pack(a, serial.pack, std::get<I>(t)...);
}

template <typename A, typename S, typename... Args>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const std::tuple<Args...>& t, serializer_helper<A>& a, S& serial)
{
    serialize_tuple_start(t, a, serial, std::make_index_sequence<sizeof...(Args)>());
}

template <typename A, typename S, typename... Args, std::size_t... I>
inline void deserialize_tuple_start(const S& serial,
                                    deserializer_helper<A>& a,
                                    std::tuple<Args...>& t,
                                    std::index_sequence<I...> /*unused*/)
{
    deserialize_parameter_pack(serial.pack, a, std::get<I>(t)...);
}

template <typename A, typename S, typename... Args>
inline void deserialize(const S& serial, deserializer_helper<A>& a, std::tuple<Args...>& t)
{
    deserialize_tuple_start(serial, a, t, std::make_index_sequence<sizeof...(Args)>());
}

// serializing structs

template <typename A, typename S>
struct struct_serializer_visitor
{
  public:
    /* KW_SUPPRESS_START: MISRA.MEMB.NOT_PRIVATE: template struct field used in another template function. */
    /*
       Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
       particular struct. The Type is Simple Template type and does not require invariance (interface OR custom
       behavior) as per the design. Moreover the type is ONLY used internally under the namespace Visitor.
    */
    // coverity[autosar_cpp14_m11_0_1_violation]
    serializer_helper<A>& a;
    // coverity[autosar_cpp14_m11_0_1_violation]
    S& serializer;
    /* KW_SUPPRESS_END: MISRA.MEMB.NOT_PRIVATE: template struct field used in another template function. */
};

template <typename A, typename S, typename T, typename... Args>
// Unused variables needed for correct template deduction
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
inline void visit_as_struct(const struct_serializer_visitor<A, S>& visitor, T&& /*unused*/, const Args&... args)
{
    serialize_parameter_pack(visitor.a, visitor.serializer.pack, args...);
}

template <typename A, typename S, typename T, typename V = decltype(get_struct_visitable<T>())>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void serialize(const T& t, serializer_helper<A>& a, S& serial)
{
    struct_serializer_visitor<A, S> visitor{a, serial};
    V::visit(visitor, t);
}

template <typename A, typename S>
struct struct_deserializer_visitor
{
    /* KW_SUPPRESS_START: MISRA.MEMB.NOT_PRIVATE: Intended by the design of this templated code. */
    /*
       Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
       particular struct. The Type is Simple Template type and does not require invariance (interface OR custom
       behavior) as per the design. Moreover the type is ONLY used internally under the namespace Visitor.
    */
    // coverity[autosar_cpp14_m11_0_1_violation]
    deserializer_helper<A>& a;
    // coverity[autosar_cpp14_m11_0_1_violation]
    const S& s;
    /* KW_SUPPRESS_END: MISRA.MEMB.NOT_PRIVATE: Intended by the design of this templated code. */
};

template <typename A, typename S, typename T, typename... Args>
// Unused variables needed for correct template deduction
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
inline void visit_as_struct(struct_deserializer_visitor<A, S> visitor, T&& /*unused*/, Args&... args)
{
    deserialize_parameter_pack(visitor.s.pack, visitor.a, args...);
}

template <typename A, typename S, typename T, typename V = decltype(get_struct_visitable<T>())>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
inline void deserialize(const S& serial, deserializer_helper<A>& a, T& t)
{
    struct_deserializer_visitor<A, S> visitor{a, serial};
    V::visit(visitor, t);
}

// serialized_visitor (compile-time creation of serialized types)

template <typename A>
struct serialized_visitor
{
};

template <typename A, typename T>
using serialized_descriptor_t =
    decltype((::score::common::visitor::visit(std::declval<serialized_visitor<A>&>(), std::declval<T&>())));

template <typename A, typename T>
using serialized_t = typename serialized_descriptor_t<A, T>::payload_type;

template <typename Tag, typename T>
/*
    Deviation from Rule A11-0-2:
    - A type defined as struct shall: (1) provide only public data members, (2)
      not provide any special member functions or methods, (3) not be a base of
      another struct or class, (4) not inherit from another struct or class.
    Deviation from Rule M11-0-1:
    - Member data in non-POD class types shall be private.

    Justification:
    - Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
      particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
      design.
*/
// coverity[autosar_cpp14_a11_0_2_violation]
struct memcpy_serialized_descriptor
{
    using payload_tag = Tag;
    using payload_type = memcpy_serialized<sizeof(T)>;
};

/* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */
template <typename A,
          typename T,
          std::enable_if_t<(std::is_integral<T>::value) && (std::is_signed<T>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::signed_le, T>();
}

template <typename A,
          typename T,
          std::enable_if_t<(std::is_integral<T>::value) && (!std::is_signed<T>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::unsigned_le, T>();
}

template <typename A, typename T, std::enable_if_t<std::is_floating_point<T>::value, std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::ieee754_float_le, T>();
}

template <typename A,
          typename T,
          std::enable_if_t<(std::is_enum<T>::value) && (std::is_signed<T>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::signed_le, T>();
}

template <typename A,
          typename T,
          std::enable_if_t<(std::is_enum<T>::value) && (!std::is_signed<T>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::unsigned_le, T>();
}
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */
/* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */

template <typename A, size_t N>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, std::bitset<N>& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::bitset, uint64_t>();
}

template <typename A,
          typename Rep,
          typename Period,
          std::enable_if_t<(std::is_integral<Rep>::value) && (std::is_signed<Rep>::value), std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, std::chrono::duration<Rep, Period>& /*unused*/)
{
    return memcpy_serialized_descriptor<payload_tags::signed_le, Rep>();
}

template <typename A, typename T, size_t N>
/*
    Deviation from Rule A11-0-2:
    - A type defined as struct shall: (1) provide only public data members, (2)
      not provide any special member functions or methods, (3) not be a base of
      another struct or class, (4) not inherit from another struct or class.
    Deviation from Rule M11-0-1:
    - Member data in non-POD class types shall be private.

    Justification:
    - Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
      particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
      design.
*/
// coverity[autosar_cpp14_a11_0_2_violation]
struct array_serialized_descriptor
{
    using payload_tag = payload_tags::array;
    using element_type = serialized_descriptor_t<A, T>;
    using payload_type = array_serialized<typename element_type::payload_type, N>;
    static constexpr size_t element_number = N;
};

/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE: intentionally */
template <typename A, typename T, size_t N>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const T (&/*unused*/)[N])
{  // NOLINT(modernize-avoid-c-arrays) intentionally
    return array_serialized_descriptor<A, T, N>();
}
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE: intentionally */

template <typename A, typename T, size_t N>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const std::array<T, N>& /*unused*/)
{
    return array_serialized_descriptor<A, T, N>();
}

template <typename A>
/*
    Deviation from Rule A11-0-2:
    - A type defined as struct shall: (1) provide only public data members, (2)
      not provide any special member functions or methods, (3) not be a base of
      another struct or class, (4) not inherit from another struct or class.
    Deviation from Rule M11-0-1:
    - Member data in non-POD class types shall be private.

    Justification:
    - Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
      particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
      design.
*/
// coverity[autosar_cpp14_a11_0_2_violation]
struct string_serialized_descriptor
{
    using payload_tag = payload_tags::string;
    using payload_type = string_serialized<A>;
};

template <typename A, typename CharT, typename Traits, typename Alloc>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const std::basic_string<CharT, Traits, Alloc>& /*unused*/)
{
    return string_serialized_descriptor<A>();
}

template <typename A, typename CharT, typename Traits, typename Alloc>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, std::basic_string<CharT, Traits, Alloc>& /*unused*/)
{
    return string_serialized_descriptor<A>();
}

template <typename A, typename T>
struct vector_serialized_descriptor
{
    using payload_tag = payload_tags::vector;
    using element_type = serialized_descriptor_t<A, T>;
    using payload_type = vector_serialized<A, typename element_type::payload_type>;
};

/* KW_SUPPRESS_START:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */
template <typename A,
          typename T,
          std::enable_if_t<::score::common::visitor::is_vector_serializable<T>::value, std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const T& /*unused*/)
{
    return vector_serialized_descriptor<A, typename T::value_type>();
}

template <typename A,
          typename T,
          std::enable_if_t<::score::common::visitor::is_vector_serializable<T>::value, std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return vector_serialized_descriptor<A, typename T::value_type>();
}
/* KW_SUPPRESS_END:AUTOSAR.STYLE.SINGLE_STMT_PER_LINE: false positive */

template <typename A, typename T>
inline auto visit_parameter_pack(serialized_visitor<A>& /*unused*/, T& /*unused*/)
{
    return serialized_t<A, T>();
}

template <typename A, typename T, typename... Args>
/*
  * T parameter not used but it is defined in the base function visit_parameter_pack(A, T)
    and the function with args... allowing it to accept multiple arguments,
    It is recursively return serialized_t<A, T, Args> until reach to base case visit_parameter_pack(A, T),
  * Unused variables needed for correct template deduction
*/
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
// coverity[autosar_cpp14_a0_1_4_violation]
inline auto visit_parameter_pack(serialized_visitor<A>& visitor, T& /*unused*/, Args&&... args)
{
    return pair_serialized<serialized_t<A, T>,
                           decltype((visit_parameter_pack(visitor, std::forward<Args>(args)...)))>();
}

template <typename A, typename D, typename... Ts>
/*
    Deviation from Rule A11-0-2:
    - A type defined as struct shall: (1) provide only public data members, (2)
      not provide any special member functions or methods, (3) not be a base of
      another struct or class, (4) not inherit from another struct or class.
    Deviation from Rule M11-0-1:
    - Member data in non-POD class types shall be private.

    Justification:
    - Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for this
      particular struct. The Type is simple and does not require invariance (interface OR custom behavior) as per the
      design.
*/
// coverity[autosar_cpp14_a11_0_2_violation]
struct pack_serialized_descriptor
{
    using payload_tag = payload_tags::pack;
    using element_types = std::tuple<serialized_descriptor_t<A, Ts>...>;
    using payload_type =
        pack_serialized<decltype(visit_parameter_pack(std::declval<serialized_visitor<A>&>(), std::declval<Ts&>()...))>;
    using pack_desc = D;
    // It is used by outside repo.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    static constexpr size_t element_number = sizeof...(Ts);
};

class default_pack_desc
{
  public:
    static std::string name()
    {
        return std::string();
    }
    static const OneByte* field_name(std::size_t /*unused*/)
    {
        return "";
    }
};

class optional_pack_desc
{
  public:
    static std::string name()
    {
        return std::string();
    }
    static const OneByte* field_name(std::size_t index)
    {
        /*
        The mapping originates in
        broken_link_g/swh/ddad/blob/ff4cddc43777f0d3746669b4c34a9cf135dad88e/score/static_reflection_with_serialization/serialization/include/serialization/visit_serialize.h#L283-L288
        to give score::cpp::optional type's has_value flag and data a field name which is needed for fibex
        generation. This is needed since score::cpp::optional's member variables are private and it is not
        plausible to change it to make them traceable.
        */

        const char* result;

        switch (index)
        {
            case 0U:
            {
                result = "has_value";
                break;
            }
            case 1U:
            {
                result = "data";
                break;
            }
            default:
            {
                result = "";
                break;
            }
        }

        return result;
    }  // namespace visitor
};     // namespace common

template <typename A, typename T1, typename T2>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const std::pair<T1, T2>& /*unused*/)
{
    return pack_serialized_descriptor<A, default_pack_desc, T1, T2>();
}

template <typename A, typename T1, typename T2>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, std::pair<T1, T2>& /*unused*/)
{
    return pack_serialized_descriptor<A, default_pack_desc, T1, T2>();
}

template <typename A, typename... T, std::size_t... I>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
inline auto visit_tuple_start(serialized_visitor<A>& visitor, std::tuple<T...>& t, std::index_sequence<I...> /*unused*/)
{
    return visit_parameter_pack(visitor, std::get<I>(t)...);
}

template <typename A, typename... Ts>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const std::tuple<Ts...>& /*unused*/)
{
    return pack_serialized_descriptor<A, default_pack_desc, Ts...>();
}

template <typename A, typename... Ts>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, std::tuple<Ts...>& /*unused*/)
{
    return pack_serialized_descriptor<A, default_pack_desc, Ts...>();
}

template <typename A>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(serialized_visitor<A>& /*unused*/, const std::tuple<>& /*unused*/)
{
}

template <typename A>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(serialized_visitor<A>& /*unused*/, std::tuple<>& /*unused*/)
{
}

template <typename A, typename S, typename... Args>
// Unused variables needed for correct template deduction
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
inline auto visit_as_struct(serialized_visitor<A>& /*unused*/, S&& /*unused*/, Args&&... /*unused*/)
{
    // Unused variables needed for correct template deduction
    // coverity[autosar_cpp14_a13_3_1_violation : FALSE]
    return pack_serialized_descriptor<A, struct_visitable<S>, Args...>();
}

template <typename A, typename T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline auto visit_as(serialized_visitor<A>& /*unused*/, const score::cpp::optional<T>& /*unused*/)
{
    return pack_serialized_descriptor<A, optional_pack_desc, char, T>();
}
/* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */

// This is false positive, because it's declared in this file in namespace score::common::visitor.
// so different namespaces are used to organize and encapsulate code,
// allowing for the same name to be used in different contexts without conflict.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class deserialization_result_t
{
  public:
    deserialization_result_t(const bool out_of_bounds, const bool invalid_format, const bool zero_offset)
        : out_of_bounds_{out_of_bounds}, invalid_format_{invalid_format}, zero_offset_{zero_offset}
    {
    }

    explicit operator bool() const
    {
        return !((out_of_bounds_ || invalid_format_) || zero_offset_);
    }

    bool getOutOfBounds() const
    {
        return out_of_bounds_;
    }
    bool getInvalidFormat() const
    {
        return invalid_format_;
    }
    bool getZeroOffset() const
    {
        return zero_offset_;
    }

    /* KW_SUPPRESS_START: MISRA.USE.EXPANSION: False positive: it is not macro. */
  private:
    /* KW_SUPPRESS_END: MISRA.USE.EXPANSION: False positive: it is not macro. */
    /* KW_SUPPRESS_START: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
    // This flag is set if tried to deserialize behind the boundary of the data array
    const bool out_of_bounds_;

    // This flag is set if the size of the dynamic element contents is 0
    const bool invalid_format_;

    // This flag is set if the offset of a dynamic element (in static payload) is 0 (as a result of too little buffer)
    const bool zero_offset_;
    /* KW_SUPPRESS_END: MISRA.MEMB.NOT_PRIVATE: False positive: it is private member. */
};

// serializer_t (encapsulating the visitors and the helpers)

template <typename A>
// This is false positive, because it's declared in this file in namespace score::common::visitor.
// so different namespaces are used to organize and encapsulate code,
// allowing for the same name to be used in different contexts without conflict.
// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class serializer_t
{
  public:
    using offset_t = typename A::offset_t;

    /// \public
    /// \thread-safe
    template <typename T>
    static offset_t serialize(const T& t, OneByte* const data, offset_t size)
    {
        using serialized_type = serialized_t<A, T>;
        const auto sizeof_serialized_type = sizeof(serialized_type);
        static_assert(sizeof_serialized_type > 0, "sizeof_serialized_type cannot be 0");
        if (sizeof_serialized_type == 0 || sizeof_serialized_type > size)
        {
            return 0;
        }

        // cast to different pointer type
        serializer_helper<A> a(reinterpret_cast<std::uint8_t* const>(data), size, sizeof_serialized_type);
        // cast to different pointer type
        ::score::common::visitor::serialize(t, a, *reinterpret_cast<serialized_type* const>(data));
        return a.total();
    }

    /// \public
    /// \thread-safe
    template <typename T>
    static offset_t serialize(const T& t, std::uint8_t* const data, offset_t size)
    {
        using serialized_type = serialized_t<A, T>;
        auto sizeof_serialized_type = sizeof(serialized_type);
        if ((sizeof_serialized_type == 0UL) || (sizeof_serialized_type > static_cast<std::size_t>(size)))
        {
            return static_cast<offset_t>(0);
        }
        // cast to different pointer type
        serializer_helper<A> a(data, size, static_cast<offset_t>(sizeof_serialized_type));
        // cast to different pointer type
        // coverity[autosar_cpp14_a5_2_4_violation]
        ::score::common::visitor::serialize(t, a, *reinterpret_cast<serialized_type* const>(data));
        return a.total();
    }
    template <typename T>
    static deserialization_result_t deserialize(const OneByte* const data, offset_t size, T& t)
    {
        using serialized_type = serialized_t<A, T>;
        // cast to different pointer type
        deserializer_helper<A> a(reinterpret_cast<const std::uint8_t* const>(data), size);
        if (sizeof(serialized_type) > size)
        {
            return deserialization_result_t{true, false, false};
        }
        // cast to different pointer type
        ::score::common::visitor::deserialize(*reinterpret_cast<const serialized_type* const>(data), a, t);

        return deserialization_result_t{a.getOutOfBounds(), a.getInvalidFormat(), a.getZeroOffset()};
    }
    template <typename T>
    static deserialization_result_t deserialize(const std::uint8_t* const data, offset_t size, T& t)
    {
        using serialized_type = serialized_t<A, T>;
        deserializer_helper<A> a(data, size);
        // Both True and False cases are already tested as shown in the coverage report screenshot there
        // broken_link_j/Ticket-175753?focusedId=18242799&page=com.atlassian.jira
        //      plugin.system.issuetabpanels:comment-tabpanel#comment-18242799
        // But the LCOV complains that the True case didn't covered. So, we are going to exclude the branch
        // from the coverage report.
        if (sizeof(serialized_type) > size)  // LCOV_EXCL_BR_LINE
        {
            return deserialization_result_t{true, false, false};
        }
        /* KW_SUPPRESS_START: MISRA.CAST.CONST: False positive: constness is preserved. */
        // cast to different pointer type
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) [score-qualified-nolint]
        /*

        */
        /*
                Deviation from Rule A5-2-4:
                - reinterpret_cast shall not be used.
                Justification:
                - Casting to different pointer type is needed because conversion
             from uint8_t* to serialized_type* const,
             because when get data as bytes(uint8_t*) here you need
             to convert it to back to it's type(serialized_type*)
             to maintain memory alignment for serialized_type
        */
        // coverity[autosar_cpp14_a5_2_4_violation]
        ::score::common::visitor::deserialize(*reinterpret_cast<const serialized_type* const>(data), a, t);
        /* KW_SUPPRESS_END: MISRA.CAST.CONST: False positive: constness is preserved. */

        return deserialization_result_t{a.getOutOfBounds(), a.getInvalidFormat(), a.getZeroOffset()};
    }
};

/* KW_SUPPRESS_END:AUTOSAR.FUNC.TMPL.EXPLICIT_SPEC: False positive: no specialization is used. */
/* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST: False positive for some cases: Variables in templates are modified. */
/* KW_SUPPRESS_END:MISRA.FUNC.UNUSEDPAR: False positive: all templates arguments are used. */
/* KW_SUPPRESS_END:AUTOSAR.CAST.REINTERPRET:Reinterpret needs to be used to cast to ptr */
/* KW_SUPPRESS_END:MISRA.LOGIC.OPERATOR.NOT_BOOL: False positive: all boolean expressions are correct. */

}  // namespace visitor

}  // namespace common

}  // namespace score

/* KW_SUPPRESS_START:MISRA.USE.DEFINE: intentionally. */
/* KW_SUPPRESS_START:MISRA.DEFINE.NOPARS: Correct usage of macro. */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) same as KW
/// \public
/*
    a16-0-1 : Defined to be global, and it will be enabled if  __VA_ARGS__ meets criteria,
    it's default value is int32_t.
    m16-0-6 : no need to add parentheses for parameters
*/
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define MEMCPY_SERIALIZABLE(tag, ...)                                                                                 \
    template <typename A,                                                                                             \
              typename T,                                                                                             \
              std::enable_if_t<std::is_same<__VA_ARGS__, typename std::remove_const<T>::type>::value, std::int32_t> = \
                  0>                                                                                                  \
    inline auto visit_as(::score::common::visitor::serialized_visitor<A>&, T&)                                          \
    {                                                                                                                 \
        return ::score::common::visitor::memcpy_serialized_descriptor<tag, T>();                                        \
    }                                                                                                                 \
    template <typename SizeType,                                                                                      \
              typename T,                                                                                             \
              std::enable_if_t<std::is_same<__VA_ARGS__, typename std::remove_const<T>::type>::value, std::int32_t> = \
                  0>                                                                                                  \
    inline void visit_as(::score::common::visitor::size_helper<SizeType>& v, T&)                                        \
    {                                                                                                                 \
        v.out += static_cast<SizeType>(sizeof(::score::common::visitor::memcpy_serialized<sizeof(std::decay_t<T>)>));   \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) tolerated per design
/// \public
/*
    a16-0-1 : Defined to be global, and it will be enabled if  __VA_ARGS__ meets criteria,
    it's default value is int32_t.
    m16-0-6 : no need to add parentheses for parameters
*/
// coverity[autosar_cpp14_a16_0_1_violation]
// coverity[autosar_cpp14_m16_0_6_violation]
#define MEMCPY_SERIALIZABLE_IF(tag, T, ...)                                                                         \
    template <typename A, typename T, std::enable_if_t<__VA_ARGS__, std::int32_t> = 0>                              \
    inline auto visit_as(::score::common::visitor::serialized_visitor<A>&, T&)                                        \
    {                                                                                                               \
        return ::score::common::visitor::memcpy_serialized_descriptor<tag, T>();                                      \
    }                                                                                                               \
    template <typename SizeType, typename T, std::enable_if_t<__VA_ARGS__, std::int32_t> = 0>                       \
    inline void visit_as(::score::common::visitor::size_helper<SizeType>& v, T&)                                      \
    {                                                                                                               \
        v.out += static_cast<SizeType>(sizeof(::score::common::visitor::memcpy_serialized<sizeof(std::decay_t<T>)>)); \
    }
/* KW_SUPPRESS_END:MISRA.DEFINE.NOPARS: Correct usage of macro. */
/* KW_SUPPRESS_END:MISRA.USE.DEFINE: intentionally. */

#endif  // SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SERIALIZE_H
