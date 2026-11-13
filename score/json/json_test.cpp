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

#include <cstddef>
#include <string>

TEST(JsonTest, NestedTreeAtMaximumDepthSurvivesRoundTrip)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth_inclusive, comp_req__json__serialization");
    RecordProperty("Description",
                   "Check cloning, equality and both writer formats for a mixed tree at the maximum depth.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    constexpr std::size_t kMaxNestingDepth{64U};
    std::string input{"0"};
    for (std::size_t depth{0U}; depth < kMaxNestingDepth; ++depth)
    {
        const bool is_array{depth % 2U == 0U};
        input.insert(0U, is_array ? "[" : R"({"value":)");
        input.push_back(is_array ? ']' : '}');
    }
    const score::json::JsonParser parser{};
    const auto root{parser.FromBuffer(input)};
    ASSERT_TRUE(root.has_value());
    const auto cloned{root.value().CloneByValue()};
    EXPECT_EQ(root.value(), cloned);

    for (const auto formatting : {score::json::Formatting::kCompact, score::json::Formatting::kPrettyPrint})
    {
        score::json::JsonWriter writer{score::json::FileSyncMode::kUnsynced,
                                       score::filesystem::kUseTargetFileUID | score::filesystem::kUseTargetFileGID,
                                       formatting};
        const auto buffer{writer.ToBuffer(cloned)};
        ASSERT_TRUE(buffer.has_value());
        const auto reparsed{parser.FromBuffer(buffer.value())};
        ASSERT_TRUE(reparsed.has_value());
        EXPECT_EQ(reparsed.value(), root.value());
    }
}

TEST(JsonTest, ReadWrite)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Create Json object with different attributes, write it to a buffer, then read it successfully.");
    RecordProperty("TestType", "requirements-based");                // requirements test
    RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

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
        read_object["list"].As<score::json::List>().value().get()[1].As<score::json::Object>().value().get().find(
            "num2"),
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

TEST(JsonTest, ReadWriteEmptyMemberName)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__serialization, comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that an object with empty member names at top level and nested, written to a buffer, is "
                   "read back to an equal value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    score::json::Object nested_object{};
    nested_object[""] = std::string{"x"};
    score::json::Object json{};
    json[""] = std::move(nested_object);
    json["a"] = true;
    const score::json::Any expected{std::move(json)};

    const auto buffer{score::json::JsonWriter{}.ToBuffer(expected)};
    ASSERT_TRUE(buffer.has_value());
    const auto read_json{score::json::JsonParser{}.FromBuffer(buffer.value())};

    ASSERT_TRUE(read_json.has_value());
    EXPECT_EQ(read_json.value(), expected);
}
