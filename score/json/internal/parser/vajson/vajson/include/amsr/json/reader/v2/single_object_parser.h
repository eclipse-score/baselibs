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
 *        \brief  A specialized parser to parse a single object.
 *
 *      \details  Provides a callback for every element it encounters and keeps track of the nesting depth.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_V2_SINGLE_OBJECT_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_V2_SINGLE_OBJECT_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/internal/level_validator.h"
#include "amsr/json/reader/v2/parser.h"
#include <cstdint>

namespace score
{
namespace json
{
namespace vajson
{
namespace v2
{
/// \brief           A parser for a single object, i.e. an object without sub-objects
/// \details         Handles the OnStartObject & OnEndObject callbacks by itself. Calls OnKey for every key it
/// encounters
///                  and Finalize on object end.

class SingleObjectParser : public v2::Parser
{
  public:
    /// \brief           Constructs a SingleObjectParser
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \param[in]       object_already_open
    ///                  Specify if the object has already been opened. Defaults to false.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    explicit SingleObjectParser(JsonData& doc, bool object_already_open = false) noexcept
        : v2::Parser{doc}, validator_{object_already_open}
    {
    }

    /// \brief Delete copy constructor
    SingleObjectParser(const SingleObjectParser&) = delete;
    /// \brief Delete copy assignment constructor
    SingleObjectParser& operator=(const SingleObjectParser&) = delete;
    /// \brief Delete move constructor
    SingleObjectParser(SingleObjectParser&&) = delete;
    /// \brief Delete move assignment constructor
    SingleObjectParser& operator=(SingleObjectParser&&) = delete;

    /// \brief           Default event for the start of objects
    /// \return          kRunning if not in an object, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if already in an object
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If not inside an object:
    ///   - Set the flag that an object has been entered.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto OnStartObject() noexcept -> ParserResult final
    {
        return this->validator_.Enter();
    }

    /// \brief           Default event for the end of objects
    /// \return          kRunning if in an object, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if not in an object
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - Check if inside an object:
    ///   - Call the Finalize callback and return its Result.
    /// \endinternal
    auto OnEndObject(std::size_t) noexcept -> ParserResult final
    {
        return this->validator_.Leave().and_then([this](ParserState state) noexcept {
            return this->Finalize().transform([&state](score::Blank) noexcept {
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

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed,
                                            "Expected to parse an object of elements.");
    }

  protected:
    /// \brief           Default event if the entire object has been successfully parsed
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
        return Result<score::Blank>{score::Blank{}};
    }

  private:
    /// \brief           Validates if the only one level of object has been entered
    internal::LevelValidator validator_;
};

}  // namespace v2
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_V2_SINGLE_OBJECT_PARSER_H_
