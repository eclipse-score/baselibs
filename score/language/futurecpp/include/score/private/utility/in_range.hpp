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

///
/// \file
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_RANGE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_RANGE_HPP

#include <score/private/utility/int_cmp.hpp> // IWYU pragma: export

#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

template <class T>
constexpr bool is_in_range_supported_type() noexcept
{
    return std::is_integral<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value;
}

} // namespace detail

/// \brief Returns whether the integer value of type U is representable by the integer type T
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/in_range.html
///
/// \tparam T The integer type
/// \tparam U The type of the integer value
/// \param value The value for which it is checked whether it is representable by T
/// \return Whether the integer value of type U is representable by the integer type T
template <class T, class U>
constexpr bool in_range(U value) noexcept
{
    static_assert(detail::is_in_range_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_in_range_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return (score::cpp::cmp_greater_equal(value, std::numeric_limits<T>::min())) &&
           score::cpp::cmp_less_equal(value, std::numeric_limits<T>::max());
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_IN_RANGE_HPP
