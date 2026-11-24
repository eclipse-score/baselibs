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
/// @brief  Contains tests concerning the parsing of booleans.

#include "score/json/internal/parser/vajson/structure_parser.h"

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/structure_parser_stub.h"
#include <utility>

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

using Count = std::size_t;

/// @brief           Parser that is only interested in bools
class DynamicBoolParser final : public StructureParserStub
{
  public:
    using StructureParserStub::StructureParserStub;

    Count true_{0};
    Count false_{0};

  private:
    auto OnBool(bool b) noexcept -> ParserResult override
    {
        if (b)
        {
            this->true_ += 1;
        }
        else
        {
            this->false_ += 1;
        }
        return ParserState::kRunning;
    }
};

/// @brief           Parser that always returns an error on call of OnBool
class ErrorBoolParser final : public StructureParserStub
{
  public:
    using StructureParserStub::StructureParserStub;

  private:
    auto OnBool(bool) noexcept -> ParserResult override
    {
        return score::MakeUnexpected(JsonErrc::kInvalidJson, "OnBool called.");
    }
};

}  // namespace

/// Test that callbacks for a boolean value can be registered — single true.
/// @trace           CREQ-Json-EventCallbacks
TEST(StructureParserBoolTest, CallsOnBoolWithTrue)
{
    auto data{JsonData::FromBuffer(std::string_view{"true"})};
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 1U);
    EXPECT_EQ(parser.false_, 0U);
}

/// Test that callbacks for a boolean value can be registered — single false.
/// @trace           CREQ-Json-EventCallbacks
TEST(StructureParserBoolTest, CallsOnBoolWithFalse)
{
    auto data{JsonData::FromBuffer(std::string_view{"false"})};
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 0U);
    EXPECT_EQ(parser.false_, 1U);
}

/// Test that callbacks for boolean values can be registered — array of bools.
/// @trace           CREQ-Json-EventCallbacks
TEST(StructureParserBoolTest, CallsOnBoolForEachBoolInArray)
{
    auto data{JsonData::FromBuffer(std::string_view{"[true,true,false,false,false]"})};
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 2U);
    EXPECT_EQ(parser.false_, 3U);
}

/// Test that parsing fails if 'true' is not expected.
/// @trace           ::score::json::vajson::internal::StructureParser::ParseValue
TEST(StructureParserBoolTest, RejectsTrueAsKey)
{
    auto result{JsonData::FromBuffer(std::string_view{R"({true:123})"})};
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicBoolParser parser{data};
    const auto parse_result{parser.Parse()};
    ASSERT_FALSE(parse_result.has_value());
    ASSERT_EQ(parse_result.error(), JsonErrc::kInvalidJson);
}

/// Test that parsing fails if 'false' is not expected.
/// @trace           ::score::json::vajson::internal::StructureParser::ParseValue
TEST(StructureParserBoolTest, RejectsFalseAsKey)
{
    auto result{JsonData::FromBuffer(std::string_view{R"({false:123})"})};
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicBoolParser parser{data};
    const auto parse_result{parser.Parse()};
    ASSERT_FALSE(parse_result.has_value());
    ASSERT_EQ(parse_result.error(), JsonErrc::kInvalidJson);
}

TEST(StructureParserBoolTest, RejectsMalformedLiteralWithReason)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description",
                   "Check that malformed 'true' and 'false' literals are rejected with the exact reason message.");

    const auto parse{[](const std::string_view input) {
        auto result{JsonData::FromBuffer(input)};
        EXPECT_TRUE(result.has_value());
        JsonData data{std::move(result).value()};
        return DynamicBoolParser{data}.Parse();
    }};

    const auto true_result{parse("trux")};
    ASSERT_FALSE(true_result.has_value());
    EXPECT_EQ(true_result.error().UserMessage(), std::string_view{"StructureParser::ParseTrue: Expected 'true'"});

    const auto false_result{parse("falsx")};
    ASSERT_FALSE(false_result.has_value());
    EXPECT_EQ(false_result.error().UserMessage(), std::string_view{"StructureParser::ParseFalse: Expected 'false'"});
}

/// Tests that when the overridden OnBool method returns an error, that error is propagated.
/// @trace           score::json::vajson::internal::StructureParser::ParseFalse
TEST(StructureParserBoolTest, PropagatesOnBoolErrorForFalse)
{
    auto doc{JsonData::FromBuffer(std::string_view{"false"})};
    ASSERT_TRUE(doc.has_value());
    ErrorBoolParser parser{doc.value()};

    const auto parser_result{parser.Parse()};
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnBool called."});
}

/// Tests that when the overridden OnBool method returns an error, that error is propagated.
/// @trace           score::json::vajson::internal::StructureParser::ParseTrue
TEST(StructureParserBoolTest, PropagatesOnBoolErrorForTrue)
{
    auto doc{JsonData::FromBuffer(std::string_view{"true"})};
    ASSERT_TRUE(doc.has_value());
    ErrorBoolParser parser{doc.value()};

    const auto parser_result{parser.Parse()};
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnBool called."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
