/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
/// @file
/// @brief  A collection of utility functions for JSON numbers.
///
/// @details  Provides string to number conversion functions.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H

#include "score/json/internal/parser/vajson/types.h"

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <system_error>
#include <type_traits>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// @brief            Checks if type is an integer
/// @details         Does not apply for bool.
/// @tparam          T                  Type to check.
template <typename T>
using IsInt = std::conjunction<std::negation<std::is_same<T, bool>>, std::is_integral<T>>;

namespace util
{
/// @brief Helper function to check if character is a digit, in ASCII/UTF-8.
inline auto IsDigit(const char chr) noexcept -> bool
{
    constexpr auto zero{std::char_traits<char>::to_int_type('0')};
    constexpr auto nine{std::char_traits<char>::to_int_type('9')};
    const auto intchr{std::char_traits<char>::to_int_type(chr)};
    return (intchr >= zero) && (intchr <= nine);
}

/// @brief Helper function to check if character is a hexadecimal digit, in ASCII/UTF-8.
inline auto IsHexDigit(const char chr) noexcept -> bool
{
    constexpr StringView kHexLetters{"abcdefABCDEF"};
    return IsDigit(chr) || (kHexLetters.find(chr) != StringView::npos);
}

/// @brief           Checks if a number uses the hexadecimal integer extension
/// @param[in]       view
///                  Number to check.
/// @return          True if the number starts with an optional minus sign followed by "0x" or "0X".
inline auto HasHexadecimalPrefix(StringView view) noexcept -> bool
{
    constexpr StringView kLowerCasePrefix{"0x"};
    constexpr StringView kUpperCasePrefix{"0X"};
    if ((!view.empty()) && (view.front() == '-'))
    {
        view.remove_prefix(1U);
    }
    const StringView prefix{view.substr(0U, kLowerCasePrefix.size())};
    return (prefix == kLowerCasePrefix) || (prefix == kUpperCasePrefix);
}

/// @brief           A parser for JSON numbers
/// @pre             This class assumes that all leading whitespace has been stripped.
class NumberParser
{
  public:
    /// @brief           Constructs a NumberParser
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    NumberParser() noexcept = default;

    /// @brief           Gets a pointer beyond the last parsed character
    /// @return          Pointer beyond the last parsed character.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto End() const& noexcept -> const char*
    {
        return end_;
    }

    // Deleted rvalue-qualified version to prevent dangling reference
    auto End() const&& noexcept -> const char* = delete;

    /// @brief           Converts a JSON integer to an integer type
    /// @details         Decimal integers are converted with base 10, hexadecimal integers with base 16.
    /// @tparam          Integer
    ///                  Type of integer to convert to.
    /// @param[in]       view
    ///                  Number to convert, without leading whitespace.
    /// @return          The Optional containing the converted number if the whole view is an integer that fits into
    ///                  Integer, or empty.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE
    /// @reentrant       TRUE
    /// @internal
    /// - Strip the optional minus sign and the hexadecimal prefix.
    /// - Convert the remaining digits to an unsigned magnitude with the explicit base.
    /// - If the whole view was consumed and the signed value fits into Integer:
    ///   - Return the value.
    /// - Otherwise:
    ///   - Return an empty Optional.
    /// @endinternal
    template <typename Integer>
    static auto ParseInteger(StringView view) noexcept -> Optional<Integer>
    {
        using Magnitude = std::make_unsigned_t<Integer>;
        constexpr std::int32_t kDecimalBase{10};
        constexpr std::int32_t kHexadecimalBase{16};
        constexpr std::size_t kHexadecimalPrefixLength{2U};

        const bool is_hexadecimal{HasHexadecimalPrefix(view)};
        const bool is_negative{(!view.empty()) && (view.front() == '-')};
        if (is_negative)
        {
            view.remove_prefix(1U);
        }
        if (is_hexadecimal)
        {
            view.remove_prefix(kHexadecimalPrefixLength);
        }

        Magnitude magnitude{};
        const char* const first{view.data()};
        const char* const last{std::next(first, static_cast<std::ptrdiff_t>(view.size()))};
        const std::from_chars_result conversion{
            std::from_chars(first, last, magnitude, is_hexadecimal ? kHexadecimalBase : kDecimalBase)};

        Optional<Integer> result{};
        if ((conversion.ec == std::errc{}) && (conversion.ptr == last))
        {
            if (!is_negative)
            {
                if (magnitude <= static_cast<Magnitude>(std::numeric_limits<Integer>::max()))
                {
                    static_cast<void>(result.emplace(static_cast<Integer>(magnitude)));
                }
            }
            else if constexpr (std::is_signed_v<Integer>)
            {
                const auto max_negative_magnitude{
                    static_cast<Magnitude>(static_cast<Magnitude>(std::numeric_limits<Integer>::max()) + 1U)};
                if (magnitude == Magnitude{0})
                {
                    static_cast<void>(result.emplace(Integer{0}));
                }
                else if (magnitude <= max_negative_magnitude)
                {
                    // Negate via magnitude - 1 so that the minimum value does not overflow.
                    const auto predecessor{static_cast<Integer>(magnitude - 1U)};
                    static_cast<void>(result.emplace(static_cast<Integer>(-predecessor - 1)));
                }
            }
        }
        return result;
    }

    /// @brief           Parses a StringView as a double
    /// @param[in]       view
    ///                  to parse.
    /// @return          The parsed number.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @internal
    /// - Convert the view to a double.
    /// @endinternal
    auto Double(StringView view) noexcept -> double
    {
        return std::strtod(view.data(), &this->end_);
    }

  private:
    /// @brief            Pointer beyond the last parsed character
    /// @details         Set by character conversion functions. Null if the parse did not work.
    char* end_{nullptr};
};

}  // namespace util
}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H
