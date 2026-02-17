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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_INT_CMP_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_INT_CMP_HPP

#include <type_traits>

namespace score::cpp
{
namespace detail
{

template <class T>
constexpr bool is_int_cmp_supported_type() noexcept
{
    return std::is_integral<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value;
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value == std::is_signed<U>::value),
                           bool> = true>
constexpr bool cmp_equal_impl(T lhs, U rhs) noexcept
{
    // Either both sides are signed, or both sides are unsigned. So any further implicit type conversion during the
    // comparison is safe.
    return lhs == rhs;
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value != std::is_signed<U>::value) && std::is_signed<T>::value,
                           bool> = true>
constexpr bool cmp_equal_impl(T lhs, U rhs) noexcept
{
    using lhs_unsigned_type = std::make_unsigned_t<T>;
    // If lhs is negative, it cannot be equal to rhs. Otherwise it fits into std::make_unsigned_t<T> and the static cast
    // is safe. After the static_cast, both sides are unsigned, so any further implicit type conversion during the
    // comparison is safe.
    return (lhs >= 0) && (static_cast<lhs_unsigned_type>(lhs) == rhs);
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value != std::is_signed<U>::value) && std::is_signed<U>::value,
                           bool> = true>
constexpr bool cmp_equal_impl(T lhs, U rhs) noexcept
{
    using rhs_unsigned_type = std::make_unsigned_t<U>;
    // If rhs is negative, it cannot be equal to lhs. Otherwise it fits into std::make_unsigned_t<T> and the static cast
    // is safe. After the static_cast, both sides are unsigned, so any further implicit type conversion during the
    // comparison is safe.
    return (rhs >= 0) && (static_cast<rhs_unsigned_type>(rhs) == lhs);
}

template <
    class T,
    class U,
    std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>(), bool> = true>
constexpr bool cmp_not_equal_impl(T lhs, U rhs) noexcept
{
    return !cmp_equal_impl(lhs, rhs);
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value == std::is_signed<U>::value),
                           bool> = true>
constexpr bool cmp_less_impl(T lhs, U rhs) noexcept
{
    return lhs < rhs;
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value != std::is_signed<U>::value) && std::is_signed<T>::value,
                           bool> = true>
constexpr bool cmp_less_impl(T lhs, U rhs) noexcept
{
    using lhs_unsigned_type = std::make_unsigned_t<T>;
    return (lhs < 0) || (static_cast<lhs_unsigned_type>(lhs) < rhs);
}

template <class T,
          class U,
          std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>() &&
                               (std::is_signed<T>::value != std::is_signed<U>::value) && std::is_signed<U>::value,
                           bool> = true>
constexpr bool cmp_less_impl(T lhs, U rhs) noexcept
{
    using rhs_unsigned_type = std::make_unsigned_t<U>;
    return (rhs >= 0) && (lhs < static_cast<rhs_unsigned_type>(rhs));
}

template <
    class T,
    class U,
    std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>(), bool> = true>
constexpr bool cmp_greater_impl(T lhs, U rhs) noexcept
{
    return cmp_less_impl(rhs, lhs);
}

template <
    class T,
    class U,
    std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>(), bool> = true>
constexpr bool cmp_less_equal_impl(T lhs, U rhs) noexcept
{
    return !cmp_less_impl(rhs, lhs);
}

template <
    class T,
    class U,
    std::enable_if_t<detail::is_int_cmp_supported_type<T>() && detail::is_int_cmp_supported_type<U>(), bool> = true>
constexpr bool cmp_greater_equal_impl(T lhs, U rhs) noexcept
{
    return !cmp_less_impl(lhs, rhs);
}

} // namespace detail

/// \brief Returns whether two integer values are equal. Unlike the builtin equality operators, this version
/// always provides a mathematically correct comparison in the case of integer types with different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare
/// \param rhs The second value to compare
/// \return Whether lhs and rhs are equal
template <class T, class U>
constexpr bool cmp_equal(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_equal_impl(lhs, rhs);
}

/// \brief Returns whether two integer values are not equal. Unlike the builtin equality operators, this version
/// always provides a mathematically correct comparison also in the case of integer types with different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare
/// \param rhs The second value to compare
/// \return Whether lhs and rhs are not equal
template <class T, class U>
constexpr bool cmp_not_equal(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_not_equal_impl(lhs, rhs);
}

/// \brief Returns whether the integer value lhs is less than the integer value rhs. Unlike the builtin equality
/// operators this version always provides a mathematically correct comparison also in the case of integer types with
/// different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare
/// \param rhs The second value to compare
/// \return Whether lhs is less than rhs
template <class T, class U>
constexpr bool cmp_less(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_less_impl(lhs, rhs);
}

/// \brief Returns whether the integer value lhs is greater than the integer value rhs. Unlike the builtin equality
/// operators this version always provides a mathematically correct comparison also in the case of integer types with
/// different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare
/// \param rhs The second value to compare
/// \return Whether lhs is greater than rhs
template <class T, class U>
constexpr bool cmp_greater(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_greater_impl(lhs, rhs);
}

/// \brief Returns whether the integer value lhs is less than or equal to the integer value rhs. Unlike the builtin
/// comparison operators this version always provides a mathematically correct comparison also in the case of integer
/// types with different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare
/// \param rhs The second value to compare
/// \return Whether lhs is less than or equal to rhs
template <class T, class U>
constexpr bool cmp_less_equal(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_less_equal_impl(lhs, rhs);
}

/// \brief Returns whether the integer value lhs is greater than or equal to the integer value rhs. Unlike the builtin
/// comparison operators this version always provides a mathematically correct comparison also in the case of
/// integer types with different signedness.
///
/// \pre T and U must not be non-integer types, character types or bool
/// \see https://en.cppreference.com/w/cpp/utility/intcmp.html
///
/// \tparam T The type of the first value to compare
/// \tparam U The type of the second value to compare
/// \param lhs The first value to compare \param rhs The second value to compare
/// \return Whether lhs is greater than or equal to rhs
template <class T, class U>
constexpr bool cmp_greater_equal(T lhs, U rhs) noexcept
{
    static_assert(detail::is_int_cmp_supported_type<T>(),
                  "T must not be a non-integer type, a character type, or bool");
    static_assert(detail::is_int_cmp_supported_type<U>(),
                  "U must not be a non-integer type, a character type, or bool");
    return detail::cmp_greater_equal_impl(lhs, rhs);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_UTILITY_INT_CMP_HPP
