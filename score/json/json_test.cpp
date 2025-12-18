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
#include "gtest/gtest.h"
#include "score/json/json_parser.h"
#include "score/json/json_writer.h"

TEST(JsonTest, ReadWrite)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Create Json object with different attributes, write it to a buffer, then read it successfully.");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    score::json::Object json{};
    json["string"] = std::string{"foo"};
    json["num"] = 5;

    score::json::Object nested_object{};
    nested_object["num2"] = std::uint8_t{4};

    score::json::List list{};
    list.emplace_back(score::json::Null{});
    list.emplace_back(std::move(nested_object));

    json["list"] = std::move(list);

    score::json::JsonParser parser{};
    score::json::JsonWriter writer{};
    auto buffer = writer.ToBuffer(json);
    auto read_json = parser.FromBuffer(*buffer);
    auto& read_object = (*read_json).As<score::json::Object>().value().get();

    EXPECT_NE(read_object.find("string"), read_object.end());
    EXPECT_EQ(read_object["string"].As<std::string>().value().get(), std::string{"foo"});
    EXPECT_NE(read_object.find("num"), read_object.end());
    EXPECT_EQ(read_object["num"].As<std::uint8_t>().value(), 5);
    EXPECT_NE(read_object.find("list"), read_object.end());
    EXPECT_EQ(read_object["list"].As<score::json::List>().value().get().size(), 2);
    EXPECT_EQ(read_object["list"].As<score::json::List>().value().get()[0].As<score::json::Null>().value().get(),
              score::json::Null{});
    EXPECT_NE(
        read_object["list"].As<score::json::List>().value().get()[1].As<score::json::Object>().value().get().find("num2"),
        read_object["list"].As<score::json::List>().value().get()[1].As<score::json::Object>().value().get().end());
    EXPECT_EQ(read_object["list"]
                  .As<score::json::List>()
                  .value()
                  .get()[1]
                  .As<score::json::Object>()
                  .value()
                  .get()["num2"]
                  .As<std::uint8_t>()
                  .value(),
              4);
}
