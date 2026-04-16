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
 *        \brief  A specialized parser for number values.
 *
 *********************************************************************************************************************/
#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/functional.hpp"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include <utility>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           A parser that only parses a single number value
/// \tparam          T
///                  Type of number to parse.
template <typename T>

class NumberParser final : public VirtualParser
{
    /// \brief           Type of function to be executed when the numbers are read
    using Fn = score::cpp::move_only_function<ResultBlank(T)>;

  public:
    /// \brief           Constructs a NumberParser
    /// \details         Callback must take the number of type T and return vajson::ResultBlank.
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \param[in]       fn
    ///                  Function to execute on the number.
    /// \context         ANY
    /// \pre             Callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    NumberParser(JsonData& doc, Fn fn) noexcept : VirtualParser{doc}, fn_{std::move(fn)} {}

    /// \brief           Event for Numbers
    /// \param[in]       number
    ///                  that is parsed.
    /// \return          kFinished if the callback function succeeds, or an error.
    /// \error           score::json::vajsonJsonErrc::kInvalidJson
    ///                  if the number could not be converted to the wanted type.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - If the JsonNumber could be successfully converted to a number of type T:
    ///   - Execute the callback with the number.
    ///   - If the callback succeeds:
    ///     - Return kRunning.
    /// - Otherwise:
    ///   - Return the error.
    /// \endinternal
    auto OnNumber(JsonNumber number) noexcept -> ParserResult final
    {

        return number.TryAs<T>().and_then(std::forward<Fn>(this->fn_)).transform([](vajson::Blank) noexcept {
            return ParserState::kFinished;
        });
    }

    /// \brief           Default event for unexpected elements that aborts the parsing
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if no number is parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto OnUnexpectedEvent() noexcept -> ParserResult final
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Expected to parse a number.");
    }

  private:
    /// \brief           Function to be executed on the number value
    Fn fn_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_NUMBER_PARSER_H_
