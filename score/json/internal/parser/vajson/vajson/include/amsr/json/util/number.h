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

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_NUMBER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_NUMBER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <algorithm>
#include <cassert>

#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

#include "score/result/result.h"

#include "amsr/json/util/json_error_domain.h"
#include "amsr/json/util/types.h"

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

/// \brief            Typedef for longlong
/// \details         Return type of std::strtoll and transformed to fixed-size type later.
using SignedLL = long long int;  // VECTOR SL AutosarC++17_10-A3.9.1: MD_JSON_base_type

/// \brief            Typedef for unsigned longlong
/// \details         Return type of std::strtoull and transformed to fixed-size type later.
using UnsignedLL = unsigned long long int;  // VECTOR SL AutosarC++17_10-A3.9.1: MD_JSON_base_type

namespace util
{
/// \brief           Resets the value of errno to zero
inline void ResetErrno() noexcept
{

    errno = 0;
}

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

    /// \brief           Parses a StringView as a long long
    /// \param[in]       view
    ///                  to parse.
    /// \return          The parsed number.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - Reset errno.
    /// - Convert the view to a long long.
    /// \endinternal
    auto LongLong(StringView view) noexcept -> SignedLL
    {
        ResetErrno();

        return std::strtoll(view.data(), &this->end_, 0);
    }

    /// \brief           Parses a StringView as an unsigned long long
    /// \param[in]       view
    ///                  to parse. Must be pointing to a mutable buffer.
    /// \return          The parsed number.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the view starts with a dash:
    ///   - Skip the parsing.
    /// - Otherwise:
    ///   - Reset errno.
    ///   - Convert the view to an unsigned long long.
    /// \endinternal
    auto UnsignedLongLong(StringView view) noexcept -> UnsignedLL
    {
        UnsignedLL result{0};
        // strtoull accepts negative values!!

        if (view[0] == '-')
        {
            // Set end_ so that it will be reported as 'parsed unsuccessfully'

            this->end_ = const_cast<char*>(view.data());
        }
        else
        {
            ResetErrno();

            result = std::strtoull(view.data(), &this->end_, 0);
        }
        return result;
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
    /// - Reset errno.
    /// - Convert the view to a double.
    /// \endinternal
    auto Double(StringView view) noexcept -> double
    {
        ResetErrno();

        return std::strtod(view.data(), &this->end_);
    }

  private:
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
    ///                  if the number contains invalid characters
    /// \context         ANY
    /// \pre             All whitespace must have been stripped.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE
    /// \internal
    /// - If the input is not empty and a valid JSON number:
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
                      "JsonNumber::New: Number contains invalid characters."));
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

    /// \brief            Tries to convert the number to a signed integer
    /// \tparam          Integer Type of signed integer to convert to.
    /// \return          The Optional containing the signed integer if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /*!
     * \internal
     * - Convert the number to a signed number.
     * - If the conversion was successful:
     *   - Return a Result containing the number.
     * - Otherwise:
     *   - Return an error.
     * \endinternal
     */
    template <typename Integer,
              std::enable_if_t<internal::IsInt<Integer>::value && std::is_signed<Integer>::value>* = nullptr>
    auto As() const noexcept -> Optional<Integer>
    {
        internal::util::NumberParser parser{};
        internal::SignedLL const result{parser.LongLong(this->view_)};

        return this->ExtractIfSuccessful<Integer>(result, parser.End());
    }

    /// \brief           Tries to convert the number to an unsigned integer
    /// \tparam          Integer
    ///                  Type of unsigned integer to convert to.
    /// \return          The Optional containing the unsigned integer if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \synchronous     TRUE

    /// \internal
    /// - Convert the number to an unsigned number.
    /// - If the conversion was successful:
    ///   - Return a Result containing the number.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    template <typename Integer,
              std::enable_if_t<internal::IsInt<Integer>::value && (!std::is_signed<Integer>::value)>* = nullptr>

    auto As() const noexcept -> Optional<Integer>
    {
        internal::util::NumberParser parser{};
        internal::UnsignedLL const result{parser.UnsignedLongLong(this->view_)};

        return this->ExtractIfSuccessful<Integer>(result, parser.End());
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

        return this->ExtractIfSuccessful<Float>(conversion_result, parser.End());
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

        if ((std::numeric_limits<TargetType>::max() >= number) && (std::numeric_limits<TargetType>::lowest() <= number))
        {
            static_cast<void>(opt.emplace(TargetType(number)));
        }
        return opt;
    }

    /// \brief           Checks if the parse has been successful
    /// \param[in]       end
    ///                  The end-pointer that is used to verify the parse.
    /// \return          True if the number was successfully parsed, otherwise false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If no ERANGE error occurred and the end-pointer has been set to past-the-last character and the last character
    /// is
    ///   no '.' character:
    ///   - Return true.
    /// - Otherwise:
    ///   - Return false.
    /// \endinternal
    auto ParseSuccessful(const char* end) const noexcept -> bool
    {
        // std::strtod uncomplainingly parses input that ends with a period, although that is invalid JSON!

        return ((errno != ERANGE) && (end == this->view_.cend()) &&

                (this->view_.back() != '.'));
    }

    /// \brief           Converts the passed number to a smaller number type if still valid
    /// \tparam          TargetType
    ///                  Type to convert to.
    /// \tparam          SourceType
    ///                  Type to convert from.
    /// \param[in]       num
    ///                  The source value.
    /// \param[in]       end
    ///                  The end iterator of the buffer.
    /// \return          The Optional containing the number if the conversion was successful, or empty.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the number has been successfully parsed and casted to the target type:
    ///   - Return an Optional containing the number.
    /// - Otherwise:
    ///   - Return an empty Optional.
    /// \endinternal
    template <typename TargetType, typename SourceType>
    auto ExtractIfSuccessful(SourceType num, const char* end) const noexcept -> Optional<TargetType>
    {
        Optional<TargetType> opt{};

        if (this->ParseSuccessful(end))
        {
            opt = JsonNumber::Cast<TargetType>(num);
        }

        return opt;
    }

    /// \brief           Validates the contained number
    /// \return          True if the number is valid, otherwise false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If the number has multiple digits:
    ///   - Check that if it has a leading zero, it is a valid hex or float number.
    /// - Otherwise:
    ///   - Check that it is a valid digit.
    /// \endinternal
    auto Validate() const noexcept -> bool
    {
        std::size_t const size{this->view_.size()};

        std::size_t const first_digit_pos{(this->view_[0] == '-') ? std::size_t{1} : std::size_t{0}};

        const bool is_multiple_digits{size > (first_digit_pos + 1)};
        bool is_valid{false};
        if (is_multiple_digits)
        {
            constexpr static StringView kValidLetters{".eExX"};

            // If it has a leading '0' character, it must be followed by a valid letter character in order to be a valid
            // hex or float number.

            is_valid =
                (this->view_[first_digit_pos] != '0') ||
                std::any_of(kValidLetters.cbegin(), kValidLetters.cend(), [this, first_digit_pos](char ch) noexcept {
                    return this->view_[first_digit_pos + 1] == ch;
                });
        }
        else
        {
            is_valid = (size == (first_digit_pos + 1)) && internal::util::IsDigit(this->view_[first_digit_pos]);
        }
        return is_valid;
    }

    /// \brief           Stored view onto the buffer
    StringView view_;
};

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_UTIL_NUMBER_H_
