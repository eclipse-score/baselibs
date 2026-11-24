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

#include "score/json/json_literals.h"

#include "score/json/object.h"

#include <gtest/gtest.h>

#include <csignal>
#include <string>

namespace score::json::literals
{
namespace
{

TEST(JsonLiteralsTest, JsonLiteralParsesObject)
{
    RecordProperty("Description", "Check that the _json literal parses a valid JSON object.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto parsed_json{R"({ "num": 1, "string": "foo" })"_json};

    const auto object{parsed_json.As<Object>()};
    ASSERT_TRUE(object.has_value());
    const auto& members{object.value().get()};
    ASSERT_EQ(members.size(), 2U);

    const auto num{members.find("num")};
    ASSERT_NE(num, members.end());
    const auto num_value{num->second.As<int>()};
    ASSERT_TRUE(num_value.has_value());
    EXPECT_EQ(num_value.value(), 1);

    const auto text{members.find("string")};
    ASSERT_NE(text, members.end());
    const auto text_value{text->second.As<std::string>()};
    ASSERT_TRUE(text_value.has_value());
    EXPECT_EQ(text_value.value().get(), "foo");
}

TEST(JsonLiteralsDeathTest, JsonLiteralPrintsReasonAndAbortsOnInvalidJson)
{
    RecordProperty("Description",
                   "Check that the _json literal prints the parsing error reason and aborts on invalid JSON.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_EXIT(R"({"a": 1 "b": 2})"_json, ::testing::KilledBySignal(SIGABRT), "Missing comma");
}

}  // namespace
}  // namespace score::json::literals
