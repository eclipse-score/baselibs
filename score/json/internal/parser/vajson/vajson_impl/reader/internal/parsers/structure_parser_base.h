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
 *        \brief  A SAX-Style JSON parser.
 *
 *      \details  Parses JSON text from a stream and sends events synchronously to an Implementer.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_BASE_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_BASE_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

#include <string>

#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/depth_counter.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/number.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

/// \brief           Contains the logic to structure_parser
/// \details         Vtable is shared among all objects of a StructureParserBase When StructureParser inherits from
///                  StructureParserBase, it doesn't increase object size per virtual function in the base class.
///                  When the base class is not a template, the templated derived class does not need multiple vtables
///                  for each template instantiation, but can just point to the base class

/// \trace           DSGN-JSON-Reader-Data-Items
class StructureParserBase
{
  public:
    StructureParserBase() noexcept = default;
    /// \brief           Deleted copy constructor
    StructureParserBase(const StructureParserBase&) = delete;
    /// \brief           Deleted move constructor
    StructureParserBase(StructureParserBase&&) = delete;

    //  Deleted copy assignment.
    StructureParserBase& operator=(const StructureParserBase&) = delete;

    // Deleted move assignment.
    StructureParserBase& operator=(StructureParserBase&&) = delete;

    /// \brief           Default destructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual ~StructureParserBase() noexcept = default;

    /// \brief           Default event for Null
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnNull() noexcept -> ParserResult = 0;

    /// \brief           Default event for Bools
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnBool(bool) noexcept -> ParserResult = 0;  // NOLINT(readability/casting)

    /// \brief           Default event for Numbers
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnNumber(JsonNumber) noexcept -> ParserResult = 0;
    /// \brief           Default event for Strings
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnString(CStringView) noexcept -> ParserResult = 0;

    /// \brief           Default event for Keys
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnKey(CStringView) noexcept -> ParserResult = 0;

    /// \brief           Default event for the end of objects
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnStartObject() noexcept -> ParserResult = 0;

    /// \brief           Default event for the start of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnEndObject(std::size_t) noexcept -> ParserResult = 0;

    /// \brief           Default event for the end of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    virtual auto OnStartArray() noexcept -> ParserResult = 0;

    /// \brief           Default event for binary content
    /// \details         The provided Span is only valid until any other method or parser operating on the same document
    /// is
    ///                  called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    /// \internal
    /// - Call the OnUnexpectedEvent callback that determines how to handle the unexpected event and return its result.
    /// \endinternal
    virtual auto OnEndArray(std::size_t) noexcept -> ParserResult = 0;

    /// \brief           Returns the reference to the encoded file
    /// \return          Reference to the encoded file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    virtual auto GetJsonOps() & noexcept -> JsonOps& = 0;

    /// \brief           Returns the reference to the encoded file
    /// \return          Reference to the encoded file const.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    virtual auto GetJsonOps() const& noexcept -> const JsonOps& = 0;

    /// \brief           Returns the Json structure state
    /// \return          The Json structure state.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetState() noexcept -> DepthCounter&;

    /// \brief           Returns a reference to the Json file
    /// \return          Reference to the Json file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() noexcept -> JsonData&;

    /// \brief           Returns a reference to the Json file
    /// \return          Const reference to the Json file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() const noexcept -> const JsonData&;

    /// \brief           Parses the file until the current parser is finished
    /// \return          The empty Result, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if parsing has failed due to invalid JSON data.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if an unknown value has been encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto Parse() noexcept -> Result<score::Blank>;

    /// \brief           Parses a "null" value
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if no valid 'null' literal was encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ParseNull() noexcept -> ParserResult;

    /// \brief           Parses a "true" value
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if no valid string was encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ParseTrue() noexcept -> ParserResult;

    /// \brief           Parses a "false" value
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajsonJsonErrc::kInvalidJson
    ///                  if the 'false' literal cannot be parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ParseFalse() noexcept -> ParserResult;

    /// \brief           Parses a number value
    /// \param[in]       cur
    ///                  The current character.
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the number literal cannot be parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto ParseNumber(char cur) noexcept -> ParserResult;

    /// \brief           Parses a string value
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the next token is no valid string.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseString() noexcept -> ParserResult;

    /// \brief           Parses an unescaped string value
    /// \param[in]       string
    ///                  The unescaped string to parse.
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the next token is no valid string.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseUnescapedString(CStringView string) noexcept -> ParserResult;

    /// \brief           Parses a JSON string and removes JSON escape characters
    /// \return          A string view into the parsed string, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if an invalid JSON string was encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ReadJsonString() noexcept -> Result<CStringView>;

    /// \brief           Unescapes a single character
    /// \param[in]       escaped
    ///                  The character to unescape.
    /// \return          The unescaped character.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If an unknown/unsupported escape sequence was encountered
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    static auto UnescapeChar(char escaped) noexcept -> Result<char>;

    /// \brief           Parses the start of an Object
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the object cannot be opened
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseStartObject() noexcept -> ParserResult;

    /// \brief           Parses the end of an Object
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseEndObject() noexcept -> ParserResult;

    /// \brief           Parses the start of an Array
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseStartArray() noexcept -> ParserResult;

    /// \brief           Parses the end of an Array
    /// \return          The Result of the implementer callback, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseEndArray() noexcept -> ParserResult;

    /// \brief           Parses a comma in a list/object
    /// \return          kRunning if the parser is in an object or array, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if in an object or on toplevel
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseComma() noexcept -> ParserResult;

    /// \brief           Parses any JSON value
    /// \return          kRunning if the parser has not finished yet, kFinished if the end of the file is reached, or an
    ///                  error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the JSON data is invalid.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if an unknown value has been encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto ParseValue() noexcept -> ParserResult;

    /// \brief           Parses a (more or less valid) number
    /// \param[in]       first
    ///                  The first character of the number.
    /// \return          The parsed number string.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto GetNumber(char first) noexcept -> CStringView;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_BASE_H_
