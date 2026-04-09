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
 *        \brief  A specialized parser to parse a single array of elements.
 *
 *      \details  Provides a callback for every element it encounters and keeps track of the nesting depth.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_SINGLE_ARRAY_PARSER_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_SINGLE_ARRAY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/level_validator.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/parser.h"
#include <cstdint>

namespace score
{
namespace json
{
namespace vajson
{
namespace v2
{
/// \brief           A parser for a single array, i.e. an array that does not contain arrays as its elements
/// \details         Handles the OnStartArray & OnEndArray callbacks by itself. Calls OnElement for every element it
///                  encounters and Finalize on array end.

class SingleArrayParser : public v2::Parser
{
  private:
    /// \brief           Validates if the only one level of array has been entered
    internal::LevelValidator validator_{};
    /// \brief           Index for the array elements
    std::size_t index_{0};

  public:
    /// \brief           Inherited CTOR
    using v2::Parser::Parser;

    /// \brief Delete copy constructor
    SingleArrayParser(const SingleArrayParser&) = delete;
    /// \brief Delete copy assignment constructor
    SingleArrayParser& operator=(const SingleArrayParser&) = delete;
    /// \brief Delete move constructor
    SingleArrayParser(SingleArrayParser&&) = delete;
    /// \brief Delete move assignment constructor
    SingleArrayParser& operator=(SingleArrayParser&&) = delete;

    /// \brief           Default event for the start of arrays

    /// \return          kRunning if not in an array, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if already in an array
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If not inside an array:
    ///   - Take a snapshot of the current state in case it has to be restored because this opening bracket is followed
    ///   by
    ///     a value.
    ///   - Return kRunning to continue parsing.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto OnStartArray() noexcept -> ParserResult final
    {
        return this->validator_
            .Enter()

            .and_then([this](ParserState) noexcept {
                return this->GetJsonDocument().Snap();
            })
            .transform([](score::Blank) noexcept {
                return ParserState::kRunning;
            });
    }

    /// \brief           Default event for the end of arrays

    /// \return          kRunning if in an array, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if not in an array
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If inside an array:
    ///   - Call the Finalize callback and return its Result.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto OnEndArray(std::size_t) noexcept -> ParserResult final
    {
        return this->validator_.Leave().and_then([this](ParserState state) noexcept {
            return Finalize().transform([&state](score::Blank) noexcept {
                return state;
            });
        });
    }

    /// \brief           Default event for unexpected elements that aborts the parsing

    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if there is no callback registered for the event
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If not inside an array, return an error.
    /// - Restore the last snapshot.
    /// - If restoring the snapshot failed:
    ///   - Return the error.
    /// - Otherwise:
    ///   - Call OnElement and return its Result.
    ///   - On success, take a new snapshot, because the value could be followed by another value, and increase the
    ///   index.
    /// \endinternal
    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return MakeResult(this->validator_.IsInside(),
                          {JsonErrc::kUserValidationFailed, "Expected to parse an array of elements."})

            .and_then([this](score::Blank) noexcept {
                return this->GetJsonDocument().Restore();
            })
            .and_then([this](score::Blank) noexcept {
                return this->OnElement();
            })

            .and_then([this](ParserState state) noexcept {
                return this->GetJsonDocument().Snap().transform([this, &state](score::Blank) {
                    index_++;
                    return state;
                });
            });
    }

    /// \brief           Returns the array index of the current element
    /// \return          Array index of the current element.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto GetIndex() const noexcept -> std::size_t
    {
        return this->index_;
    }

  protected:
    /// \brief           Event for array Elements to be implemented by the user
    /// \details         This function is expected to consume all tokens representing the element, unless it returns an
    ///                  error Result or ParserState::kFinished.
    /// \return          -
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    virtual auto OnElement() noexcept -> ParserResult = 0;

    /// \brief           Default event if the entire array has been successfully parsed
    /// \details         Default implementation does nothing. User implementation could be used to validate parser
    /// results
    ///                  or fill in inout parameters etc.
    /// \return          The empty Result.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto Finalize() noexcept -> Result<score::Blank>
    {
        return ResultBlank{score::Blank{}};
    }
};

}  // namespace v2
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_SINGLE_ARRAY_PARSER_H_
