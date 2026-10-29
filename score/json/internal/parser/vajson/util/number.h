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
/*!        \file
 *        \brief  A collection of utility functions for JSON numbers.
 *
 *      \details  Provides string to number conversion functions.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_UTIL_NUMBER_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_UTIL_NUMBER_H

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cassert>

#include <charconv>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include "score/result/result.h"

#include "score/json/internal/parser/vajson/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief            Checks if type is an integer
/// \details         Does not apply for bool.
/// \tparam          T                  Type to check.
template <typename T>
using IsInt = std::conjunction<std::negation<std::is_same<T, bool>>, std::is_integral<T>>;

namespace util
{
/// \brief Helper function to check if character is a digit, in ASCII/UTF-8.
inline auto IsDigit(const char chr) noexcept -> bool
{
    constexpr auto zero = std::char_traits<char>::to_int_type('0');
    constexpr auto nine = std::char_traits<char>::to_int_type('9');
    const auto intchr = std::char_traits<char>::to_int_type(chr);
    return (intchr >= zero) && (intchr <= nine);
}

/// \brief           A parser for JSON numbers
/// \pre             This class assumes that all leading whitespace has been stripped.
class NumberParser
{
  public:
    /// \brief           Constructs a NumberParser
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    NumberParser() noexcept = default;

    /// \brief           Gets a pointer beyond the last parsed character
    /// \return          Pointer beyond the last parsed character.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto End() const& noexcept -> const char*
    {
        return end_;
    }

    // Deleted rvalue-qualified version to prevent dangling reference
    auto End() const&& noexcept -> const char* = delete;

    /// @brief           Parses a StringView as a decimal integer
    /// @tparam          Integer
    ///                  Type of integer to parse into.
    /// @param[in]       view
    ///                  to parse.
    /// @return          The Optional containing the number if the whole view is a decimal integer that fits into
    ///                  Integer, or empty.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE
    /// @reentrant       TRUE
    template <typename Integer>
    static auto ParseInteger(const StringView view) noexcept -> Optional<Integer>
    {
        Integer value{};
        const char* const first{view.data()};
        const char* const last{std::next(first, static_cast<std::ptrdiff_t>(view.size()))};
        const std::from_chars_result conversion{std::from_chars(first, last, value, kDecimalBase)};

        Optional<Integer> opt{};
        if ((conversion.ec == std::errc{}) && (conversion.ptr == last))
        {
            static_cast<void>(opt.emplace(value));
        }
        return opt;
    }

    /// \brief           Parses a StringView as a double
    /// \param[in]       view
    ///                  to parse.
    /// \return          The parsed number.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - Convert the view to a double.
    /// \endinternal
    auto Double(StringView view) noexcept -> double
    {
        return std::strtod(view.data(), &this->end_);
    }

  private:
    /// @brief           JSON integers are always decimal.
    static constexpr std::int32_t kDecimalBase{10};

    /// \brief            Pointer beyond the last parsed character
    /// \details         Set by character conversion functions. Null if the parse did not work.
    char* end_{nullptr};
};

}  // namespace util
}  // namespace internal

/// \brief           A representation of a parsed JSON number
/// \trace           DSGN-JSON-Reader-Number-Formatting
class JsonNumber final
{
  public:
    /// \brief           Creates a new number
    /// \param[in]       view
    ///                  to parse.
    /// \return          A Result containing the number or the error that occurred.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the number does not match the RFC 8259 number grammar
    /// \context         ANY
    /// \pre             All whitespace must have been stripped.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - If the input is not empty and matches the RFC 8259 number grammar:
    ///   - Return the number.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    static auto New(StringView view) noexcept -> Result<JsonNumber>
    {
        return Filter(
            Filter(
                Result<JsonNumber>{JsonNumber{view}},
                [](const JsonNumber& num) noexcept {
                    return !num.view_.empty();
                },
                MakeError(static_cast<ErrorCode>(JsonErrc::kInvalidJson), "JsonNumber::New: View must not be empty.")),
            [](const JsonNumber& num) noexcept {
                return num.Validate();
            },
            MakeError(static_cast<ErrorCode>(JsonErrc::kInvalidJson),
                      "JsonNumber::New: Number does not match the RFC 8259 number grammar."));
    };

    /// \brief           Tries to convert the number to a type
    /// \tparam          T
    ///                  Type to convert to.
    /// \return          The Result containing the converted number if the conversion was successful, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  Could not convert number
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - Convert the number to type T.
    /// - If the conversion was successful:
    ///   - Return a Result containing the number.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename T>
    auto TryAs() const noexcept -> Result<T>
    {
        return MakeResult(this->As<T>(), {JsonErrc::kInvalidJson, "Could not convert number."});
    }

    /// \brief           Tries to convert the number to a bool
    /// \details         Maps "1" to true and "0" to false.
    /// \return          The Optional containing the bool if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - If the number is a single '1' or '0' character:
    ///   - Convert to true or false respectively.
    ///   - Return a Result containing the bool.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Bool, std::enable_if_t<std::is_same<Bool, bool>::value>* = nullptr>
    auto As() const noexcept -> Optional<bool>
    {
        Optional<bool> opt{};
        if (this->GetNumberOfChars() == 1)
        {

            switch (std::char_traits<char>::to_int_type(this->view_[0]))
            {
                case std::char_traits<char>::to_int_type('1'):
                    static_cast<void>(opt.emplace(true));
                    break;
                case std::char_traits<char>::to_int_type('0'):
                    static_cast<void>(opt.emplace(false));
                    break;
                default:
                    // in case something different from 0 or 1 is included, there will be no conversion.
                    break;
            }
        }
        return opt;
    }

    /// \brief            Tries to convert the number to an integer
    /// \tparam          Integer Type of integer to convert to.
    /// \return          The Optional containing the integer if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /*!
     * \internal
     * - Convert the number to an integer.
     * - If the conversion was successful:
     *   - Return a Result containing the number.
     * - Otherwise:
     *   - Return an error.
     * \endinternal
     */
    template <typename Integer, std::enable_if_t<internal::IsInt<Integer>::value>* = nullptr>
    auto As() const noexcept -> Optional<Integer>
    {
        return internal::util::NumberParser::ParseInteger<Integer>(this->view_);
    }

    /// \brief           Tries to convert the number to a floating point type
    /// \tparam          Float
    ///                  Floating point type to convert to.
    /// \return          The Optional containing the floating point number if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - Convert the number to the desired floating point type.
    /// - If the conversion was successful:
    ///   - Return a Result containing the number.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Float, std::enable_if_t<std::is_floating_point<Float>::value>* = nullptr>
    auto As() const noexcept -> Optional<Float>
    {
        internal::util::NumberParser parser{};
        const double conversion_result{parser.Double(this->view_)};

        return this->ExtractIfFloatingSuccessful<Float>(conversion_result, parser.End());
    }

    /// \brief           Converts the number to a JsonNumber
    /// \return          The Optional containing the JsonNumber.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - Return the number as a Number type.
    /// \endinternal
    template <typename Num, std::enable_if_t<std::is_same<JsonNumber, Num>::value>* = nullptr>
    auto As() const noexcept -> Optional<Num>
    {
        return Optional<Num>{JsonNumber(this->view_)};
    }

    /// \brief           Converts the number using a user defined parser
    /// \details         Parser function must take the string representation of the number as std::string_view and
    ///                  return the desired number type.
    /// \tparam          Fn
    ///                  Type of function parser.
    /// \param[in]       parser
    ///                  function to convert the string view into a number.
    /// \return          The parsed number.
    /// \context         ANY
    /// \pre             Parser does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - Convert the number using the given function.
    /// \endinternal
    template <typename Fn>
    auto Convert(Fn&& parser) const noexcept -> decltype(parser(std::declval<StringView>()))
    {

        return std::forward<Fn>(parser)(this->view_);
    }

  private:
    /// \brief           Constructs a Number from a StringView
    /// \param[in]       view
    ///                  to parse.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    explicit constexpr JsonNumber(StringView view) noexcept : view_{view} {}

    /// \brief           Gets the number of characters
    /// \return          The number of characters.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetNumberOfChars() const noexcept -> std::size_t
    {
        return this->view_.size();
    }

    /// \brief           Casts the passed number to a smaller number type if still valid
    /// \tparam          TargetType
    ///                  Type to cast to.
    /// \tparam          SourceType
    ///                  Type to cast from.
    /// \param[in]       number
    ///                  to cast.
    /// \return          The Optional containing the number if the cast was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - If the number does not exceed the numeric limits of the target type:
    ///   - Cast the number to the target type.
    ///   - Return an Optional containing the number.
    /// - Otherwise:
    ///   - Return an empty Optional.
    /// \endinternal
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

    /// \brief           Checks if the parsed token format is valid
    /// \param[in]       end
    ///                  The end-pointer that is used to verify the parse.
    /// \return          True if parsing consumed the whole token.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ParseFormatSuccessful(const char* end) const noexcept -> bool
    {
        return (end == this->view_.cend());
    }

    template <typename Float>
    auto ParseFloatingSuccessful(const char* end, const double conversion_result) const noexcept -> bool
    {
        // Some libc implementations set ERANGE for near-limit finite values; accept those when representable.
        return (this->ParseFormatSuccessful(end) &&  // LCOV_EXCL_BR_LINE (Not reachable in the "C" locale; kept
                                                     // because strtod reads a locale-specific decimal separator.)
                std::isfinite(conversion_result) && JsonNumber::Cast<Float>(conversion_result).has_value());
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

    /// @brief           Validates the contained number against the RFC 8259 number grammar
    /// @details         number = [ "-" ] int [ frac ] [ exp ]
    ///                  int    = "0" / ( digit1-9 *DIGIT )
    ///                  frac   = "." 1*DIGIT
    ///                  exp    = ( "e" / "E" ) [ "-" / "+" ] 1*DIGIT
    /// @return          True if the whole number matches the grammar, otherwise false.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    /// @internal
    /// - Skip an optional minus sign.
    /// - Skip either a single zero or a non-empty sequence of digits starting with 1-9.
    /// - If a decimal point follows, skip it and require a non-empty sequence of digits.
    /// - If an exponent marker follows, skip it and an optional sign, and require a non-empty sequence of digits.
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
        const auto skip_digits = [&rest]() noexcept -> bool {
            const std::size_t size_before{rest.size()};
            while ((!rest.empty()) && internal::util::IsDigit(rest.front()))
            {
                rest.remove_prefix(1U);
            }
            return rest.size() < size_before;
        };

        bool is_valid{true};

        static_cast<void>(skip_one_of("-"));

        // A leading zero must stand alone; otherwise the integer part starts with 1-9.
        if (!skip_one_of("0"))
        {
            is_valid = skip_digits();
        }

        if (skip_one_of("."))
        {
            const bool has_fraction_digits{skip_digits()};
            is_valid = is_valid && has_fraction_digits;
        }

        if (skip_one_of("eE"))
        {
            static_cast<void>(skip_one_of("+-"));
            const bool has_exponent_digits{skip_digits()};
            is_valid = is_valid && has_exponent_digits;
        }

        return is_valid && rest.empty();
    }

    /// \brief           Stored view onto the buffer
    StringView view_;
};

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_UTIL_NUMBER_H
