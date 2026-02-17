/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Bit component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_BIT_HPP
#define SCORE_LANGUAGE_FUTURECPP_BIT_HPP

#include <score/private/bit/bit_cast.hpp> // IWYU pragma: export

#include <score/private/type_traits/is_unsigned_integer_type.hpp>
#include <limits>
#include <type_traits>
#include <score/assert.hpp>

namespace score::cpp
{

namespace detail
{

template <typename T>
constexpr int countl_zero_impl(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    constexpr int unsigned_digit_count{std::numeric_limits<unsigned>::digits};
    constexpr int digit_count{std::numeric_limits<T>::digits};
    // correction needed because of integral promotion
    return __builtin_clz(x) - (unsigned_digit_count - digit_count);
}

constexpr int countl_zero_impl(const unsigned int x) noexcept { return __builtin_clz(x); }

constexpr int countl_zero_impl(const unsigned long x) noexcept { return __builtin_clzl(x); }

constexpr int countl_zero_impl(const unsigned long long x) noexcept { return __builtin_clzll(x); }

template <typename T>
constexpr int countr_zero_impl(const T x) noexcept
{
    static_assert(is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return __builtin_ctz(x);
}

constexpr int countr_zero_impl(const unsigned long x) noexcept { return __builtin_ctzl(x); }

constexpr int countr_zero_impl(const unsigned long long x) noexcept { return __builtin_ctzll(x); }

template <typename T>
constexpr int popcount_impl(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return __builtin_popcount(x);
}

constexpr int popcount_impl(const unsigned long x) noexcept { return __builtin_popcountl(x); }

constexpr int popcount_impl(const unsigned long long x) noexcept { return __builtin_popcountll(x); }

} // namespace detail

///
/// \brief Bitwise left rotation.
/// \param x values of unsigned integer type.
/// \param s number of bit positions.
/// \return The result of bitwise left-rotating \c x by \c s positions.
/// \see https://en.cppreference.com/w/cpp/numeric/rotl
///
template <typename T>
constexpr T rotl(const T x, const int s) noexcept
{
    static_assert(std::is_unsigned<T>::value, "Bit rotation is defined for unsigned integers only");
    constexpr auto bit_count = std::numeric_limits<T>::digits;
    const auto r = s % bit_count;

    if (r == 0)
    {
        return x;
    }
    else if (r > 0)
    {
        return static_cast<T>((x << r) | (x >> (bit_count - r))); // left rotation
    }

    return static_cast<T>((x >> -r) | (x << (bit_count + r))); // right rotation
}

///
/// \brief Bitwise right rotation.
/// \param x values of unsigned integer type.
/// \param s number of bit positions.
/// \return The result of bitwise right-rotating \c x by \c s positions.
/// \see https://en.cppreference.com/w/cpp/numeric/rotr
///
template <typename T>
constexpr T rotr(const T x, const int s) noexcept
{
    return score::cpp::rotl<T>(x, -s);
}

/// \brief Returns the number of consecutive 0 bits in the value of x, starting from the most significant bit ("left").
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/countl_zero
///
/// \param x values of unsigned integer type
/// \return The number of consecutive 0 bits in the value of x, starting from the most significant bit.
template <typename T>
constexpr int countl_zero(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return (x == 0U) ? std::numeric_limits<T>::digits : score::cpp::detail::countl_zero_impl(x);
}

/// \brief Returns the number of consecutive 1 ("one") bits in the value of x, starting from the most significant bit
/// ("left").
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/countl_one
///
/// \param x values of unsigned integer type
/// \return The number of consecutive 1 bits in the value of x, starting from the most significant bit.
template <typename T>
constexpr int countl_one(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return score::cpp::countl_zero(static_cast<T>(~x));
}

/// \brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit
/// ("right").
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/countr_zero
///
/// \param x values of unsigned integer type
/// \return The number of consecutive 0 bits in the value of x, starting from the least significant bit.
template <typename T>
constexpr int countr_zero(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return (x == 0U) ? std::numeric_limits<T>::digits : score::cpp::detail::countr_zero_impl(x);
}

/// \brief Returns the number of consecutive 1 bits in the value of x, starting from the least significant bit
/// ("right").
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/countr_one
///
/// \param x values of unsigned integer type
/// \return The number of consecutive 1 bits in the value of x, starting from the least significant bit.
template <typename T>
constexpr int countr_one(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return score::cpp::countr_zero(static_cast<T>(~x));
}

/// \brief If x is not zero, calculates the number of bits needed to store the value x, that is, 1 + floor(log2(x)). If
/// x is zero, returns zero.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/bit_width
///
/// \param x unsigned integer value
/// \return Zero if x is zero; otherwise, one plus the base-2 logarithm of x, with any fractional part discarded.
template <typename T>
constexpr T bit_width(T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return static_cast<T>(std::numeric_limits<T>::digits - score::cpp::countl_zero(x));
}

/// \brief Calculates the smallest integral power of two that is not smaller than x.
///
/// If that value is not representable in T, the behavior is undefined. Call to this function is permitted in constant
/// evaluation only if the undefined behavior does not occur.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \pre x <= std::numeric_limits<T>::max() / 2 + 1
/// \see https://en.cppreference.com/w/cpp/numeric/bit_ceil
///
/// \param x values of unsigned integer type
/// \return The smallest integral power of two that is not smaller than x.
template <typename T>
constexpr T bit_ceil(const T x)
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(x <= std::numeric_limits<T>::max() / 2U + 1U);
    return (x <= 1U) ? T{1U} : static_cast<T>(T{1U} << score::cpp::bit_width(static_cast<T>(x - 1U)));
}

/// \brief If x is not zero, calculates the largest integral power of two that is not greater than x. If x is zero,
/// returns zero.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/bit_floor
///
/// \param x unsigned integer value
/// \return Zero if x is zero; otherwise, the largest integral power of two that is not greater than x.
template <typename T>
constexpr T bit_floor(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return (x == 0U) ? T{0U} : static_cast<T>(T{1U} << (score::cpp::bit_width(x) - 1U));
}

/// \brief Returns the number of 1 bits in the value of x.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/popcount
///
/// \param x values of unsigned integer type
/// \return The number of 1 bits in the value of x.
template <typename T>
constexpr int popcount(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return score::cpp::detail::popcount_impl(x);
}

/// \brief Checks if x is an integral power of two.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \see https://en.cppreference.com/w/cpp/numeric/has_single_bit
///
/// \param x values of unsigned integer type
/// \return true if x is an integral power of two; otherwise false.
template <typename T>
constexpr bool has_single_bit(const T x) noexcept
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    return score::cpp::popcount(x) == 1;
}

/// \brief Round up the specified non-negative integer \p n to a multiple of the integer m.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \pre has_single_bit(m)
/// \pre n <= std::numeric_limits<T>::max() - (m - 1U)
///
/// \param n Value to be rounded up.
/// \param m Value n is rounded up to the next multiple of m.
/// \return The smallest multiple of m not less than \p n.
template <typename T>
constexpr T align_up(const T n, const T m)
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(score::cpp::has_single_bit(m));
    const auto t = static_cast<T>(m - T{1U});
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n <= (std::numeric_limits<T>::max() - t));
    return static_cast<T>(n + static_cast<T>(static_cast<T>(-n) & t));
}

/// \brief Round down the specified non-negative integer \p n to a multiple of the integer m.
///
/// \pre T is an unsigned integer type (that is, unsigned char, unsigned short, unsigned int, unsigned long or unsigned
/// long long).
/// \pre has_single_bit(m)
///
/// \param n Value to be rounded down.
/// \param m Value n is rounded down to the next smaller multiple of m.
/// \return The next smaller multiple of m.
template <typename T>
constexpr T align_down(const T n, const T m)
{
    static_assert(detail::is_unsigned_integer_type<T>::value, "T must be an unsigned integer type.");
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(score::cpp::has_single_bit(m));
    return static_cast<T>(n & static_cast<T>(-m));
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_BIT_HPP
