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
#ifndef SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_STUB_H
#define SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_STUB_H

#include "score/json/internal/parser/vajson/structure_parser.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{

/// @brief Test stub that accepts every event, so that tests only override the callbacks they observe.
class StructureParserStub : public internal::StructureParser
{
  public:
    using internal::StructureParser::StructureParser;

  private:
    auto OnNull() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnBool(bool) noexcept -> ParserResult override  // NOLINT(readability/casting)
    {
        return ParserState::kRunning;
    }

    auto OnNumber(JsonNumber) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnString(CStringView) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnKey(CStringView) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnStartObject() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnEndObject(std::size_t) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnStartArray() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnEndArray(std::size_t) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_JSON_INTERNAL_PARSER_VAJSON_STRUCTURE_PARSER_STUB_H
