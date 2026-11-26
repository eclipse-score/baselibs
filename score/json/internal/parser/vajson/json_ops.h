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
/// @brief  Collection of all operations on a JsonData object.
///
/// @details  Provides operations for in-memory input data.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_OPS_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_OPS_H

#include "score/functional.hpp"
#include "score/json/internal/parser/vajson/opt_char.h"
#include "score/json/internal/parser/vajson/reader_config.h"
#include "score/json/internal/parser/vajson/reader_fwd.h"
#include "score/result/result.h"
#include <functional>
#include <string_view>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// @brief           A handler for operations on a JsonData object
class JsonOps final
{
  public:
    /// @brief           Initializes JsonOps
    /// @param[in]       json_data
    ///                  to operate on.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    explicit JsonOps(JsonData& json_data) noexcept;

    /// @brief           Default move constructor
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    JsonOps(JsonOps&&) noexcept = default;

    /// @brief           Default move assignment
    /// @return          A reference to the moved into object.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto operator=(JsonOps&&) & noexcept -> JsonOps& = default;

    /// @brief           Deleted copy constructor
    JsonOps(const JsonOps&) = delete;
    /// @brief           Deleted copy assignment
    auto operator=(const JsonOps&) -> JsonOps& = delete;

    /// @brief           Default Destructor
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    ~JsonOps() noexcept = default;

    /// @brief           Returns the character at the current position and moves the cursor to the next character
    /// @return          The character at the current position.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto Take() noexcept -> char;

    /// @brief           Tries to take the character at the current position and moves the cursor to the next character
    /// @return          The character if inside the document, or the error.
    /// @error           score::json::Error::kParsingError
    ///                  if the document has ended
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto TryTake() noexcept -> Result<char>;

    /// @brief           Moves the cursor from the current position to the next position
    /// @return          True if the move succeeded.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto Move() noexcept -> bool;

    /// @brief           Moves the cursor to the next position if the passed character is equal to the current character
    /// @param[in]       character
    ///                  to compare to.
    /// @return          True if the characters are equal and the cursor has been moved to the next position, otherwise
    ///                  false.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto Skip(char character) noexcept -> bool;

    /// @brief           Checks if the next characters are equal to the passed string
    /// @param[in]       string
    ///                  that is expected.
    /// @param[in]       error_msg
    ///                  in case the check fails. Must live until the error object is evaluated.
    /// @return          The empty Result if the string was found, or the error.
    /// @error           score::json::Error::kParsingError
    ///                  if the passed string is empty or it could not be found.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto CheckString(std::string_view string, std::string_view error_msg) noexcept -> Result<void>;

    /// @brief           Moves the cursor past the passed string if the next characters are equal to it
    /// @param[in]       string
    ///                  that is expected.
    /// @return          True if the string was found and the cursor has been moved past it, otherwise false.
    /// @context         ANY
    /// @pre             string is not empty.
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ReadString(std::string_view string) noexcept -> bool;

    /// @brief           Skips all valid whitespace characters
    /// @details         Valid JSON whitespace characters are space, newline, carriage return, and tab.
    /// @return          True in case we can continue, false in case end of file occurred.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto SkipWhitespace() noexcept -> bool;

    /// @brief            Reads characters as long as the delimiter is not found and executes the action for each span
    /// of characters @param[in]       delimiter                  to decide if the character should be skipped.
    /// @param[in]       callback                  Callback to call per view.
    /// @return          Either EOF or the delimiter that was found.
    /// @context         ANY
    /// @pre             callback does not throw exceptions.
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ReadUntil(std::string_view delimiter,
                   const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept -> Result<OptChar>;

    /// @brief           Returns a reference to the Json file
    /// @return          Reference to the Json file.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() & noexcept -> JsonData&;

  private:
    /// @brief           Get direct access to the unread input
    /// @return          Reference to the unread input.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetInput() & noexcept -> std::string_view&;

    /// @brief           JsonData to operate on
    std::reference_wrapper<JsonData> data_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_JSON_OPS_H
