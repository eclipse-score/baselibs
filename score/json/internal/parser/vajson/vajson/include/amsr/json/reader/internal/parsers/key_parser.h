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
 *        \brief  A specialized parser for keys.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_KEY_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_KEY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/internal/parsers/virtual_parser.h"
#include "score/functional.hpp"
#include <utility>

namespace amsr
{
namespace json
{
namespace internal
{
/// \brief           A parser that only parses a single key value

class KeyParser final : public VirtualParser
{
    /// \brief           Type of function to be executed when the keys are read
    using Fn = score::cpp::move_only_function<ResultBlank(StringView)>;

  public:
    /// \brief           Constructs a KeyParser
    /// \details         Callback must take the key as a score::safecpp::zstring_view and return ResultBlank.
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \param[in]       fn
    ///                  Function to execute on the key.
    /// \context         ANY
    /// \pre             Callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    KeyParser(JsonData& doc, Fn fn) noexcept : VirtualParser{doc}, fn_{std::move(fn)} {}

    /// \brief           Event for Keys
    /// \param[in]       key
    ///                  that is parsed.
    /// \return          kFinished if the callback function succeeds, or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - Execute the callback with the parsed key.
    /// - If the callback succeeds:
    ///   - Return kFinished.
    /// - Otherwise:
    ///   - Return the error of the callback.
    /// \endinternal
    auto OnKey(StringView key) noexcept -> ParserResult final
    {

        return this->fn_(key).transform([](score::Blank) noexcept {
            return ParserState::kFinished;
        });
    }

    /// \brief           Default event for unexpected elements that aborts the parsing
    /// \error           amsr::json::JsonErrc::kUserValidationFailed
    ///                  if no key is parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Expected to parse a key.");
    }

  private:
    /// \brief           Function to be executed on the key
    Fn fn_;
};

}  // namespace internal
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_KEY_PARSER_H_
