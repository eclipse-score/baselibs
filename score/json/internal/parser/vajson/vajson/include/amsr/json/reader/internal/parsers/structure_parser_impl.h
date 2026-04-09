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
 *        \brief  The implementation of a SAX-Style JSON parser.
 *
 *      \details  Parses JSON text from a stream and sends events synchronously to an Implementer.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_IMPL_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_IMPL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "amsr/json/reader/internal/parsers/structure_parser.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include "amsr/json/reader/internal/depth_counter.h"
#include "amsr/json/reader/internal/json_ops.h"
#include "amsr/json/reader/parser_state.h"
#include "amsr/json/util/json_error_domain.h"
#include "amsr/json/util/number.h"
#include "amsr/json/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief           Callbacks to an implementer
template <typename Implementer>
auto StructureParser<Implementer>::OnNull() noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnNull());
}
template <typename Implementer>
auto StructureParser<Implementer>::OnBool(bool v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnBool(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnNumber(JsonNumber v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnNumber(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnString(CStringView v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnString(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnKey(CStringView v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnKey(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnStartObject() noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnStartObject());
}
template <typename Implementer>
auto StructureParser<Implementer>::OnEndObject(std::size_t v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnEndObject(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnStartArray() noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnStartArray());
}
template <typename Implementer>
auto StructureParser<Implementer>::OnEndArray(std::size_t v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnEndArray(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnBinaryKey(StringView v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnBinaryKey(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnBinaryString(StringView v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnBinaryString(v));
}
template <typename Implementer>
auto StructureParser<Implementer>::OnBinary(score::cpp::span<const char> v) noexcept -> ParserResult
{

    return ParserResult(this->implementer_.get().GetChild().OnBinary(v));
}

template <typename Implementer>
StructureParser<Implementer>::StructureParser(Implementer& implementer, JsonData& doc) noexcept

    : StructureParserBase(), implementer_{implementer}, json_ops_(doc)
{
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

template <typename Implementer>
auto StructureParser<Implementer>::SubParse() const noexcept -> ParserResult
{

    return this->implementer_.get().GetChild().Parse().transform([](score::Blank) noexcept {
        return ParserState::kRunning;
    });
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_INTERNAL_PARSERS_STRUCTURE_PARSER_IMPL_H_
