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
 *        \brief  Contains tests concerning the parsing of null values.
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

class DynamicNullParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnNull() noexcept -> ParserResult override
    {
        this->null_ += 1;
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Count null_{0};
};

class ErrorNullParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnNull() noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnNull called.");
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace

/*!
 * Test that null is counted — single null value.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Null, Dynamic__SingleNull)
{
    auto data = JsonData::FromBuffer(std::string_view{"null"});
    ASSERT_TRUE(data.has_value());
    DynamicNullParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.null_, 1U);
}

/*!
 * Test that null is counted — multiple null values.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Null, Dynamic__MultipleNulls)
{
    auto data = JsonData::FromBuffer(std::string_view{"[null,null,null]"});
    ASSERT_TRUE(data.has_value());
    DynamicNullParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.null_, 3U);
}

/*!
 * Test that no nulls are counted when not present.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Null, Dynamic__NoNulls)
{
    auto data = JsonData::FromBuffer(std::string_view{"true"});
    ASSERT_TRUE(data.has_value());
    DynamicNullParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.null_, 0U);
}

/*!
 * Test that parsing fails if 'null' is used as key without quotation marks.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseValue
 */
TEST(UT__Parser__Null, Dynamic__WrongFormat)
{
    auto result = JsonData::FromBuffer(std::string_view{R"({null:123})"});
    ASSERT_TRUE(result.has_value());
    JsonData data{std::move(result).value()};

    DynamicNullParser parser{data};
    const auto parse_result = parser.Parse();
    ASSERT_FALSE(parse_result.has_value());
    ASSERT_EQ(parse_result.error(), JsonErrc::kInvalidJson);
}

/*!
 * Tests that when the overridden OnNull method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseNull
 */
TEST(UT__Parser__Null, Dynamic__OnNull__Error)
{
    auto doc = JsonData::FromBuffer(std::string_view{"null"});
    ASSERT_TRUE(doc.has_value());
    ErrorNullParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnNull called."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
