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
 *        \brief  Contains tests concerning the parsing of booleans.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
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

/*!
 * \brief           Parser that is only interested in bools
 */
class DynamicBoolParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

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

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Count true_{0};
    Count false_{0};
};

/*!
 * \brief           Parser that always returns an error on call of OnBool
 */
class ErrorBoolParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnBool(bool) noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnBool called.");
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace

/*!
 * Test that callbacks for a boolean value can be registered — single true.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Bool, Dynamic__True)
{
    auto data = JsonData::FromBuffer(std::string_view{"true"});
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 1U);
    EXPECT_EQ(parser.false_, 0U);
}

/*!
 * Test that callbacks for a boolean value can be registered — single false.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Bool, Dynamic__False)
{
    auto data = JsonData::FromBuffer(std::string_view{"false"});
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 0U);
    EXPECT_EQ(parser.false_, 1U);
}

/*!
 * Test that callbacks for boolean values can be registered — array of bools.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Bool, Dynamic__Bools)
{
    auto data = JsonData::FromBuffer(std::string_view{"[true,true,false,false,false]"});
    ASSERT_TRUE(data.has_value());
    DynamicBoolParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.true_, 2U);
    EXPECT_EQ(parser.false_, 3U);
}

/*!
 * Test that parsing fails if 'true' is not expected.
 * \trace           ::score::json::vajson::internal::StructureParserBase::ParseValue
 */
TEST(UT__Parser__Bool, Dynamic__ParseTrue_WrongFormat)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({true:123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicBoolParser parser{data};
    const auto parse_result = parser.Parse();
    ASSERT_FALSE(parse_result.has_value());
    ASSERT_EQ(parse_result.error(), JsonErrc::kInvalidJson);
}

/*!
 * Test that parsing fails if 'false' is not expected.
 * \trace           ::score::json::vajson::internal::StructureParserBase::ParseValue
 */
TEST(UT__Parser__Bool, Dynamic__ParseFalse__WrongFormat)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({false:123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicBoolParser parser{data};
    const auto parse_result = parser.Parse();
    ASSERT_FALSE(parse_result.has_value());
    ASSERT_EQ(parse_result.error(), JsonErrc::kInvalidJson);
}

/*!
 * Tests that when the overridden OnBool method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseFalse
 */
TEST(UT__Parser__Bool, Dynamic__ParseFalse__Error)
{
    auto doc = JsonData::FromBuffer(std::string_view{"false"});
    ASSERT_TRUE(doc.has_value());
    ErrorBoolParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnBool called."});
}

/*!
 * Tests that when the overridden OnBool method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseTrue
 */
TEST(UT__Parser__Bool, Dynamic__ParseTrue__Error)
{
    auto doc = JsonData::FromBuffer(std::string_view{"true"});
    ASSERT_TRUE(doc.has_value());
    ErrorBoolParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnBool called."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
