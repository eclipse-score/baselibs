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
 *        \brief  Contains tests concerning the parsing of valid JSON.
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

class DynamicAnyParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

}  // namespace

/*!
 * Test that vaJson can parse a simple integer value.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseInt)
{
    auto data = JsonData::FromBuffer(std::string_view{"42"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that vaJson can parse a simple boolean value.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseBool)
{
    auto data = JsonData::FromBuffer(std::string_view{"true"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that vaJson can parse a null value.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseNull)
{
    auto data = JsonData::FromBuffer(std::string_view{"null"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that vaJson can parse a JSON object.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseObject)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"key": 42, "flag": true, "name": "test"})"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that vaJson can parse a JSON array.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseArray)
{
    auto data = JsonData::FromBuffer(std::string_view{R"([1, 2, 3, null, true, false, "str"])"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

/*!
 * Test that vaJson can parse a nested JSON structure.
 * \trace           CREQ-Json-Deserialization
 */
TEST(UT__Parser__Valid, Dynamic__ParseNested)
{
    auto data = JsonData::FromBuffer(std::string_view{R"({"a": [1, {"b": 2}], "c": null})"});
    ASSERT_TRUE(data.has_value());
    DynamicAnyParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
