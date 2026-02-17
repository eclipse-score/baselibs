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
#ifndef SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SIZE_H
#define SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SIZE_H

#include "static_reflection_with_serialization/serialization/visit_type_traits.h"
#include "static_reflection_with_serialization/visitor/visit.h"
#include "static_reflection_with_serialization/visitor/visit_as_struct.h"

#include "score/assert.hpp"
#include <cstdint>
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

template <typename SizeType = uint64_t>
struct size_helper
{
    /* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
    static_assert(std::is_arithmetic<SizeType>::value, "Size type should be arithmetic");
    static_assert(!std::is_const<SizeType>::value, "Size type should not be const");
    /* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
    /*
      1 - It's not a POD type because of initialization of member variables.
      2 - Maintaining compatibility and avoiding performance overhead outweighs POD Type (class) based design for
          this particular struct. The Type is simple and does not require invariance (interface OR custom behavior)
      except for default initialization as per the design. Moreover the type is ONLY used internally under the namespace
      visitor and NOT exposed publicly; this is additionally guaranteed by the build system.
    */
    // coverity[autosar_cpp14_m11_0_1_violation]
    SizeType out{0UL};
    /**
     * Additional fixed bit offset for some types
     */
    // coverity[autosar_cpp14_m11_0_1_violation]
    SizeType vector_offset{0UL};
    // coverity[autosar_cpp14_m11_0_1_violation]
    SizeType string_offset{0UL};
};

/* KW_SUPPRESS_START:MISRA.LOGIC.NOT_BOOL: false positive */
template <typename SizeType,
          typename T,
          std::enable_if_t<(!std::is_pointer<std::decay_t<T>>::value) && (!std::is_class<std::decay_t<T>>::value),
                           std::int32_t> = 0>
/* KW_SUPPRESS_END:MISRA.LOGIC.NOT_BOOL: false positive */
/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, T&&)
{
    static_assert(sizeof(std::decay_t<T>) <= std::numeric_limits<SizeType>::max(), "size of T too large");
    /* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
    const auto new_size = v.out + static_cast<SizeType>(sizeof(std::decay_t<T>));
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure.
    if (new_size < v.out)  // LCOV_EXCL_BR_LINE
    {
        // no-op, Possible overflow! Keep existing value of v.out
    }
    else
    {
        v.out = new_size;
    }
}

template <typename SizeType, typename T, typename V = decltype(score::common::visitor::get_struct_visitable<T>())>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, T&& t)
{
    V::visit(v, t);
}

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
template <typename SizeType, typename S, typename... Args>
inline void visit_as_struct(size_helper<SizeType>& v, S&&, Args&&... args)
{
    /* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
    // pre-C++17 parameter pack folding idiom
    /* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE: intentionally */
    using expander = std::int32_t[];
    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE: intentionally */
    /* KW_SUPPRESS_START:MISRA.COMMA: False positive: correct usage of comma. */
    std::ignore = expander{(score::common::visitor::visit(v, std::forward<Args>(args)), 0)...};
    /* KW_SUPPRESS_END:MISRA.COMMA: False positive: correct usage of comma. */
}

template <typename SizeType, typename T1, typename T2>
// we to overload function, because we pass different types of T.
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const std::pair<T1, T2>& t)
{
    score::common::visitor::visit(v, t.first);
    score::common::visitor::visit(v, t.second);
}

template <typename SizeType, typename T>
inline void visit_tuple_continue(size_helper<SizeType>& v, const T& t)
{
    score::common::visitor::visit(v, t);
}

template <typename SizeType, typename T, typename... Args>
// It is template recursive function until reach to base call, so we need this overload.
// coverity[autosar_cpp14_a13_3_1_violation : FALSE]
inline void visit_tuple_continue(size_helper<SizeType>& v, const T& t, Args&&... args)
{
    score::common::visitor::visit(v, t);
    // static recursion version of parameter pack folding
    visit_tuple_continue(v, std::forward<Args>(args)...);
}

/* KW_SUPPRESS_START: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
template <typename SizeType, typename... T, std::size_t... I>
inline void visit_tuple_start(size_helper<SizeType>& v, const std::tuple<T...>& t, std::index_sequence<I...>)
{
    /* KW_SUPPRESS_END: MISRA.FUNC.UNUSEDPAR.UNNAMED: Unused variables needed for correct template deduction. */
    visit_tuple_continue(v, std::get<I>(t)...);
}

template <typename SizeType, typename... T>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const std::tuple<T...>& t)
{
    visit_tuple_start(v, t, std::make_index_sequence<sizeof...(T)>());
}

template <typename SizeType, typename C, typename T, typename Alloc>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const std::basic_string<C, T, Alloc>& t)
{
    // Suppress "AUTOSAR C++14 A4-7-1" The rule states: "An integer expression shall not lead to data loss"
    // Justification: data overflow is allowed as the overflow handling in the code
    // coverity[autosar_cpp14_a4_7_1_violation]
    const auto new_size = v.out + v.string_offset + static_cast<SizeType>(sizeof(uint16_t)) +
                          // coverity[autosar_cpp14_a4_7_1_violation]  see above
                          static_cast<SizeType>(t.size() * sizeof(std::string::value_type) + 1UL);
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure.
    if (new_size < v.out)  // LCOV_EXCL_BR_LINE
    {
        // no-op, Possible overflow! Keep existing value of v.out
    }
    else
    {
        v.out = new_size;
    }
}

template <typename SizeType,
          typename T,
          std::enable_if_t<::score::common::visitor::is_vector_serializable<T>::value, std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const T& t)
{
    // Suppress "AUTOSAR C++14 A4-7-1" The rule states: "An integer expression shall not lead to data loss"
    // Justification: data overflow is allowed as the overflow handling in the code
    // coverity[autosar_cpp14_a4_7_1_violation]
    const auto new_size = v.out + v.vector_offset + static_cast<SizeType>(sizeof(uint16_t));
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure.
    if (new_size < v.out)  // LCOV_EXCL_BR_LINE
    {
        // no-op, Possible overflow! Keep existing value of v.out
    }
    else
    {
        v.out = new_size;
    }
    // We are facing 'Decision couldn't be analyzed' for the below 'for' loop, it's phantom. So, we suppressed it.
    for (std::size_t i = 0UL; i < t.size(); ++i)  // LCOV_EXCL_BR_LINE
    {
        score::common::visitor::visit(v, *(t.cbegin() + static_cast<std::ptrdiff_t>(i)));
    }
}

//  optimized version of size serialization for integral types
template <typename SizeType, typename T, typename Alloc, std::enable_if_t<std::is_integral<T>::value, std::int32_t> = 0>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const std::vector<T, Alloc>& t)
{
    auto new_size = v.out + v.vector_offset;
    SCORE_LANGUAGE_FUTURECPP_ASSERT(new_size <= (std::numeric_limits<SizeType>::max() - static_cast<SizeType>(sizeof(uint16_t))));
    new_size += static_cast<SizeType>(sizeof(uint16_t));
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure.
    if (new_size < v.out)  // LCOV_EXCL_BR_LINE
    {
        // no-op, Possible overflow! Keep existing value of v.out
    }
    else
    {
        v.out = new_size;
    }
    // we checked the overflow after this statement and
    // if it happens so there is a data loss and we'll discard the value.
    // This will depend on passing the size of vector and which data type we use inside it.
    // coverity[autosar_cpp14_a4_7_1_violation]
    const auto new_all_elements_size = v.out + static_cast<decltype(v.out)>(sizeof(T) * t.size());
    // There is no benefit of writing unit test to cover the TRUE case of this condition, it will not have any
    // expectation or assertion because the function will return gracefully without any return values or even
    // a failure.
    if (new_all_elements_size < v.out)  // LCOV_EXCL_BR_LINE
    {
        // no-op, Possible overflow! Keep existing value of v.out
    }
    else
    {
        v.out = new_all_elements_size;
    }
}

template <typename SizeType, typename T, size_t N>
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const std::array<T, N>& t)
{
    for (size_t i = 0UL; i < N; ++i)
    {
        score::common::visitor::visit(v, t[i]);
    }
}

template <typename SizeType, typename T, size_t N>
/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE: intentionally */
// This is false positive, Overload signatures are different.
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
inline void visit_as(size_helper<SizeType>& v, const T (&t)[N])
{
    /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE: intentionally */
    for (size_t i = 0; i < N; ++i)
    {
        score::common::visitor::visit(v, t[i]);
    }
}

template <typename A>
class serialized_size_t
{
  public:
    /**
     * \brief Calculates size of serialized data in bytes.
     * \public
     * \thread-safe
     * @param data Data for which we want to calculate serialization size
     */
    template <typename SizeType, typename T>
    static SizeType serialized_size(const T& data)
    {
        static_assert(std::is_unsigned<SizeType>::value, "SizeType must be an unsigned type");
        size_helper<SizeType> v;

        v.string_offset = static_cast<SizeType>(sizeof(typename A::offset_t));
        v.vector_offset = v.string_offset;

        score::common::visitor::visit(v, data);

        return v.out;
    }
};

}  // namespace visitor
}  // namespace common
}  // namespace score

#endif  // SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_SIZE_H
