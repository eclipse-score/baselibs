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
 *        \brief  json parser
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/json_parser.h"
#include "amsr/json/reader/internal/parsers/composition_parser_impl.h"
#include "amsr/json/reader/internal/parsers/virtual_parser.h"
#include "amsr/json/reader/json_data.h"

namespace amsr {
namespace json {

namespace {
/*!
 * \brief           A parser that only parses an opening curly bracket
 */
// VCA_VAJSON_MOLE_1298
class StartObjectParser final : public internal::VirtualParser {
 public:
  /*!
   * \brief           Constructs the parser
   */
  // VCA_VAJSON_INTERNAL_CALL
  using internal::VirtualParser::VirtualParser;

  /*!
   * \brief           Implements the StartObject callback returning kFinished
   */
  auto OnStartObject() noexcept -> ParserResult final { return ParserState::kFinished; }
};

/*!
 * \brief           A parser that only parses a closing curly bracket
 */
// VCA_VAJSON_MOLE_1298
class EndObjectParser final : public internal::VirtualParser {
 public:
  /*!
   * \brief           Constructs the parser
   */
  // VCA_VAJSON_INTERNAL_CALL
  using internal::VirtualParser::VirtualParser;

  /*!
   * \brief           Implements the EndObject callback returning kFinished
   */
  auto OnEndObject(std::size_t) noexcept -> ParserResult final { return ParserState::kFinished; }
};

/*!
 * \brief           Parser that only parses an opening square bracket
 */
// VCA_VAJSON_MOLE_1298
class StartArrayParser final : public internal::VirtualParser {
 public:
  /*!
   * \brief           Constructs the parser
   */
  // VCA_VAJSON_INTERNAL_CALL
  using internal::VirtualParser::VirtualParser;

  /*!
   * \brief           Implements the StartArray callback returning kFinished
   */
  auto OnStartArray() noexcept -> ParserResult final { return ParserState::kFinished; }
};

/*!
 * \brief           Parser that only parses a closing square bracket
 */
// VCA_VAJSON_MOLE_1298
class EndArrayParser final : public internal::VirtualParser {
 public:
  /*!
   * \brief           Constructs the parser
   */
  // VCA_VAJSON_INTERNAL_CALL
  using internal::VirtualParser::VirtualParser;

  /*!
   * \brief           Implements the EndArray callback returning kFinished
   */
  auto OnEndArray(std::size_t) noexcept -> ParserResult final { return ParserState::kFinished; }
};

}  // namespace

JsonParser::JsonParser(JsonData& data) noexcept : parser_{data}, data_{data} {}

/*!
 * \internal
 * - If no error occurred before and the start of the object could be parsed successfully:
 *   - Execute the given callable.
 * - If an error occurred, store it as the state of the parser.
 * \endinternal
 */
auto JsonParser::StartObject() noexcept -> JsonParser& {
  // VECTOR NCL MisraC++2023-8.2.2: MD_JSON_MisraC++2023-8.2.2_false_positive
  // VCA_VAJSON_INTERNAL_CALL
  return this->IfValid([this]() noexcept { return StartObjectParser(this->data_.get()).Parse(); });
}

/*!
 * \internal
 * - If no error occurred before and the end of the object could be parsed successfully:
 *   - Execute the given callable.
 * - If an error occurred, store it as the state of the parser.
 * \endinternal
 */
auto JsonParser::EndObject() noexcept -> JsonParser& {
  // VECTOR NCL MisraC++2023-8.2.2: MD_JSON_MisraC++2023-8.2.2_false_positive
  // VCA_VAJSON_INTERNAL_CALL
  return this->IfValid([this]() noexcept { return EndObjectParser(this->data_.get()).Parse(); });
}

/*!
 * \internal
 * - If no error occurred before and the start of the array could be parsed successfully:
 *   - Execute the given callable.
 * - If an error occurred, store it as the state of the parser.
 * \endinternal
 */
auto JsonParser::StartArray() noexcept -> JsonParser& {
  // VECTOR NCL MisraC++2023-8.2.2: MD_JSON_MisraC++2023-8.2.2_false_positive
  // VCA_VAJSON_INTERNAL_CALL
  return this->IfValid([this]() noexcept { return StartArrayParser(this->data_.get()).Parse(); });
}

/*!
 * \internal
 * - If no error occurred before and the end of the array could be parsed successfully:
 *   - Execute the given callable.
 * - If an error occurred, store it as the state of the parser.
 * \endinternal
 */
auto JsonParser::EndArray() noexcept -> JsonParser& {
  // VECTOR NCL MisraC++2023-8.2.2: MD_JSON_MisraC++2023-8.2.2_false_positive
  // VCA_VAJSON_INTERNAL_CALL
  return this->IfValid([this]() noexcept { return EndArrayParser(this->data_.get()).Parse(); });
}

}  // namespace json
}  // namespace amsr
