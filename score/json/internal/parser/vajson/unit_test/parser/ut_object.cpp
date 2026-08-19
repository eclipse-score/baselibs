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
 *        \brief  Contains tests concerning the parsing of objects.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

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

using Count = std::streamsize;

class DynamicObjectParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartObject() noexcept -> ParserResult override
    {
        this->start_ += 1;
        this->balance_ += 1;
        return ParserState::kRunning;
    }

    auto OnEndObject(std::size_t) noexcept -> ParserResult override
    {
        this->end_ += 1;
        if (this->balance_ == 0)
        {
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "EndObject called too often");
        }
        this->balance_ -= 1;
        return ParserResult{ParserState::kRunning};
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Count start_{0};
    Count end_{0};
    Count balance_{0};
};

class EndObjectParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartObject() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnEndObject(std::size_t) noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnEndObject called.");
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

class StartObjectParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartObject() noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnStartObject called.");
    }

    auto OnEndObject(std::size_t) noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace

/*!
 * Test object start/end counting — single object.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Object, Dynamic__SingleObject)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"a":1})"});
    ASSERT_TRUE(data.has_value());
    DynamicObjectParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 1);
    EXPECT_EQ(parser.end_, 1);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Test object start/end counting — nested objects.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Object, Dynamic__NestedObjects)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"a":{"b":1},"c":{"d":2}})"});
    ASSERT_TRUE(data.has_value());
    DynamicObjectParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 3);
    EXPECT_EQ(parser.end_, 3);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Test object start/end counting — no objects (only array).
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Object, Dynamic__NoObjects)
{
    auto data = JsonData::FromBuffer(std::string_view{"[1,2,3]"});
    ASSERT_TRUE(data.has_value());
    DynamicObjectParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 0);
    EXPECT_EQ(parser.end_, 0);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Tests that when the overridden OnEndObject method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseEndObject
 */
TEST(UT__Parser__Object__Error, Dynamic__OnEndObject)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"({})"});
    ASSERT_TRUE(doc.has_value());
    EndObjectParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnEndObject called."});
}

/*!
 * Tests that when the overridden OnStartObject method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseStartObject
 */
TEST(UT__Parser__Object__Error, Dynamic__OnStartObject)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"({})"});
    ASSERT_TRUE(doc.has_value());
    StartObjectParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnStartObject called."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
