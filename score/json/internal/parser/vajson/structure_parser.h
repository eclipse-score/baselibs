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
/// @details  Parses JSON text from a stream and sends events synchronously to an Implementer.

#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H

#include "score/json/internal/parser/vajson/depth_counter.h"
#include "score/json/internal/parser/vajson/json_number.h"
#include "score/json/internal/parser/vajson/json_ops.h"
#include "score/json/internal/parser/vajson/parser_state.h"
#include "score/json/internal/parser/vajson/structure_parser_base.h"
#include "score/json/internal/parser/vajson/types.h"
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

/// @brief           A SAX-style JSON parser
/// @details         Parses JSON text from a stream and sends events synchronously to an Implementer.
/// @tparam          Implementer
///                  to use.
/// @trace           DSGN-JSON-Reader-Data-Items
template <typename Implementer>
class StructureParser : public StructureParserBase
{
  public:
    /// @brief           Constructs a StructureParser
    /// @param[in]       implementer
    ///                  The implementer for this parser.
    /// @param[in]       doc
    ///                  JSON document to parse.
    /// @context         ANY
    /// @pre             Implementer does not throw exceptions.
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    StructureParser(Implementer& implementer, JsonData& doc) noexcept;

    /// @brief           Default move constructor
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    StructureParser(StructureParser&&) noexcept = default;

    /// @brief           Default move assignment
    /// @return          A reference to the moved into object.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto operator=(StructureParser&&) & noexcept -> StructureParser& = default;

    /// @brief           Deleted copy constructor
    StructureParser(const StructureParser&) = delete;

    /// @brief           Deleted copy assignment
    auto operator=(const StructureParser&) -> StructureParser& = delete;

    /// @brief           Default destructor
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    ~StructureParser() noexcept override = default;

    /// @brief           Default event for Null
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnNull() noexcept -> ParserResult override;

    /// @brief           Default event for Bools
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnBool(bool) noexcept -> ParserResult override;  // NOLINT(readability/casting)

    /// @brief           Default event for Numbers
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnNumber(JsonNumber) noexcept -> ParserResult override;

    /// @brief           Default event for Strings
    /// @details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnString(CStringView) noexcept -> ParserResult override;

    /// @brief           Default event for Keys
    /// @details         The provided StringView is only valid until any other method or parser operating on the same
    ///                  document is called.
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnKey(CStringView) noexcept -> ParserResult override;

    /// @brief           Default event for the start of objects
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnStartObject() noexcept -> ParserResult override;

    /// @brief           Default event for the end of objects
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnEndObject(std::size_t) noexcept -> ParserResult override;

    /// @brief           Default event for the start of arrays
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnStartArray() noexcept -> ParserResult override;

    /// @brief           Default event for the end of arrays
    /// @return          The result of the OnUnexpectedEvent callback or its error.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      FALSE
    /// @reentrant       FALSE
    auto OnEndArray(std::size_t) noexcept -> ParserResult override;

  private:
    /// @brief           Returns the reference to the encoded file
    /// @return          Reference to the encoded file.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetJsonOps() & noexcept -> JsonOps& override;

    /// @brief           Returns the reference to the encoded file
    /// @return          Reference to the encoded file const.
    /// @context         ANY
    /// @pre             -
    /// @threadsafe      TRUE, for different this pointer
    auto GetJsonOps() const& noexcept -> const JsonOps& override;

    // Deleted rvalue-qualified version to prevent dangling reference
    auto GetJsonOps() const&& noexcept -> const JsonOps& = delete;

    /// @brief           Callback implementer
    std::reference_wrapper<Implementer> implementer_;

    /// @brief           Operations on JSON document
    JsonOps json_ops_;
};

template <typename Implementer>
StructureParser<Implementer>::StructureParser(Implementer& implementer, JsonData& doc) noexcept
    : implementer_{implementer}, json_ops_{doc}
{
}

// The event callbacks forward to the implementer.
template <typename Implementer>
auto StructureParser<Implementer>::OnNull() noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnNull());
}

template <typename Implementer>
auto StructureParser<Implementer>::OnBool(bool v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnBool(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::OnNumber(JsonNumber v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnNumber(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::OnString(CStringView v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnString(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::OnKey(CStringView v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnKey(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::OnStartObject() noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnStartObject());
}

template <typename Implementer>
auto StructureParser<Implementer>::OnEndObject(std::size_t v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnEndObject(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::OnStartArray() noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnStartArray());
}

template <typename Implementer>
auto StructureParser<Implementer>::OnEndArray(std::size_t v) noexcept -> ParserResult
{
    return ParserResult(this->implementer_.get().OnEndArray(v));
}

template <typename Implementer>
auto StructureParser<Implementer>::GetJsonOps() & noexcept -> JsonOps&
{
    return this->json_ops_;
}

template <typename Implementer>
auto StructureParser<Implementer>::GetJsonOps() const& noexcept -> const JsonOps&
{
    return this->json_ops_;
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_H
