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
 *        \brief  Contains tests concerning the parsing of arrays.
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

class DynamicArrayParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartArray() noexcept -> ParserResult override
    {
        this->start_ += 1;
        this->balance_ += 1;
        return ParserState::kRunning;
    }

    auto OnEndArray(std::size_t) noexcept -> ParserResult override
    {
        this->end_ += 1;
        if (this->balance_ == 0)
        {
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "EndArray called too often");
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

class LimitedEndArrayParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartArray() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    auto OnEndArray(std::size_t number_of_elems) noexcept -> ParserResult override
    {
        if (number_of_elems >= 2)
        {
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed,
                                                "OnEndArray called with array size >=2.");
        }
        return ParserResult{ParserState::kRunning};
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

class LimitedStartArrayParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnStartArray() noexcept -> ParserResult override
    {
        static std::uint8_t call_cnt{0};
        if (call_cnt > 0)
        {
            return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnStartArray called a second time.");
        }
        call_cnt++;
        return ParserResult{ParserState::kRunning};
    }

    auto OnEndArray(std::size_t) noexcept -> ParserResult override
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
 * Test array start/end counting — integers in array.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Array, Dynamic__Ints)
{
    auto data = JsonData::FromBuffer(std::string_view{"[1,2,3]"});
    ASSERT_TRUE(data.has_value());
    DynamicArrayParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 1);
    EXPECT_EQ(parser.end_, 1);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Test array start/end counting — nested arrays.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Array, Dynamic__Nested)
{
    auto data = JsonData::FromBuffer(std::string_view{"[[1,2],[3,4]]"});
    ASSERT_TRUE(data.has_value());
    DynamicArrayParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 3);
    EXPECT_EQ(parser.end_, 3);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Test array start/end counting — no arrays present.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Array, Dynamic__NoArrays)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"a":1,"b":2})"});
    ASSERT_TRUE(data.has_value());
    DynamicArrayParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.start_, 0);
    EXPECT_EQ(parser.end_, 0);
    EXPECT_EQ(parser.balance_, 0);
}

/*!
 * Tests that when the overridden OnEndArray method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseEndArray
 */
TEST(UT__Parser__Array__Error, Dynamic__OnEndArray)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"([1,2])"});
    ASSERT_TRUE(doc.has_value());
    LimitedEndArrayParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnEndArray called with array size >=2."});
}

/*!
 * Tests that when the overridden OnStartArray method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseStartArray
 */
TEST(UT__Parser__Array__Error, Dynamic__OnStartArray)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"([[]])"});
    ASSERT_TRUE(doc.has_value());
    LimitedStartArrayParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnStartArray called a second time."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score