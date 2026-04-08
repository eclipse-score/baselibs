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
 *        \brief  A specialized parser for binary content.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_BINARY_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_BINARY_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>
#include "amsr/json/reader/internal/parsers/virtual_parser.h"
#include "score/functional.hpp"

namespace amsr {
namespace json {
namespace internal {
/*!
 * \brief           A parser that only parses a single binary value
 */
// VCA_VAJSON_MOLE_1298
class BinaryParser final : public VirtualParser {
  /*!
   * \brief           Type of function to be executed when a binary values are read
   */
  using Fn = score::cpp::move_only_function<ResultBlank(Bytes)>;

 public:
  /*!
   * \brief           Constructs a BinaryParser
   * \details         Callback must take the binary content as score::cpp::span<char const> and return ResultBlank.
   * \param[in]       doc
   *                  JSON document to parse.
   * \param[in]       fn
   *                  Function to execute when the binary content is read.
   *
   * \context         ANY
   * \pre             Callback does not throw exceptions.
   * \threadsafe      FALSE
   * \reentrant       FALSE
   */
  // VCA_VAJSON_INTERNAL_CALL
  BinaryParser(JsonData& doc, Fn fn) noexcept : VirtualParser{doc}, fn_{std::move(fn)} {}

  /*!
   * \brief           Event for binary content
   * \param[in]       view
   *                  Read binary content.
   * \return          kFinished if the callback function succeeds, or its error.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   * \internal
   * - Execute the callback with the read binary content.
   * - If the callback succeeds:
   *   - Return kFinished.
   * - Otherwise:
   *   - Return the error of the callback.
   * \endinternal
   */
  auto OnBinary(Bytes view) noexcept -> ParserResult final {
    // VCA_VAJSON_WITHIN_SPEC
    return this->fn_(view).transform([](score::Blank) noexcept { return ParserState::kFinished; });
  }

  /*!
   * \brief           Default event for unexpected elements that aborts the parsing
   *
   * \error           amsr::json::JsonErrc::kUserValidationFailed
   *                  if no binary content is parsed
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  auto OnUnexpectedEvent() noexcept -> ParserResult final {
    return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Expected to parse binary content.");
  }

 private:
  /*!
   * \brief           Function to be executed on binary content
   */
  Fn fn_;
};

}  // namespace internal
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_BINARY_PARSER_H_
