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
 *        \brief  A parser that aborts on unknown values.
 *
 *      \details  Provides pre-defined parsers for JSON elements. If the parser encounters a JSON element for which no
 *                user callback is implemented by the child parser, it returns a negative Result.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_PARSER_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/composition_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace v2
{
/// \brief           A parser that aborts on unknown values
/// \details         The VirtualParser implements a interface for parsers and is based on dynamic polymorphism. The
///                  derived parser can implement callbacks for all types of elements it expects to appear. If the
///                  parser encounters any other type, it calls OnUnexpectedEvent. If this callback is not defined by
///                  the derived parser, the default implementation aborts parsing.

class Parser : public internal::CompositionParser<internal::VirtualParser>
{
  public:
    /// \brief           Constructor
    using internal::CompositionParser<internal::VirtualParser>::CompositionParser;

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;
};

}  // namespace v2
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_V2_PARSER_H_
