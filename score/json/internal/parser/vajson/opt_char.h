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
/// @brief  Character value that can also represent EOF.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_OPT_CHAR_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_OPT_CHAR_H

#include "score/json/internal/parser/vajson/json_error_domain.h"

#include <cstdint>
#include <string>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// @brief           Contains either the character value or an EOF value
class OptChar
{
  public:
    /// @brief           Construct the object with a value
    /// @param[in]       val
    ///                  The value to construct with.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    explicit OptChar(std::int64_t val) noexcept : char_{val} {}

    /// @brief           Returns true in case EOF was encountered
    /// @return          True if EOF was encountered.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto EofFound() const& noexcept -> bool
    {
        return this->char_ == -1;
    }

    /// @brief           Returns true in case a value is contained
    /// @return          True if a value is contained.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto HasValue() const& noexcept -> bool
    {
        return !EofFound();
    }

    /// @brief           Returns the character contained
    /// @return          The contained character.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto Value() const& noexcept -> char
    {
        AssertCondition(this->char_ != -1, "OptChar::Value: OptChar does not hold a value.");
        return std::char_traits<char>::to_char_type(static_cast<std::int32_t>(this->char_));
    }

    /// @brief           Compare the OptChar with a character for equality
    /// @param[in]       candidate
    ///                  The character to compare with the stored one.
    /// @return          True in case a character is contained and the value is equal.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto operator==(const char candidate) const noexcept -> bool
    {

        return this->HasValue() && (this->Value() == candidate);
    }

  private:
    /// @brief           The optional character value
    std::int64_t char_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_OPT_CHAR_H
