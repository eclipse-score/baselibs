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
#include <string_view>

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

TEST(JsonTest, ReadsUnicodeEscapeSequences)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Read a document whose strings use RFC 8259-required escapes and verify their decoded values.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const auto read_json = score::json::JsonParser{}.FromBuffer(R"({"control":"a\u0001z","quote":"\u0022"})");
    ASSERT_TRUE(read_json.has_value());
    const auto& read_object = read_json->As<score::json::Object>().value().get();

    EXPECT_EQ(read_object.at("control").As<std::string>().value().get(), std::string{"a\x01z"});
    EXPECT_EQ(read_object.at("quote").As<std::string>().value().get(), std::string{"\""});
}

TEST(JsonTest, ReadsNonAsciiUnicodeEscapeSequences)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Read a document whose strings use non-ASCII Unicode escapes, including a surrogate pair.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto read_json = score::json::JsonParser{}.FromBuffer(R"({"umlaut":"\u00e4","emoji":"\ud83d\ude00"})");
    ASSERT_TRUE(read_json.has_value());
    const auto& read_object = read_json->As<score::json::Object>().value().get();

    EXPECT_EQ(read_object.at("umlaut").As<std::string>().value().get(), std::string{"\xC3\xA4"});
    EXPECT_EQ(read_object.at("emoji").As<std::string>().value().get(), std::string{"\xF0\x9F\x98\x80"});
}

TEST(JsonTest, RejectsUnescapedControlCharacters)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that documents with an unescaped newline or null character inside a string are rejected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    EXPECT_FALSE(score::json::JsonParser{}.FromBuffer(std::string_view{"{\"value\":\"a\nb\"}"}).has_value());
    EXPECT_FALSE(score::json::JsonParser{}.FromBuffer(std::string_view{"{\"value\":\"a\0b\"}", 15U}).has_value());
}

TEST(JsonTest, ReadWriteEscapedCharacters)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__serialization, comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that a string with every control character, including the null character, and with "
                   "characters the writer escapes, written to a buffer, is read back to an equal value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::string value{};
    for (char control_character{'\x00'}; control_character < '\x20'; ++control_character)
    {
        value.push_back(control_character);
    }
    value += "\"\\/\xC3\xA4\xF0\x9F\x98\x80";
    score::json::Object json{};
    json["value"] = value;
    const score::json::Any expected{std::move(json)};

    const auto buffer{score::json::JsonWriter{}.ToBuffer(expected)};
    ASSERT_TRUE(buffer.has_value());
    const auto read_json{score::json::JsonParser{}.FromBuffer(buffer.value())};

    ASSERT_TRUE(read_json.has_value());
    EXPECT_EQ(read_json.value(), expected);
}
