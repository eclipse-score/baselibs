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
 *        \brief  Contains tests concerning the parsing of strings.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
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

class DynamicStringParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnString(StringView str) noexcept -> ParserResult override
    {
        this->strings.push_back(std::string(str));
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Strings strings;
};

class ErrorStringParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnString(StringView) noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnString called.");
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace

/*!
 * Test that string callbacks are called for a string array.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__String, Dynamic__StringArray)
{
    auto data = JsonData::FromBuffer(std::string_view{R"(["hello","world"])"});
    ASSERT_TRUE(data.has_value());
    DynamicStringParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.strings, (Strings{"hello", "world"}));
}

/*!
 * Tests that when the overridden OnString method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseUnescapedString
 */
TEST(UT__Parser__String, Dynamic__ParseUnescapedString__Error)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"("SomeString")"});
    ASSERT_TRUE(doc.has_value());
    ErrorStringParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnString called."});
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
