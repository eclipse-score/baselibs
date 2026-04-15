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
 *        \brief  Contains a SAX-style JSON parser.
 *
 *      \details  Provides callbacks for implementing parsers.
 *
 *********************************************************************************************************************/

#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_VIRTUAL_PARSER_H_
#define SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_VIRTUAL_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/structure_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/structure_parser_impl.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

/// \brief           A SAX-style JSON parser
/// \details         The VirtualParser implements an interface for parsers and is based on dynamic polymorphism. The
///                  derived parser can implement callbacks for all types of elements it expects to appear. If the
///                  parser encounters any other type, it calls OnUnexpectedEvent. If this callback is not overridden by
///                  the derived parser, the default implementation aborts parsing.
/// \trace           DSGN-JSON-Reader-Data-Items
class VirtualParser
{
  public:
    /// \brief            Friend declaration, in order to make StructureParser able to access private methods of
    /// VirtualParser
    template <typename T>

    friend class StructureParser;

    /// \brief           Constructs a VirtualParser
    /// \param[in]       doc
    ///                  to parse.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    explicit VirtualParser(JsonData& doc) noexcept;

    /// \brief           Deleted copy constructor
    VirtualParser(const VirtualParser&) = delete;

    /// \brief           Deleted copy assignment
    auto operator=(const VirtualParser&) -> VirtualParser& = delete;

    /// \brief           Deleted move constructor
    VirtualParser(VirtualParser&&) = delete;

    /// \brief           Deleted move assignment
    auto operator=(VirtualParser&&) -> VirtualParser& = delete;

    /// \brief           Default destructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual ~VirtualParser() noexcept = default;

    /// \brief           Parses file until the current parser is finished
    /// \return          The empty Result, or an error.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if an unknown value has been encountered.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if parsing has failed due to invalid JSON data.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto Parse() noexcept -> Result<vajson::Blank>;

    /// \brief           Parses file until the current parser is finished
    /// \details         Parserstate will be kRunning to allow for further parsing.
    /// \return          A ParserResult.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if an unknown value has been encountered.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if parsing has failed due to invalid JSON data.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    auto SubParse() const noexcept -> ParserResult;

    /// \brief           Returns a reference to the CRTP child
    /// \return          Reference to the CRTP child.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetChild() & noexcept -> VirtualParser&;

  protected:
    /// \brief           Returns the current key
    /// \return          THe current key.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetCurrentKey() const noexcept -> CStringView;

    /// \brief           Returns a reference to the JSON file
    /// \return          Reference to the JSON file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() noexcept -> JsonData&;

    /// \brief           Returns a reference to the JSON file
    /// \return          Const reference to the JSON file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    auto GetJsonDocument() const noexcept -> const JsonData&;

  private:
    /// \brief           Default event for Null
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnNull() noexcept -> ParserResult;

    /// \brief           Default event for Bools
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnBool(bool) noexcept -> ParserResult;  // NOLINT(readability/casting)

    /// \brief           Default event for Numbers
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnNumber(JsonNumber) noexcept -> ParserResult;

    /// \brief           Default event for Strings
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnString(StringView) noexcept -> ParserResult;

    /// \brief           Default event for Keys
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnKey(StringView) noexcept -> ParserResult;

    /// \brief           Default event for the start of objects
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnStartObject() noexcept -> ParserResult;

    /// \brief           Default event for the end of objects
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnEndObject(std::size_t) noexcept -> ParserResult;

    /// \brief           Default event for the start of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnStartArray() noexcept -> ParserResult;

    /// \brief           Default event for the end of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    virtual auto OnEndArray(std::size_t) noexcept -> ParserResult;

    /// \brief           Default event for default callbacks that aborts parsing
    /// \details         This callback is called by all default event callbacks which are not overridden by the derived
    ///                  parser. It allows the parser to continue parsing if an unexpected event is encountered (i.e. a
    ///                  JSON element for which no callback is implemented by the derived parser). It may be overridden
    ///                  by the derived parser if a different behaviour is desired.
    /// \return          An error result.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer
    virtual auto OnUnexpectedEvent() noexcept -> ParserResult;

    /// \brief           Generic JSON structure parser
    StructureParser<VirtualParser> parser_;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_VAJSON_JSON_READER_INTERNAL_PARSERS_VIRTUAL_PARSER_H_
