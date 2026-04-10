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
 *        \brief  A specialized parser for arrays.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/functional.hpp"
#include "score/json/internal/parser/vajson/vajson_impl/reader/v2/single_array_parser.h"
#include <utility>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           A parser that only parses multiple values of the same type
class ArrayParser final : public v2::SingleArrayParser
{
    /// \brief           Type of function to be executed when the keys are read
    using Fn = score::cpp::move_only_function<ResultBlank(std::size_t)>;

  public:
    /// \brief           Constructs an ArrayParser
    /// \details         Callback must take the array index as a std::size_t and return ResultBlank.
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \param[in]       fn
    ///                  Function to execute on every array element.
    /// \context         ANY
    /// \pre             Callback does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    ArrayParser(JsonData& doc, Fn fn) noexcept : v2::SingleArrayParser{doc}, fn_{std::move(fn)} {}

    /// \brief           Event for array Elements
    /// \return          kRunning if the callback function succeeds, or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    /// \internal
    /// - Execute the callback with the current array index.
    /// - If the callback succeeds:
    ///   - Return kRunning.
    /// - Otherwise:
    ///   - Return the error of the callback.
    /// \endinternal
    auto OnElement() noexcept -> ParserResult final
    {

        return std::forward<Fn>(this->fn_)(this->GetIndex()).transform([](score::Blank) noexcept {
            return ParserState::kRunning;
        });
    }

  private:
    /// \brief           Function to be executed on every element
    Fn fn_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_ARRAY_PARSER_H_