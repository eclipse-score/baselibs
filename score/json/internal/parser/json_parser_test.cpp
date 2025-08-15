/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#include "score/json/json_parser.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <iostream>

namespace score
{
namespace json
{
namespace
{

#define JSON_INPUT R"({ "num": 1, "string": "foo" })"
#define JSON_ERROR_INPUT R"({"something"})"

void json_verify(const score::json::Any& parsed_json)
{

    const auto& obj_result = parsed_json.As<score::json::Object>();
    ASSERT_TRUE(obj_result.has_value());
    const auto& obj = obj_result.value().get();

    const auto num = obj.find("num");
    ASSERT_TRUE(num != obj.end());
    const auto num_value = num->second.As<int>();
    ASSERT_TRUE(num_value.has_value());
    EXPECT_EQ(num_value.value(), 1);

    const auto foo = obj.find("string");
    ASSERT_TRUE(foo != obj.end());
    const auto foo_value = foo->second.As<std::string>();
    ASSERT_TRUE(foo_value.has_value());
    EXPECT_EQ(foo_value.value().get(), "foo");
}

TEST(JsonParserTest, FromBuffer)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Parsing json object using FromBuffer(), cf. RFC-8259 section 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    json::JsonParser json_parser{};
    auto result = json_parser.FromBuffer(JSON_INPUT);
    ASSERT_TRUE(result.has_value());
    json_verify(result.value());
}

TEST(JsonParserTest, ViaLiteral)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Parsing json object using "
                   "_json operator, cf. RFC-8259 section 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto parsed_json = JSON_INPUT ""_json;
    json_verify(parsed_json);
}

TEST(JsonParserTest, ViaErrorLiteral)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Parsing invalid json object using "
                   "_json operator causes failure, cf. RFC-8259 section 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_EXIT(JSON_ERROR_INPUT ""_json, ::testing::KilledBySignal(SIGABRT), "");
}

}  // namespace
}  // namespace json
}  // namespace score
