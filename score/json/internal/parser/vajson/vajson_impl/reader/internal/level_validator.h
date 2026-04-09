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
 *        \brief  Allows semi-automatic checks for single object and single array parsers.
 *
 *      \details  Keeps track of the nesting depth of structures and allows only one level of depth.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_LEVEL_VALIDATOR_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_LEVEL_VALIDATOR_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include <utility>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           A validator for structure elements
class LevelValidator
{
  public:
    /// \brief           Constructs a new Level Validator object
    /// \param[in]       object_already_open
    ///                  If true the validator assumes that the object is already open.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    explicit LevelValidator(bool object_already_open = false) noexcept : entered_{object_already_open} {}

    /// \brief           Tries to enter a structure
    /// \return          kRunning if no structure has been entered yet, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if already inside a structure
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If not inside a structure:
    ///   - Set the signal that a structure has been entered.
    ///   - Return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto Enter() noexcept -> ParserResult
    {
        ParserResult result{ParserState::kRunning};

        if (!entered_)
        {
            entered_ = true;
        }
        else
        {

            result = MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Did not expect nested elements");
        }

        return result;
    }

    /// \brief           Tries to leave a structure
    /// \return          kFinished if inside a structure, or the error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if not inside a structure.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - If inside a structure:
    ///   - Set the signal that a structure has been left.
    ///   - Return kRunning.
    /// - Otherwise:
    ///   - Return an error.
    /// \endinternal
    auto Leave() noexcept -> ParserResult
    {
        ParserResult result{ParserState::kFinished};

        if (entered_)
        {
            entered_ = false;
        }
        else
        {

            result = MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Cannot leave level");
        }

        return result;
    }

    /// \brief           Returns true if currently inside a structure
    /// \return          True if inside a structure.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto IsInside() const noexcept -> bool
    {
        return this->entered_;
    }

  private:
    /// \brief           Signals that a level has been entered
    bool entered_{false};
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_LEVEL_VALIDATOR_H_
