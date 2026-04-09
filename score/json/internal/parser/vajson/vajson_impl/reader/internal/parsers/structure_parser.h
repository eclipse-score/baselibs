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

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/depth_counter.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/json_ops.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/structure_parser_base.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/parser_state.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/number.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include <functional>
#include <string>

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{

/// \brief           A SAX-style JSON parser
/// \details         Parses JSON text from a stream and sends events synchronously to an Implementer.

/// \tparam          Implementer
///                  to use.
/// \trace           DSGN-JSON-Reader-Data-Items
template <typename Implementer>

class StructureParser : public StructureParserBase
{
    /// \brief           Callback implementer
    std::reference_wrapper<Implementer> implementer_;

    /// \brief           Operations on JSON document
    JsonOps json_ops_;

  public:
    /// \brief           Default event for Null
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnNull() noexcept -> ParserResult override;

    /// \brief           Default event for Bools
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnBool(bool) noexcept -> ParserResult override;  // NOLINT(readability/casting)

    /// \brief           Default event for Numbers
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnNumber(JsonNumber) noexcept -> ParserResult override;

    /// \brief           Default event for Strings
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnString(CStringView) noexcept -> ParserResult override;

    /// \brief           Default event for Keys
    /// \details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnKey(CStringView) noexcept -> ParserResult override;

    /// \brief           Default event for the end of objects
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnStartObject() noexcept -> ParserResult override;

    /// \brief           Default event for the start of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnEndObject(std::size_t) noexcept -> ParserResult override;

    /// \brief           Default event for the end of arrays
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnStartArray() noexcept -> ParserResult override;

    /// \brief           Default event for binary content
    /// \details         The provided Span is only valid until any other method or parser operating on the same document
    /// is
    ///                  called. Call the OnUnexpectedEvent callback that determines how to handle the unexpected event
    ///                  and return its result.
    /// \return          The result of the OnUnexpectedEvent callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto OnEndArray(std::size_t) noexcept -> ParserResult override;

    /// \brief           Forwarding function to accept binary strings
    /// \return          The result of the OnString callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto OnBinaryKey(StringView) noexcept -> ParserResult override;

    /// \brief           Forwarding function to accept binary strings
    /// \return          The result of the OnString callback or its error.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto OnBinaryString(StringView) noexcept -> ParserResult override;

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
    auto OnBinary(score::cpp::span<const char>) noexcept -> ParserResult override;

    /// \brief           Constructs a StructureParser
    /// \param[in]       implementer
    ///                  The implementer for this parser.
    /// \param[in]       doc
    ///                  JSON document to parse.
    /// \context         ANY
    /// \pre             Implementer does not throw exceptions.
    /// \threadsafe      FALSE
    /// \reentrant       FALSE
    StructureParser(Implementer& implementer, JsonData& doc) noexcept;

    /// \brief           Default move constructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    StructureParser(StructureParser&&) noexcept = default;

    /// \brief           Default move assignment
    /// \return          A reference to the moved into object.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto operator=(StructureParser&&) & noexcept -> StructureParser& = default;

    /// \brief           Deleted copy constructor
    StructureParser(const StructureParser&) = delete;

    /// \brief           Deleted copy assignment
    auto operator=(const StructureParser&) -> StructureParser& = delete;

    /// \brief           Default destructor
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    ~StructureParser() noexcept override = default;

    /// \brief           Parses the file until the current parser is finished
    /// \details         Parserstate will be kRunning to allow for further parsing.
    /// \return          kRunning, or an error.
    /// \error           score::json::vajson::JsonErrc::kInvalidJson
    ///                  if parsing has failed due to invalid JSON data.
    /// \error           score::json::vajson::JsonErrc::kUserValidationFailed
    ///                  if an unknown value has been encountered.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      FALSE
    /// \reentrant       FALSE

    auto SubParse() const noexcept -> ParserResult;

  private:
    /// \brief           Returns the reference to the encoded file
    /// \return          Reference to the encoded file.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto GetJsonOps() & noexcept -> JsonOps& override;

    /// \brief           Returns the reference to the encoded file
    /// \return          Reference to the encoded file const.
    /// \context         ANY
    /// \pre             -
    /// \threadsafe      TRUE, for different this pointer

    auto GetJsonOps() const& noexcept -> const JsonOps& override;

    // Deleted rvalue-qualified version to prevent dangling reference
    auto GetJsonOps() const&& noexcept -> const JsonOps& = delete;
};

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_H_
