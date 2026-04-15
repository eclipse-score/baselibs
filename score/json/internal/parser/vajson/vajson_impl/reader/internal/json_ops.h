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
 *        \brief  Collection of all operations on a JsonData object.
 *
 *      \details  Provides operations for stream based input data.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_JSON_OPS_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_JSON_OPS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/functional.hpp"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/config/json_reader_cfg.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader_fwd.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include <cstdint>
#include <functional>
#include <istream>
#include <string>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           Contains either the character value or an EOF value
class OptChar
{
    /// \brief           The optional character value
    std::int64_t char_;

  public:
    /// \brief           Construct the object with a value
    /// \param[in]       val
    ///                  The value to construct with.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    explicit OptChar(std::int64_t val) noexcept : char_{val} {}

    /// \brief           Returns true in case EOF was encountered
    /// \return          True if EOF was encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto EofFound() const& noexcept -> bool
    {
        return this->char_ == -1;
    }

    /// \brief           Returns true in case a value is contained
    /// \return          True if a value is contained.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto HasValue() const& noexcept -> bool
    {
        return !EofFound();
    }

    /// \brief           Returns the character contained
    /// \return          The contained character.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto Value() const& noexcept -> char
    {
        AssertCondition(this->char_ != -1, "OptChar::Value: OptChar does not hold a value.");
        return std::char_traits<char>::to_char_type(static_cast<std::int32_t>(this->char_));
    }

    /// \brief           Compare the OptChar with a character for equality
    /// \param[in]       candidate
    ///                  The character to compare with the stored one.
    /// \return          True in case a character is contained and the value is equal.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto operator==(const char candidate) const noexcept -> bool
    {

        return this->HasValue() && (this->Value() == candidate);
    }
};

/// \brief           A handler for operations on a JsonData object
class JsonOps final
{
  public:
    /// \brief           Initializes JsonOps
    /// \param[in]       json_data
    ///                  to operate on.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    explicit JsonOps(JsonData& json_data) noexcept;

    /// \brief           Default move constructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    JsonOps(JsonOps&&) noexcept = default;

    /// \brief           Default move assignment
    /// \return          A reference to the moved into object.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto operator=(JsonOps&&) & noexcept -> JsonOps& = default;

    /// \brief           Deleted copy constructor
    JsonOps(const JsonOps&) = delete;
    /// \brief           Deleted copy assignment
    auto operator=(const JsonOps&) -> JsonOps& = delete;

    /// \brief           Default Destructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    ~JsonOps() noexcept = default;

    /// \brief           Returns the character at the current position and moves the cursor to the next character
    /// \return          The character at the current position.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto Take() noexcept -> char;

    /// \brief           Tries to take the character at the current position and moves the cursor to the next character
    /// \return          The character if inside stream bounds, or the error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson,
    ///                  if the stream has ended
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto TryTake() noexcept -> Result<char>;

    /// \brief           Moves the cursor from the current position to the next position
    /// \return          True if the move succeeded.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto Move() noexcept -> bool;

    /// \brief           Gets the current position
    /// \return          The current position.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto Tell() const noexcept -> Result<std::uint64_t>;

    /// \brief           Moves the cursor to the next position if the passed character is equal to the current character
    /// \param[in]       character
    ///                  to compare to.
    /// \return          True if the characters are equal and the cursor has been moved to the next position, otherwise
    ///                  false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto Skip(char character) noexcept -> bool;

    /// \brief           Checks if the next characters are equal to the passed string
    /// \param[in]       string
    ///                  that is expected.
    /// \param[in]       error_msg
    ///                  in case the check fails. Must live until the error object is evaluated.
    /// \return          The empty Result if the string was found, or the error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the passed string is empty or it could not be found.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto CheckString(std::string_view string, std::string_view error_msg) noexcept -> Result<vajson::Blank>;

    /// \brief           Returns if the next characters are equal to the passed string
    /// \param[in]       string
    ///                  that is expected.
    /// \return          True if the string was found else false.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson,
    ///                  if any error occurred.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ReadString(std::string_view string) noexcept -> Result<bool>;

    /// \brief           Skips all valid whitespace characters
    /// \details         Valid JSON whitespace characters are space, newline, carriage return, and tab.
    /// \return          True in case we can continue, false in case end of file occurred.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto SkipWhitespace() noexcept -> bool;

    /// \brief           Reads num_to_read characters and executes the action for each span of characters
    /// \param[in]       num_to_read
    ///                  The number of characters to read.
    /// \param[in]       callback
    ///                  Callback to call per view.
    /// \return          The number of characters read.
    /// \context         ANY
    /// \pre             callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto Read(std::uint64_t num_to_read,
              const score::cpp::move_only_function<void(std::string_view), 40>& callback) noexcept
        -> Result<std::uint64_t>;

    /// \brief           Reads exactly num_to_read characters and executes the action
    /// \details         Buffers the data if necessary. The callback is either executed once (requested amount of
    ///                  characters has been read) or never (less characters available or EOF).
    /// \param[in]       num_to_read
    ///                  The number of characters to read.
    /// \param[in]       callback
    ///                  Callback to call once all characters have been read.
    /// \return          An empty Result or an error if less characters are available.
    /// \context         ANY
    /// \pre             callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ReadExactly(std::uint64_t num_to_read,
                     const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept
        -> Result<vajson::Blank>;

    /// \brief            Reads characters as long as the delimiter is not found and executes the action for each span
    /// of characters \param[in]       delimiter                  to decide if the character should be skipped.
    /// \param[in]       callback                  Callback to call per view.
    /// \return          Either EOF or the delimiter that was found.
    /// \context         ANY
    /// \pre             callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ReadUntil(std::string_view delimiter,
                   const score::cpp::move_only_function<void(std::string_view)>& callback) noexcept -> Result<OptChar>;

    /// \brief           Returns a reference to the Json file
    /// \return          Reference to the Json file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() & noexcept -> JsonData&;

    /// \brief           Returns a reference to the Json file
    /// \return          Const reference to the Json file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() const& noexcept -> const JsonData&;

  private:
    /// \brief           Rewind the position of the document if a condition is fulfilled
    /// \param[in]       condition
    ///                  The condition to check before seeking.
    /// \param[in]       num
    ///                  The number of bytes to rewind.
    /// \return          nothing or the error occurred.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto RewindIf(bool condition, std::size_t num) noexcept -> Result<vajson::Blank>;

    /// \brief           Get direct access to the input stream
    /// \return          Reference to the input stream.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetStream() & noexcept -> std::istream&;

    /// \brief           Get direct access to the input stream
    /// \return          Const reference to the input stream.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetStream() const& noexcept -> const std::istream&;

    /// \brief           JsonData to operate on
    std::reference_wrapper<JsonData> data_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_JSON_OPS_H_
