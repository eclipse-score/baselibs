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
 *        \brief  Contains tests concerning the parsing of numbers.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/number_parser.h"
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

using Count = std::size_t;

class DynamicNumberParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnNumber(JsonNumber) noexcept -> ParserResult override
    {
        this->number_ += 1;
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Count number_{0};
};

}  // namespace

/*!
 * Test that numbers are counted — integer array.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Number, Dynamic__IntegerArray)
{
    auto data = JsonData::FromBuffer(std::string_view{"[1,2,3,4,5]"});
    ASSERT_TRUE(data.has_value());
    DynamicNumberParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.number_, 5U);
}

/*!
 * Test that numbers are counted — float array.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Number, Dynamic__FloatArray)
{
    auto data = JsonData::FromBuffer(std::string_view{"[1.0,-1.5,3.14]"});
    ASSERT_TRUE(data.has_value());
    DynamicNumberParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.number_, 3U);
}

/*!
 * Tests that when OnNumber is called with a number not convertible to the NumberParser's number type,
 * that error is propagated to the Parse() call.
 * \trace           score::json::vajson::internal::NumberParser::OnNumber
 */
TEST(UT__Parser__Number, TryAs__Error)
{
    auto json_data_result = JsonData::FromBuffer(std::string_view{"-123"});
    ASSERT_TRUE(json_data_result.has_value());
    internal::NumberParser<std::uint8_t> parser{json_data_result.value(),
                                                [](std::uint8_t) noexcept -> score::Result<void> {
                                                    return score::Result<void>{};
                                                }};

    const auto result = parser.Parse();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
    ASSERT_EQ(result.error().UserMessage(), std::string_view{"Could not convert number."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
