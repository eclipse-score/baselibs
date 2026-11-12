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
/// @file
/// @brief  A SAX-Style JSON parser.
///
/// @details  Parses JSON text from a stream and sends events synchronously to the derived class.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H

#include <cstddef>

#include "score/json/internal/parser/vajson/depth_counter.h"
#include "score/json/internal/parser/vajson/json_number.h"
#include "score/json/internal/parser/vajson/json_ops.h"
#include "score/json/internal/parser/vajson/parser_state.h"
#include "score/json/internal/parser/vajson/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

/// @brief           A SAX-style JSON parser that reports each JSON element to the callbacks of the derived class.
/// @trace           DSGN-JSON-Reader-Data-Items
class StructureParser
{
  public:
    /// @brief           Constructs a StructureParser
    /// @param[in]       doc
    ///                  JSON document to parse. Must outlive the parser.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    explicit StructureParser(JsonData& doc) noexcept;

    /// @brief           Deleted copy constructor
    StructureParser(const StructureParser&) = delete;
    /// @brief           Deleted move constructor
    StructureParser(StructureParser&&) = delete;

    /// @brief           Deleted copy assignment
    StructureParser& operator=(const StructureParser&) = delete;

    /// @brief           Deleted move assignment
    StructureParser& operator=(StructureParser&&) = delete;

    /// @brief           Default destructor
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual ~StructureParser() noexcept = default;

    /// @brief           Parses the file until the current parser is finished
    /// @return          The empty Result, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if parsing has failed due to invalid JSON data.
    /// @error           Any error returned by a callback.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto Parse() noexcept -> Result<void>;

  private:
    /// @brief           Called for a null value
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnNull() noexcept -> ParserResult = 0;

    /// @brief           Called for a boolean value
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnBool(bool) noexcept -> ParserResult = 0;  // NOLINT(readability/casting)

    /// @brief           Called for a number value
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnNumber(JsonNumber) noexcept -> ParserResult = 0;

    /// @brief           Called for a string value
    /// @details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnString(CStringView) noexcept -> ParserResult = 0;

    /// @brief           Called for an object key
    /// @details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnKey(CStringView) noexcept -> ParserResult = 0;

    /// @brief           Called for the start of an object
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnStartObject() noexcept -> ParserResult = 0;

    /// @brief           Called for the end of an object with its number of keys
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnEndObject(std::size_t) noexcept -> ParserResult = 0;

    /// @brief           Called for the start of an array
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnStartArray() noexcept -> ParserResult = 0;

    /// @brief           Called for the end of an array with its number of elements
    /// @return          The parser state, or an error that aborts parsing.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    virtual auto OnEndArray(std::size_t) noexcept -> ParserResult = 0;

    /// @brief           Returns the Json structure state
    /// @return          The Json structure state.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetState() noexcept -> DepthCounter&;

    /// @brief           Returns a reference to the Json file
    /// @return          Reference to the Json file.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() noexcept -> JsonData&;

    /// @brief           Parses a "null" value
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if no valid 'null' literal was encountered.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseNull() noexcept -> ParserResult;

    /// @brief           Parses a "true" value
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if no valid string was encountered.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseTrue() noexcept -> ParserResult;

    /// @brief           Parses a "false" value
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the 'false' literal cannot be parsed
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseFalse() noexcept -> ParserResult;

    /// @brief           Parses a number value
    /// @param[in]       cur
    ///                  The current character.
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the number literal cannot be parsed
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseNumber(char cur) noexcept -> ParserResult;

    /// @brief           Parses a string value
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the next token is no valid string.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseString() noexcept -> ParserResult;

    /// @brief           Parses an unescaped string value
    /// @param[in]       string
    ///                  The unescaped string to parse.
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the next token is no valid string.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseUnescapedString(CStringView string) noexcept -> ParserResult;

    /// @brief           Parses a JSON string and removes JSON escape characters
    /// @return          A string view into the parsed string, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if an invalid JSON string was encountered.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ReadJsonString() noexcept -> Result<CStringView>;

    /// @brief           Unescapes a single character
    /// @param[in]       escaped
    ///                  The character to unescape.
    /// @return          The unescaped character.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If an unknown/unsupported escape sequence was encountered
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    static auto UnescapeChar(char escaped) noexcept -> Result<char>;

    /// @brief           Parses the start of an Object
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the object cannot be opened
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseStartObject() noexcept -> ParserResult;

    /// @brief           Parses the end of an Object
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseEndObject() noexcept -> ParserResult;

    /// @brief           Parses the start of an Array
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseStartArray() noexcept -> ParserResult;

    /// @brief           Parses the end of an Array
    /// @return          The Result of the callback, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  If no end of an object can be parsed
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseEndArray() noexcept -> ParserResult;

    /// @brief           Parses a comma in a list/object
    /// @return          kRunning if the parser is in an object or array, or an error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if in an object or on toplevel
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseComma() noexcept -> ParserResult;

    /// @brief           Parses any JSON value
    /// @return          kRunning if the parser has not finished yet, kFinished if the end of the file is reached, or an
    ///                  error.
    /// @error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if the JSON data is invalid.
    /// @error           Any error returned by a callback.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto ParseValue() noexcept -> ParserResult;

    /// @brief           Parses a (more or less valid) number
    /// @param[in]       first
    ///                  The first character of the number.
    /// @return          The parsed number string.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto GetNumber(char first) noexcept -> CStringView;

    /// @brief           Operations on the JSON document
    JsonOps json_ops_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H
