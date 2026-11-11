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
/// @brief  Contains tests concerning the parsing of strings.

#include "score/json/internal/parser/vajson/structure_parser.h"

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/structure_parser_stub.h"
#include <optional>
#include <string>
#include <vector>

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
namespace
{

using Strings = std::vector<std::string>;

class DynamicStringParser final : public StructureParserStub
{
  public:
    using StructureParserStub::StructureParserStub;

    Strings strings;

  private:
    auto OnString(CStringView str) noexcept -> ParserResult override
    {
        this->strings.push_back(std::string(str));
        return ParserState::kRunning;
    }
};

class ErrorStringParser final : public StructureParserStub
{
  public:
    using StructureParserStub::StructureParserStub;

  private:
    auto OnString(CStringView) noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnString called.");
    }
};

/// Parses a document that consists of a single string and returns the decoded value.
auto ParseString(const std::string_view json) -> std::optional<std::string>
{
    auto data{JsonData::FromBuffer(json)};
    if (!data.has_value())
    {
        return std::nullopt;
    }
    DynamicStringParser parser{data.value()};
    if (!parser.Parse().has_value() || (parser.strings.size() != 1U))
    {
        return std::nullopt;
    }
    return parser.strings.front();
}

}  // namespace

/// Test that string callbacks are called for a string array.
/// @trace           CREQ-Json-EventCallbacks
TEST(UT__Parser__String, Dynamic__StringArray)
{
    auto data{JsonData::FromBuffer(std::string_view{R"(["hello","world"])"})};
    ASSERT_TRUE(data.has_value());
    DynamicStringParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.strings, (Strings{"hello", "world"}));
}

/// Tests that when the overridden OnString method returns an error, that error is propagated.
/// @trace           score::json::vajson::internal::StructureParser::ParseUnescapedString
TEST(UT__Parser__String, Dynamic__ParseUnescapedString__Error)
{
    auto doc{JsonData::FromBuffer(std::string_view{R"("SomeString")"})};
    ASSERT_TRUE(doc.has_value());
    ErrorStringParser parser{doc.value()};

    const auto parser_result{parser.Parse()};
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnString called."});
}

/// Tests that the two-character JSON string escapes are decoded before the string callback.
TEST(UT__Parser__String, Dynamic__TwoCharacterEscapes)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that standard JSON string escapes are decoded before the callback.");

    const auto parsed{ParseString(R"("\"\\\/\b\f\n\r\t")")};
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed.value(), (std::string{"\"\\/\b\f\n\r\t"}));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
