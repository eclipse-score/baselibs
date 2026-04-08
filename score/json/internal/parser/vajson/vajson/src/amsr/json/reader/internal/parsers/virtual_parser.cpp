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
 *        \brief  Contains the implementation of a SAX-style JSON parser.
 *
 *      \details  Provides callbacks for implementing parsers.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/internal/parsers/virtual_parser.h"
#include "amsr/json/reader/internal/parsers/structure_parser.h"
#include "amsr/json/reader/internal/parsers/structure_parser_impl.h"
#include "amsr/json/reader/json_data.h"
#include "amsr/json/util/json_error_domain.h"

namespace amsr {
namespace json {
namespace internal {
VirtualParser::VirtualParser(JsonData& doc) noexcept : parser_{*this, doc} {}

auto VirtualParser::Parse() noexcept -> Result<score::Blank> { return this->parser_.Parse(); }

auto VirtualParser::SubParse() const noexcept -> ParserResult { return this->parser_.SubParse(); }

auto VirtualParser::GetChild() & noexcept -> VirtualParser& { return *this; }

auto VirtualParser::GetCurrentKey() const noexcept -> CStringView { return this->GetJsonDocument().GetCurrentKey(); }

auto VirtualParser::GetJsonDocument() noexcept -> JsonData& { return this->parser_.GetJsonDocument(); }

auto VirtualParser::GetJsonDocument() const noexcept -> JsonData const& { return this->parser_.GetJsonDocument(); }

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnNull() noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnBool(bool) noexcept -> ParserResult {  // NOLINT(readability/casting)

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnNumber(JsonNumber) noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnString(StringView) noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnKey(StringView) noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnStartObject() noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnEndObject(std::size_t) noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnStartArray() noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}

/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnEndArray(std::size_t) noexcept -> ParserResult {

  return this->OnUnexpectedEvent();
}
/*!
 * \brief           Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return
 *                  its result
 */
auto VirtualParser::OnUnexpectedEvent() noexcept -> ParserResult {
  return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "Use of default method not allowed in this context.");
}

}  // namespace internal
}  // namespace json
}  // namespace amsr
