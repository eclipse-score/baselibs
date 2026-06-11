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
 *        \brief  Contains tests concerning the parsing of keys.
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

using Key = std::string;
using Keys = std::vector<Key>;

struct DynamicKeyParser final : public internal::VirtualParser
{
    using internal::VirtualParser::VirtualParser;

    auto OnKey(StringView key) noexcept -> ParserResult override
    {
        this->keys.push_back(std::string(key));
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Keys keys{};
};

}  // namespace

/*!
 * Test that key callback is called for a single-key object.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Key, Dynamic__SingleKey)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"key":42})"});
    ASSERT_TRUE(data.has_value());
    DynamicKeyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.keys, (Keys{Key("key")}));
}

/*!
 * Test that key callbacks are called for a multi-key object.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Key, Dynamic__MultipleKeys)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"a":1,"b":2,"c":3})"});
    ASSERT_TRUE(data.has_value());
    DynamicKeyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.keys, (Keys{Key("a"), Key("b"), Key("c")}));
}

/*!
 * Test that key callbacks are collected from a nested object.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__Key, Dynamic__NestedKeys)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"outer":{"inner":1}})"});
    ASSERT_TRUE(data.has_value());
    DynamicKeyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.keys, (Keys{Key("outer"), Key("inner")}));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
