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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_NUMERIC_SATURATE_CAST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_NUMERIC_SATURATE_CAST_HPP

#include <score/private/algorithm/clamp.hpp> // IWYU pragma: export
#include <score/private/utility/int_cmp.hpp> // IWYU pragma: export

#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

template <class T>
constexpr bool is_saturate_cast_supported_type() noexcept
{
    return std::is_integral<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value;
}

} // namespace detail

/// \brief Casts value from the integer type U to the integer type T. If the value cannot be represented in T, it is
/// clamped to the range of T.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/numeric/saturate_cast.html
///
/// \tparam T The integer type to cast to
/// \tparam U The integer type to cast from
/// \param value The value to cast
/// \return value casted to T and clamped to the range of T
template <class T,
          class U,
          std::enable_if_t<detail::is_saturate_cast_supported_type<T>() &&
                               detail::is_saturate_cast_supported_type<U>() && std::is_same<T, U>::value,
                           bool> = true>
constexpr T saturate_cast(U value) noexcept
{
    // T and U are identical => just return value
    return value;
}

/// \brief Casts value from the integer type U to the integer type T. If the value cannot be represented in T, it is
/// clamped to the range of T.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/numeric/saturate_cast.html
///
/// \tparam T The integer type to cast to
/// \tparam U The integer type to cast from
/// \param value The value to cast
/// \return value casted to T and clamped to the range of T
template <
    class T,
    class U,
    std::enable_if_t<detail::is_saturate_cast_supported_type<T>() && detail::is_saturate_cast_supported_type<U>() &&
                         (!std::is_same<T, U>::value &&
                          (detail::cmp_greater_equal_impl(std::numeric_limits<U>::lowest(),
                                                          std::numeric_limits<T>::lowest())) &&
                          (detail::cmp_less_equal_impl(std::numeric_limits<U>::max(), std::numeric_limits<T>::max()))),
                     bool> = true>
constexpr T saturate_cast(U value) noexcept
{
    // U completely fits inside T => just use static_cast
    return static_cast<T>(value);
}

/// \brief Casts value from the integer type U to the integer type T. If the value cannot be represented in T, it is
/// clamped to the range of T.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/numeric/saturate_cast.html
///
/// \tparam T The integer type to cast to
/// \tparam U The integer type to cast from
/// \param value The value to cast
/// \return value casted to T and clamped to the range of T
template <
    class T,
    class U,
    std::enable_if_t<detail::is_saturate_cast_supported_type<T>() && detail::is_saturate_cast_supported_type<U>() &&
                         (!std::is_same<T, U>::value &&
                          (detail::cmp_greater_equal_impl(std::numeric_limits<T>::lowest(),
                                                          std::numeric_limits<U>::lowest())) &&
                          (detail::cmp_less_equal_impl(std::numeric_limits<T>::max(), std::numeric_limits<U>::max()))),
                     bool> = true>
constexpr T saturate_cast(U value) noexcept
{
    // T completely fits inside U => clamp it as U to the range of T and then static_cast it to T
    return static_cast<T>(score::cpp::clamp(
        value, static_cast<U>(std::numeric_limits<T>::lowest()), static_cast<U>(std::numeric_limits<T>::max())));
}

/// \brief Casts value from the integer type U to the integer type T. If the value cannot be represented in T, it is
/// clamped to the range of T.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/numeric/saturate_cast.html
///
/// \tparam T The integer type to cast to
/// \tparam U The integer type to cast from
/// \param value The value to cast
/// \return value casted to T and clamped to the range of T
template <
    class T,
    class U,
    std::enable_if_t<detail::is_saturate_cast_supported_type<T>() && detail::is_saturate_cast_supported_type<U>() &&
                         (detail::cmp_less_impl(std::numeric_limits<T>::lowest(), std::numeric_limits<U>::lowest()) &&
                          detail::cmp_less_impl(std::numeric_limits<T>::max(), std::numeric_limits<U>::max())),
                     bool> = true>
constexpr T saturate_cast(U value) noexcept
{
    // Assert that both types overlap
    static_assert(cmp_less_equal(std::numeric_limits<U>::lowest(), std::numeric_limits<T>::max()),
                  "U's lower bound must be less than or equal to T's upper bound.");
    // Partial overlap and the lowest representable value of T is lower than U => clamp it as U such that the value is
    // not higher than the upper bound of T and then static_cast it to T
    return static_cast<T>(
        score::cpp::clamp(value, std::numeric_limits<U>::lowest(), static_cast<U>(std::numeric_limits<T>::max())));
}

/// \brief Casts value from the integer type U to the integer type T. If the value cannot be represented in T, it is
/// clamped to the range of T.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/numeric/saturate_cast.html
///
/// \tparam T The integer type to cast to
/// \tparam U The integer type to cast from
/// \param value The value to cast
/// \return value casted to T and clamped to the range of T
template <
    class T,
    class U,
    std::enable_if_t<detail::is_saturate_cast_supported_type<T>() && detail::is_saturate_cast_supported_type<U>() &&
                         (detail::cmp_less_impl(std::numeric_limits<U>::lowest(), std::numeric_limits<T>::lowest()) &&
                          detail::cmp_less_impl(std::numeric_limits<U>::max(), std::numeric_limits<T>::max())),
                     bool> = true>
constexpr T saturate_cast(U value) noexcept
{
    // Assert that both types overlap
    static_assert(cmp_less_equal(std::numeric_limits<T>::lowest(), std::numeric_limits<U>::max()),
                  "T's lower bound must be less than or equal to U's upper bound.");
    return static_cast<T>(
        // Partial overlap and the lowest representable value of U is lower than T => clamp it as U such that the value
        // is not smaller than the lower bound of T and then static_cast it to T
        score::cpp::clamp(value, static_cast<U>(std::numeric_limits<T>::lowest()), std::numeric_limits<U>::max()));
}

} // namespace score::cpp

#endif
