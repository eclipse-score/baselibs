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

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/types.h"

#include <cerrno>
#include <cstdlib>
#include <string>
#include <type_traits>

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
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_NUMBER_PARSER_H
