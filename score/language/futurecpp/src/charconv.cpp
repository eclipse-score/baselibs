/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/charconv.hpp>

#include <cstdint>
#include <type_traits>
#include <score/span.hpp>
#include <score/string_view.hpp>

namespace score::cpp
{

namespace
{

template <typename T, typename std::enable_if<std::is_signed<T>::value, bool>::type = true>
inline std::uint64_t to_chars_abs(const T value)
{
    // We cannot use std::abs here because in 2's complement systems, the absolute value of the most-negative value is
    // out of range, e.g., for 32-bit 2's complement type int, INT_MIN is -2147483648, but the would-be result
    // 2147483648 is greater than INT_MAX, which is 2147483647.
    // Instead we cast the value to unsigned and apply the unary negate which gives the value of `-a` is `2**b - a`,
    // where b is the number of bits after promotion. It is important to cast the value to the exact unsigned type to
    // handle the integral promotion in the case of char and short.
    // It gives the same codegen as `std::abs` https://godbolt.org/z/ssoMqWh4Y

    const auto u = static_cast<typename std::make_unsigned<T>::type>(value);
    return static_cast<typename std::make_unsigned<T>::type>((value < 0) ? -u : u);
}

template <typename T, typename std::enable_if<std::is_unsigned<T>::value, bool>::type = true>
inline std::uint64_t to_chars_abs(const T value)
{
    return value;
}

template <typename T>
to_chars_result to_chars_base16_impl(char* first, char* last, const T value, const int base)
{
    static_assert(std::is_integral<T>::value, "Must be an integral type");

    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(base == 16);

    const std::uint64_t value_convert{score::cpp::to_chars_abs(value)};
    const std::ptrdiff_t buffer_len{last - first};
    const std::ptrdiff_t max_hex_len{sizeof(T) * 2};
    const std::ptrdiff_t signed_offset{(std::is_signed<T>::value && (value < 0)) ? 1 : 0};
    const bool buffer_len_valid{buffer_len >= (max_hex_len + signed_offset)};

    if (!buffer_len_valid)
    {
        return {last, std::errc::value_too_large};
    }

    const string_view digits{"0123456789abcdef"};
    span<char> hex_string{first, static_cast<std::size_t>(buffer_len)};

    std::ptrdiff_t index_hex_string{0};
    if (signed_offset > 0)
    {
        score::cpp::at(hex_string, index_hex_string) = '-';
        ++index_hex_string;
    }

    for (std::ptrdiff_t index_value = (max_hex_len - 1) * 4; index_value >= 0; index_value -= 4)
    {
        const char digit{score::cpp::at(
            digits, static_cast<std::ptrdiff_t>((value_convert >> static_cast<std::size_t>(index_value)) & 0x0FU))};
        if (((index_hex_string - signed_offset) == 0) && (digit == '0') && (index_value != 0))
        {
            continue;
        }
        score::cpp::at(hex_string, index_hex_string) = digit;
        ++index_hex_string;
    }

    return {first + index_hex_string, std::errc{}};
}

} // namespace

to_chars_result to_chars(char* first, char* last, std::int8_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint8_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int16_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint16_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int32_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint32_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int64_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint64_t value, int base)
{
    return score::cpp::to_chars_base16_impl(first, last, value, base);
}

} // namespace score::cpp
