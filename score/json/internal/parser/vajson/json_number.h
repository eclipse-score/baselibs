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
/// @brief  A representation of a parsed JSON number.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_NUMBER_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_NUMBER_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "score/result/result.h"

#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/number_parser.h"
#include "score/json/internal/parser/vajson/types.h"

namespace score
{
namespace json
{
namespace vajson
{
/// @brief           A representation of a parsed JSON number
/// @trace           DSGN-JSON-Reader-Number-Formatting
class JsonNumber final
{
  public:
    /// @brief           Creates a new number
    /// @param[in]       view
    ///                  to parse.
    /// @return          A Result containing the number or the error that occurred.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the number does not match the accepted number grammar
    /// @context         ANY
    /// @pre             All whitespace must have been stripped.
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @synchronous     TRUE
    /// @internal
    /// - If the input is not empty and matches the accepted number grammar:
    ///   - Return the number.
    /// - Otherwise:
    ///   - Return an error.
    /// @endinternal
    static auto New(StringView view) noexcept -> Result<JsonNumber>
    {
        const JsonNumber number{view};
        Result<JsonNumber> result{number};
        if (view.empty())
        {
            result = score::MakeUnexpected(JsonErrc::kInvalidJson, "JsonNumber::New: View must not be empty.");
        }
        else if (!number.Validate())
        {
            result = score::MakeUnexpected(JsonErrc::kInvalidJson,
                                           "JsonNumber::New: Number does not match the accepted number grammar.");
        }
        else
        {
            // The number is valid and already stored in the result.
        }
        return result;
    }

    /// @brief           Tries to convert the number to an integer
    /// @tparam          Integer
    ///                  Type of integer to convert to.
    /// @return          The Optional containing the integer if the conversion was successful, or empty.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @synchronous     TRUE
    /// @internal
    /// - Convert the decimal or hexadecimal integer to the integer type.
    /// - If the conversion was successful:
    ///   - Return a Result containing the number.
    /// - Otherwise:
    ///   - Return an error.
    /// @endinternal
    template <typename Integer, std::enable_if_t<internal::IsInt<Integer>::value>* = nullptr>
    auto As() const noexcept -> Optional<Integer>
    {
        return internal::util::NumberParser::ParseInteger<Integer>(this->view_);
    }

    /// @brief           Tries to convert the number to a floating point type
    /// @tparam          Float
    ///                  Floating point type to convert to.
    /// @return          The Optional containing the floating point number if the conversion was successful, or empty.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @synchronous     TRUE
    /// @internal
    /// - If the number is a hexadecimal integer:
    ///   - Return an error, since hexadecimal numbers are integers only.
    /// - Convert the number to the desired floating point type.
    /// - If the conversion was successful:
    ///   - Return a Result containing the number.
    /// - Otherwise:
    ///   - Return an error.
    /// @endinternal
    template <typename Float, std::enable_if_t<std::is_floating_point<Float>::value>* = nullptr>
    auto As() const noexcept -> Optional<Float>
    {
        Optional<Float> result{};
        if (!internal::util::HasHexadecimalPrefix(this->view_))
        {
            internal::util::NumberParser parser{};
            const double conversion_result{parser.Double(this->view_)};
            result = this->ExtractIfFloatingSuccessful<Float>(conversion_result, parser.End());
        }
        return result;
    }

  private:
    /// @brief           Constructs a Number from a StringView
    /// @param[in]       view
    ///                  to parse.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    explicit constexpr JsonNumber(StringView view) noexcept : view_{view} {}

    /// @brief           Casts the passed number to a smaller number type if still valid
    /// @tparam          TargetType
    ///                  Type to cast to.
    /// @tparam          SourceType
    ///                  Type to cast from.
    /// @param[in]       number
    ///                  to cast.
    /// @return          The Optional containing the number if the cast was successful, or empty.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @internal
    /// - If the number does not exceed the numeric limits of the target type:
    ///   - Cast the number to the target type.
    ///   - Return an Optional containing the number.
    /// - Otherwise:
    ///   - Return an empty Optional.
    /// @endinternal
    template <typename TargetType, typename SourceType>
    static auto Cast(SourceType number) noexcept -> Optional<TargetType>
    {
        Optional<TargetType> opt{};

        // The comparison converts to the common type anyway; be explicit to avoid -Wdouble-promotion.
        using CommonType = std::common_type_t<TargetType, SourceType>;
        const CommonType value{static_cast<CommonType>(number)};

        if ((static_cast<CommonType>(std::numeric_limits<TargetType>::max()) >= value) &&
            (static_cast<CommonType>(std::numeric_limits<TargetType>::lowest()) <= value))
        {
            static_cast<void>(opt.emplace(TargetType(number)));
        }
        return opt;
    }

    /// @brief           Checks if the parsed token format is valid
    /// @param[in]       end
    ///                  The end-pointer that is used to verify the parse.
    /// @return          True if parsing consumed the whole token.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseFormatSuccessful(const char* end) const noexcept -> bool
    {
        return end == this->view_.cend();
    }

    template <typename Float>
    auto ParseFloatingSuccessful(const char* end, const double conversion_result) const noexcept -> bool
    {
        // Some libc implementations set ERANGE for near-limit finite values; accept those when representable.
        return (this->ParseFormatSuccessful(end) && std::isfinite(conversion_result) &&
                JsonNumber::Cast<Float>(conversion_result).has_value());
    }

    template <typename TargetType>
    auto ExtractIfFloatingSuccessful(double num, const char* end) const noexcept -> Optional<TargetType>
    {
        Optional<TargetType> opt{};

        if (this->ParseFloatingSuccessful<TargetType>(end, num))
        {
            opt = JsonNumber::Cast<TargetType>(num);
        }

        return opt;
    }

    /// @brief           Validates the contained number against the accepted number grammar
    /// @details         The grammar is the RFC 8259 number grammar extended by hexadecimal integers:
    ///                  number  = [ "-" ] ( hex-int / ( int [ frac ] [ exp ] ) )
    ///                  hex-int = "0" ( "x" / "X" ) 1*HEXDIG
    ///                  int     = "0" / ( digit1-9 *DIGIT )
    ///                  frac    = "." 1*DIGIT
    ///                  exp     = ( "e" / "E" ) [ "-" / "+" ] 1*DIGIT
    /// @return          True if the whole number matches the grammar, otherwise false.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @internal
    /// - Skip an optional minus sign.
    /// - If a hexadecimal prefix follows, skip it and require a non-empty sequence of hexadecimal digits.
    /// - Otherwise:
    ///   - Skip either a single zero or a non-empty sequence of digits starting with 1-9.
    ///   - If a decimal point follows, skip it and require a non-empty sequence of digits.
    ///   - If an exponent marker follows, skip it and an optional sign, and require a non-empty sequence of digits.
    /// - Return whether all parts are well-formed and the whole number has been consumed.
    /// @endinternal
    auto Validate() const noexcept -> bool
    {
        StringView rest{this->view_};

        const auto skip_one_of = [&rest](const StringView chars) noexcept -> bool {
            const bool matches{(!rest.empty()) && (chars.find(rest.front()) != StringView::npos)};
            if (matches)
            {
                rest.remove_prefix(1U);
            }
            return matches;
        };
        const auto skip_digits = [&rest](const auto is_digit) noexcept -> bool {
            const std::size_t size_before{rest.size()};
            while ((!rest.empty()) && is_digit(rest.front()))
            {
                rest.remove_prefix(1U);
            }
            return rest.size() < size_before;
        };

        static_cast<void>(skip_one_of("-"));

        bool is_valid{false};
        bool is_hexadecimal{false};
        // A leading zero must stand alone or start a hexadecimal prefix; otherwise the integer part starts with 1-9.
        if (skip_one_of("0"))
        {
            is_hexadecimal = skip_one_of("xX");
            is_valid = true;
        }
        else
        {
            is_valid = skip_digits(internal::util::IsDigit);
        }

        if (is_hexadecimal)
        {
            is_valid = skip_digits(internal::util::IsHexDigit);
        }
        else
        {
            if (skip_one_of("."))
            {
                is_valid = skip_digits(internal::util::IsDigit) && is_valid;
            }
            if (skip_one_of("eE"))
            {
                static_cast<void>(skip_one_of("+-"));
                is_valid = skip_digits(internal::util::IsDigit) && is_valid;
            }
        }
        return is_valid && rest.empty();
    }

    /// @brief           Stored view onto the buffer
    StringView view_;
};

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_NUMBER_H
