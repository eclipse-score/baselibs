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

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace score
{
namespace json
{
namespace
{

constexpr std::string_view kJsonInput{R"({ "num": 1, "string": "foo" })"};

constexpr std::size_t kMaxNestingDepth{64U};

std::string MakeNestedArrays(const std::size_t depth)
{
    return std::string(depth, '[') + "0" + std::string(depth, ']');
}

std::string MakeNestedObjects(const std::size_t depth)
{
    std::string input{};
    for (std::size_t level{0U}; level < depth; ++level)
    {
        input += R"({"value":)";
    }
    input += "0";
    input.append(depth, '}');
    return input;
}

void VerifyJson(const score::json::Any& parsed_json)
{

    const auto& obj_result{parsed_json.As<score::json::Object>()};
    ASSERT_TRUE(obj_result.has_value());
    const auto& obj{obj_result.value().get()};

    const auto num{obj.find("num")};
    ASSERT_TRUE(num != obj.end());
    const auto num_value{num->second.As<int>()};
    ASSERT_TRUE(num_value.has_value());
    EXPECT_EQ(num_value.value(), 1);

    const auto foo{obj.find("string")};
    ASSERT_TRUE(foo != obj.end());
    const auto foo_value{foo->second.As<std::string>()};
    ASSERT_TRUE(foo_value.has_value());
    EXPECT_EQ(foo_value.value().get(), "foo");
}

TEST(JsonParserTest, FromBuffer)
{
    RecordProperty("Verifies", "::score::json::JsonParser::FromBuffer");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Parsing json object using FromBuffer(), cf. RFC-8259 section 9");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    RecordProperty("Priority", "3");

    json::JsonParser json_parser{};
    auto result{json_parser.FromBuffer(kJsonInput)};
    ASSERT_TRUE(result.has_value());
    VerifyJson(result.value());
}

TEST(JsonParserTest, EnforcesMaximumNestingDepth)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth, comp_req__json__nesting_depth_inclusive");
    RecordProperty("Description", "Check that the container depth limit is inclusive for arrays and objects.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    const JsonParser parser{};
    for (const auto make_input : {MakeNestedArrays, MakeNestedObjects})
    {
        for (const auto depth : {std::size_t{0U}, std::size_t{1U}, kMaxNestingDepth - 1U, kMaxNestingDepth})
        {
            EXPECT_TRUE(parser.FromBuffer(make_input(depth)).has_value()) << depth;
        }
        for (const auto depth : {kMaxNestingDepth + 1U, kMaxNestingDepth + 2U})
        {
            const auto result{parser.FromBuffer(make_input(depth))};
            ASSERT_FALSE(result.has_value()) << depth;
            EXPECT_EQ(result.error(), Error::kParsingError);
        }
    }
}

TEST(JsonParserTest, CountsOnlyOpenContainers)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth_counting");
    RecordProperty("Description",
                   "Check that sibling containers, keys and string delimiters do not consume extra depth.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // The deepest container has depth kMaxNestingDepth; its siblings and the delimiters inside strings add none.
    const auto input{std::string(kMaxNestingDepth - 2U, '[') + R"([0],[1],{"[{}]":[]},{"key":"[{}]"})" +
                     std::string(kMaxNestingDepth - 2U, ']')};
    EXPECT_TRUE(JsonParser{}.FromBuffer(input).has_value());
}

TEST(JsonParserTest, ReusesNestingBudgetAfterErrors)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth_independence, comp_req__json__deserialization");
    RecordProperty("Description", "Check that depth and syntax errors leave the parser reusable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const JsonParser parser{};
    const auto accepted{MakeNestedArrays(kMaxNestingDepth)};
    for (const auto& input : {MakeNestedArrays(kMaxNestingDepth + 1U),
                              std::string(kMaxNestingDepth, '[') + "0",
                              std::string(kMaxNestingDepth + 1U, '['),
                              std::string{"[{"},
                              std::string{R"([{"value":[)"}})
    {
        const auto result{parser.FromBuffer(input)};
        ASSERT_FALSE(result.has_value()) << input;
        EXPECT_EQ(result.error(), Error::kParsingError);
        EXPECT_TRUE(parser.FromBuffer(accepted).has_value());
    }
}

TEST(JsonParserTest, RejectsDeepInputWithoutRetainingState)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth, comp_req__json__nesting_depth_independence");
    RecordProperty("Description", "Check normal rejection and reuse for deeply nested valid arrays and objects.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    constexpr std::size_t kInputDepth{20000U};
    const JsonParser parser{};
    for (const auto& input : {MakeNestedArrays(kInputDepth), MakeNestedObjects(kInputDepth)})
    {
        const auto result{parser.FromBuffer(input)};
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), Error::kParsingError);
        EXPECT_TRUE(parser.FromBuffer("[]").has_value());
    }
}

TEST(JsonParserTest, LiteralUsesMaximumNestingDepth)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth_literal");
    RecordProperty("Description", "Check that the literal accepts the maximum depth and terminates above it.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    const auto accepted{MakeNestedArrays(kMaxNestingDepth)};
    EXPECT_TRUE(operator""_json(accepted.data(), accepted.size()).As<List>().has_value());
    const auto rejected{MakeNestedArrays(kMaxNestingDepth + 1U)};
    EXPECT_DEATH(static_cast<void>(operator""_json(rejected.data(), rejected.size())), "JSON could not be parsed!");
}

TEST(JsonParserTest, ViaLiteral)
{
    RecordProperty("Verifies", "::score::json::operator\"\"_json");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Parsing json object using "
                   "_json operator, cf. RFC-8259 section 9");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    RecordProperty("Priority", "3");

    // Same JSON as kJsonInput; the _json operator only accepts a string literal.
    auto parsed_json{R"({ "num": 1, "string": "foo" })"_json};
    VerifyJson(parsed_json);
}

TEST(JsonParserTest, ViaErrorLiteral)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Parsing invalid json object using _json operator, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");  // requirements test
    RecordProperty("DerivationTechnique", "error-guessing");
    RecordProperty("Priority", "3");

    EXPECT_EXIT(R"({"something"})"_json, ::testing::KilledBySignal(SIGABRT), "");
}

TEST(JsonParserTest, FromFileSuceess)
{
    RecordProperty("Verifies", "::score::json::JsonParser::FromFile");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Parsing json object from file path, cf. RFC-8259 section 9");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");  // boundary values
    RecordProperty("Priority", "3");

    const std::string file_path{std::tmpnam(nullptr)};
    std::ofstream file{file_path};
    if (file.is_open())
    {
        file << kJsonInput;
        file.close();
    }

    json::JsonParser json_parser{};
    auto result{json_parser.FromFile(file_path)};
    ASSERT_TRUE(result.has_value());
    VerifyJson(result.value());
}

TEST(JsonParserTest, FromFileParseError)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Parsing invalid json object from file path causes failure, cf. RFC-8259 section 9");
    RecordProperty("TestType", "requirements-based");  // requirements test
    RecordProperty("DerivationTechnique", "error-guessing");
    RecordProperty("Priority", "3");

    const std::string file_path{std::tmpnam(nullptr)};
    std::ofstream file{file_path};
    if (file.is_open())
    {
        file << kJsonInput;
        file << "!";  // append garbage
        file.close();
    }

    json::JsonParser json_parser{};
    auto result{json_parser.FromFile(file_path)};
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::json::Error::kParsingError);
}

template <typename T, std::enable_if_t<!std::is_arithmetic<T>::value, bool> = true>
const T& GetValueOfObject(const score::json::Any& any, const std::string& key)
{
    return any.As<score::json::Object>().value().get().at(key).As<T>().value().get();
}

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
T GetValueOfObject(const score::json::Any& any, const std::string& key)
{
    return any.As<score::json::Object>().value().get().at(key).As<T>().value();
}

constexpr std::string_view kSimpleJsonBuffer{R"(
{
  "boolean": true,
  "color": "gold",
  "null": null,
  "number": 123,
  "number_u16": 65535,
  "number_u32": 4294967295,
  "number_u64": 18446744073709551615,
  "number_i": -123,
  "number_i16": -32768,
  "number_i32": -2147483648,
  "number_i64": -922337203685477580,
  "float": 2.3,
  "double": 1.79769e+308,
  "object": {
    "a": "b",
    "c": "d"
  },
  "list": [
    "first",
    2,
    "third"
  ]
}
)"};

class ParserTest : public testing::Test
{
  protected:
    std::string CreateJsonFile(const std::string_view filename, const std::string_view json_string)
    {
        const auto file_path{::testing::TempDir() + std::string{filename}};
        std::ofstream ofstream{file_path};
        ofstream << json_string.data();
        ofstream.close();
        return file_path;
    }
};

TEST_F(ParserTest, AppliesMaximumNestingDepthToFiles)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__nesting_depth_file");
    RecordProperty("Description", "Check that files and buffers share the inclusive array and object depth limit.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    const JsonParser parser{};
    for (const auto& input : {MakeNestedArrays(kMaxNestingDepth),
                              MakeNestedObjects(kMaxNestingDepth),
                              MakeNestedArrays(kMaxNestingDepth + 1U),
                              MakeNestedObjects(kMaxNestingDepth + 1U)})
    {
        const auto file_path{CreateJsonFile("maximum_depth.json", input)};
        const auto from_buffer{parser.FromBuffer(input)};
        const auto from_file{parser.FromFile(file_path)};
        ASSERT_EQ(from_file.has_value(), from_buffer.has_value()) << input;
        if (from_buffer.has_value())
        {
            EXPECT_EQ(from_file.value(), from_buffer.value());
        }
        else
        {
            EXPECT_EQ(from_file.error(), Error::kParsingError);
        }
    }
    EXPECT_TRUE(parser.FromFile(CreateJsonFile("maximum_depth.json", "[]")).has_value());
}

TEST_F(ParserTest, CanParseObjectBool)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a boolean value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as bool
    auto value{GetValueOfObject<bool>(root.value(), "boolean")};

    // Then the correct value is returned
    EXPECT_EQ(value, true);
}

TEST_F(ParserTest, CanParseObjectString)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a string value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as std::string
    auto& value{GetValueOfObject<std::string>(root.value(), "color")};

    // Then the correct value is returned
    EXPECT_EQ(value, "gold");
}

TEST_F(ParserTest, CanParseObjectNull)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a null value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as Null
    auto& value{GetValueOfObject<Null>(root.value(), "null")};

    // Then the correct value is returned
    EXPECT_EQ(value, Null{});
}

TEST_F(ParserTest, CanParseObjectNumber)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a uint64_t value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as number
    auto value{GetValueOfObject<std::uint64_t>(root.value(), "number")};

    // Then the correct value is returned
    EXPECT_EQ(value, 123);
}

TEST_F(ParserTest, CanParseObjectFloatingPointNumber)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a float value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as floating point number
    auto float_value{GetValueOfObject<float>(root.value(), "float")};
    double double_value{GetValueOfObject<double>(root.value(), "double")};
    auto double_as_float_value{root->As<score::json::Object>().value().get().at("double").As<float>().has_value()};

    // Then the correct value is returned
    EXPECT_FLOAT_EQ(float_value, 2.3F);
    EXPECT_DOUBLE_EQ(double_value, 1.79769e+308);
    EXPECT_FALSE(double_as_float_value);
}

TEST_F(ParserTest, CanParseObjectInObject)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a nested object from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as number
    auto& value{GetValueOfObject<Object>(root.value(), "object")};

    // Then the correct value is returned
    EXPECT_EQ(value.at("a").As<std::string>().value().get(), "b");
}

TEST_F(ParserTest, CanParseListInObject)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a list from a JSON buffer returns the correct values in order.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(kSimpleJsonBuffer)};

    // When reading a key of an object that is interpreted as number
    auto& value{GetValueOfObject<List>(*root, "list")};

    // Then the correct value is returned
    EXPECT_EQ(value[0].As<std::string>().value().get(), "first");
    EXPECT_EQ(value[1].As<std::uint64_t>().value(), 2UL);
    EXPECT_EQ(value[2].As<std::string>().value().get(), "third");
}

TEST_F(ParserTest, CanParseObjectInObjectAndIterateOverKeys)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing and iterating over an object whose keys are unknown to the program returns all entries.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a JSON structure that contains objects in objects where the keys are unknown to the program
    std::string buffer{R"(
{
   "file_format_version":"1.0",
   "storage_list":{
      "cdc/cdc_RootSwc/PersistencyConfigPPort":{
         "is_installed":true,
         "path":"\/opt\/cdc\/etc\/config\/key_value_storage",
         "is_crc_protected":false,
         "max_redundancy_files":1,
         "sw_cluster_version":"1.0.0",
         "executable_version":"1.0.0"
      },
      "cdc/cdc_RootSwc/PersistencyRegistryPPort":{
         "is_installed":true,
         "path":"\/persistent\/cdc\/nvmblock\/key_value_storage",
         "is_crc_protected":false,
         "max_redundancy_files":1,
         "sw_cluster_version":"1.0.0",
         "executable_version":"1.0.0"
      }
   }
}
)"};
    auto root{JsonParser{}.FromBuffer(buffer)};

    // When iterating over the unknown keys
    const auto& storage_list{root.value().As<Object>().value().get()["storage_list"]};
    std::vector<std::string> collected_paths{};
    for (const auto& element : storage_list.As<Object>().value().get())
    {
        collected_paths.push_back(element.second.As<Object>().value().get().at("path").As<std::string>().value().get());
    }

    // Then we can store them and thus also access them
    std::sort(collected_paths.begin(), collected_paths.end());
    EXPECT_EQ(collected_paths.at(0), "/opt/cdc/etc/config/key_value_storage");
    EXPECT_EQ(collected_paths.at(1), "/persistent/cdc/nvmblock/key_value_storage");
}

TEST_F(ParserTest, EmitsErrorWhenParsingObjectWithValueButNoKey)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing an object value without a preceding key is rejected as malformed.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    std::string buffer{R"(
{
false
}
)"};

    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = JsonParser{}.FromBuffer(buffer)));
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TEST_F(ParserTest, EmitsErrorWhenParsingObjectWithBinaryValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing an object with a non-UTF-8 binary value element is rejected as malformed.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    // Binary content - format specific to vaJSON Parser
    // First 4 bytes represent length of binary content in std::uint64_t
    // Following 8 bytes represent binary content
    // Hence, this is a binary content of one std::uint64_t with all ones
    std::uint8_t binary[12]{0, 0, 0, 1, 255, 255, 255, 255, 255, 255, 255, 255};

    // Stitch the JSON object together
    std::string buffer{R"(
{
  "binary": b)"};
    buffer += std::string{std::begin(binary), std::end(binary)};
    buffer += R"(
}
)";

    // When trying to read this JSON object there is no fatal failure or exception and...
    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = JsonParser{}.FromBuffer(buffer)));

    // We get an error returned that specifies that parsing failed
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TEST_F(ParserTest, EmitsErrorWhenParsingTooLargeNumber)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a number too large to be represented is rejected as malformed.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    std::string buffer{R"({ "number": 1e+500 })"};

    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = JsonParser{}.FromBuffer(buffer)));

    // We get an error returned that specifies that parsing failed
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TEST_F(ParserTest, FromFileFail)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing from a non-existent file path returns an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");
    // Pass invalid file path
    auto foo{JsonParser{}.FromFile("foo")};

    EXPECT_FALSE(foo.has_value());
}

TEST_F(ParserTest, ParsingFromFileWorks)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a JSON buffer written to a file returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a simple JSON File
    std::string file_name{"test.json"};
    auto root{JsonParser{}.FromFile(CreateJsonFile(file_name, kSimpleJsonBuffer))};

    // Then json file is successfully parsed
    EXPECT_TRUE(root.has_value());

    // When reading a key of an object that is interpreted as bool
    auto value{GetValueOfObject<bool>(root.value(), "boolean")};

    // Then the correct value is returned
    EXPECT_EQ(value, true);
}

struct DuplicateMemberNameParam
{
    std::string_view name;
    std::string_view json;
};

// Found by gtest via ADL to print GetParam() in failure messages.
void PrintTo(const DuplicateMemberNameParam& param, std::ostream* os)
{
    *os << param.json;
}

class DuplicateMemberNameTest : public testing::TestWithParam<DuplicateMemberNameParam>
{
};

TEST_P(DuplicateMemberNameTest, EmitsErrorWhenParsingObjectWithDuplicateMemberName)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__duplicate_member_names");
    RecordProperty("Description",
                   "Check that parsing an object with two members of the same name returns a parsing error, for "
                   "each combination of member value types and object positions.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto root{JsonParser{}.FromBuffer(GetParam().json)};

    ASSERT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

INSTANTIATE_TEST_SUITE_P(ParserTest,
                         DuplicateMemberNameTest,
                         testing::Values(DuplicateMemberNameParam{"Null", R"({"a": null, "a": null})"},
                                         DuplicateMemberNameParam{"Bool", R"({"a": true, "a": false})"},
                                         DuplicateMemberNameParam{"Number", R"({"a": 1, "a": 2})"},
                                         DuplicateMemberNameParam{"String", R"({"a": "x", "a": "y"})"},
                                         DuplicateMemberNameParam{"Object", R"({"a": {"b": 1}, "a": {"c": 2}})"},
                                         DuplicateMemberNameParam{"List", R"({"a": [1], "a": [2]})"},
                                         DuplicateMemberNameParam{"ScalarThenObject", R"({"a": 1, "a": {"b": 1}})"},
                                         DuplicateMemberNameParam{"ScalarThenList", R"({"a": 1, "a": [1]})"},
                                         DuplicateMemberNameParam{"ObjectThenScalar", R"({"a": {"b": 1}, "a": 1})"},
                                         DuplicateMemberNameParam{"ListThenObject", R"({"a": [1], "a": {"b": 1}})"},
                                         DuplicateMemberNameParam{"NotAdjacent", R"({"a": 1, "b": 2, "a": 3})"},
                                         DuplicateMemberNameParam{"InNestedObject", R"({"o": {"a": 1, "a": 2}})"},
                                         DuplicateMemberNameParam{"InObjectInList", R"([{"a": 1, "a": 2}])"}),
                         [](const testing::TestParamInfo<DuplicateMemberNameParam>& param_info) {
                             return std::string{param_info.param.name};
                         });

TEST_F(ParserTest, EmitsErrorWhenParsingObjectWithDuplicateEscapedMemberName)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__duplicate_member_names");
    RecordProperty("Description",
                   "Check that two member names that differ only in escaping are treated as duplicates and return a "
                   "parsing error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");

    const auto root{JsonParser{}.FromBuffer(R"({"a/b": 1, "a\/b": 2})")};

    ASSERT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TEST_F(ParserTest, CanParseSameMemberNameInDifferentObjects)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__duplicate_member_names");
    RecordProperty("Description",
                   "Check that the same member name in different objects, and names differing only in case, are "
                   "accepted.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const auto root{JsonParser{}.FromBuffer(R"({"a": {"a": 1}, "b": [{"a": 2}, {"a": 3}], "A": 4})")};

    ASSERT_TRUE(root.has_value());
    EXPECT_EQ(GetValueOfObject<Object>(root.value(), "a").at("a").As<std::uint64_t>().value(), 1U);
    const auto& list{GetValueOfObject<List>(root.value(), "b")};
    EXPECT_EQ(list.at(0).As<Object>().value().get().at("a").As<std::uint64_t>().value(), 2U);
    EXPECT_EQ(list.at(1).As<Object>().value().get().at("a").As<std::uint64_t>().value(), 3U);
    EXPECT_EQ(GetValueOfObject<std::uint64_t>(root.value(), "A"), 4U);
}

TEST(JsonParserVajsonTest, CanParseObjectHexadecimalNumber)
{
    // vaJson accepts hexadecimal number syntax as a lenient extension beyond the RFC 8259 well-formedness
    // that comp_req__json__deserialization mandates, so this documents the parser's actual behavior rather than
    // normative text.
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing a hexadecimal number, an accepted extension beyond RFC-8259, returns "
                   "the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "design-analysis");

    std::string buffer_json_hex{R"(
        {
          "boolean": true,
          "color": "gold",
          "null": null,
          "number": 123,
          "number_u16": 65535,
          "number_u32": 4294967295,
          "number_u64": 18446744073709551615,
          "number_i": -123,
          "number_i16": -32768,
          "number_i32": -2147483648,
          "number_i64": -922337203685477580,
          "float": 2.3,
          "double": 1.79769e+308,
          "hex" : 0xdd,
          "object": {
            "a": "b",
            "c": "d"
          },
          "list": [
            "first",
            2,
            "third"
          ]
        }
        )"};

    // Given a simple JSON buffer
    auto root{JsonParser{}.FromBuffer(buffer_json_hex)};

    // When reading a key of an object that is interpreted as floating point number
    auto value{GetValueOfObject<std::uint64_t>(root.value(), "hex")};

    // Then the correct value is returned
    EXPECT_EQ(value, 0xdd);
}

TEST(JsonParserVajsonTest, CanParseTwoByteBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a two-byte JSON object, shorter than the 3-byte UTF-8 BOM, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a valid JSON buffer of exactly 2 bytes (shorter than BOM)
    auto root{JsonParser{}.FromBuffer("{}")};

    // Then parsing succeeds and returns an empty object
    ASSERT_TRUE(root.has_value());
    auto obj{root->As<Object>()};
    ASSERT_TRUE(obj.has_value());
    EXPECT_TRUE(obj->get().empty());
}

TEST(JsonParserVajsonTest, CanParseBufferWithUtf8Bom)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing a JSON buffer prefixed with a UTF-8 BOM consumes the BOM correctly.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a JSON buffer with a UTF-8 BOM prefix
    std::string bom_json{"\xEF\xBB\xBF{\"key\": 123}"};
    auto root{JsonParser{}.FromBuffer(bom_json)};

    // Then parsing succeeds and the value is correct
    ASSERT_TRUE(root.has_value());
    auto value{GetValueOfObject<std::uint64_t>(root.value(), "key")};
    EXPECT_EQ(value, 123U);
}

constexpr std::string_view kNumberLimitsJsonBuffer{R"(
{
    "bool_min":false,
    "bool_max":true,
    "uint8_min":0,
    "uint8_max":255,
    "uint16_min":0,
    "uint16_max":65535,
    "uint32_min":0,
    "uint32_max":4294967295,
    "uint64_min":0,
    "uint64_max":18446744073709551615,
    "int8_min":-128,
    "int8_max":127,
    "int16_min":-32768,
    "int16_max":32767,
    "int32_min":-2147483648,
    "int32_max":2147483647,
    "int64_min":-9223372036854775808,
    "int64_max":9223372036854775807,
    "float_min": 1.175494350822287507969e-38,
    "float_max": 3.402823466385288598117e+38,
    "double_min":2.225073858507201383090e-308,
    "double_max":1.797693134862315708145e+308
    }
)"};

template <typename T, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
score::Result<T> GetNumberOfObject(const score::json::Any& any, const std::string& key)
{
    return any.As<score::json::Object>().value().get().at(key).As<T>();
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
void CheckReturnedValues(score::Result<T> min, score::Result<T> max)
{
    EXPECT_TRUE(min.has_value());
    EXPECT_EQ(min.value(), std::numeric_limits<T>::min());
    EXPECT_TRUE(max.has_value());
    EXPECT_EQ(max, std::numeric_limits<T>::max());
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
void CheckReturnedValues(score::Result<T> min, score::Result<T> max)
{
    EXPECT_TRUE(min.has_value());
    auto diff{std::abs(min.value() - std::numeric_limits<T>::min())};
    EXPECT_TRUE(diff < std::numeric_limits<T>::epsilon());

    EXPECT_TRUE(max.has_value());
    diff = std::abs(max.value() - std::numeric_limits<T>::max());
    EXPECT_TRUE(diff < std::numeric_limits<T>::epsilon());
}

TEST(NumberTest, WhenParsingABoolStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing the JSON boolean values false and true returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<bool>(root.value(), "bool_min")};
    auto max{GetNumberOfObject<bool>(root.value(), "bool_max")};

    // Then the correct values are returned
    CheckReturnedValues<bool>(min, max);
}

TEST(NumberTest, WhenParsingAUint8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint8_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as std::uint8_t
    auto min{GetNumberOfObject<std::uint8_t>(root.value(), "uint8_min")};
    auto max{GetNumberOfObject<std::uint8_t>(root.value(), "uint8_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::uint8_t>(min, max);
}

TEST(NumberTest, WhenParsingAUint16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint16_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::uint16_t>(root.value(), "uint16_min")};
    auto max{GetNumberOfObject<std::uint16_t>(root.value(), "uint16_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::uint16_t>(min, max);
}

TEST(NumberTest, WhenParsingAUint32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint32_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::uint32_t>(root.value(), "uint32_min")};
    auto max{GetNumberOfObject<std::uint32_t>(root.value(), "uint32_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::uint32_t>(min, max);
}

TEST(NumberTest, WhenParsingAUint64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the uint64_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::uint64_t>(root.value(), "uint64_min")};
    auto max{GetNumberOfObject<std::uint64_t>(root.value(), "uint64_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::uint64_t>(min, max);
}

TEST(NumberTest, WhenParsingAInt8StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int8_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::int8_t>(root.value(), "int8_min")};
    auto max{GetNumberOfObject<std::int8_t>(root.value(), "int8_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::int8_t>(min, max);
}

TEST(NumberTest, WhenParsingAInt16StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int16_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::int16_t>(root.value(), "int16_min")};
    auto max{GetNumberOfObject<std::int16_t>(root.value(), "int16_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::int16_t>(min, max);
}

TEST(NumberTest, WhenParsingAInt32StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int32_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::int32_t>(root.value(), "int32_min")};
    auto max{GetNumberOfObject<std::int32_t>(root.value(), "int32_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::int32_t>(min, max);
}

TEST(NumberTest, WhenParsingAInt64StoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty(
        "Description",
        "Check that parsing a JSON number at the int64_t limits, cf. RFC-8259 section 9, returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as bool
    auto min{GetNumberOfObject<std::int64_t>(root.value(), "int64_min")};
    auto max{GetNumberOfObject<std::int64_t>(root.value(), "int64_max")};

    // Then the correct values are returned
    CheckReturnedValues<std::int64_t>(min, max);
}
TEST(NumberTest, WhenParsingAFloatStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing a JSON number at the float limits, cf. RFC-8259 section 9, returns the "
                   "correct value within its precision.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as double
    auto min{GetNumberOfObject<double>(root.value(), "float_min")};
    auto max{GetNumberOfObject<double>(root.value(), "float_max")};

    // Then the correct values are returned
    CheckReturnedValues<float>(min, max);
}

TEST(NumberTest, WhenParsingADoubleStoredInAJsonThenParsingIsSuccessfulAndCorrectValueIsReturned)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description",
                   "Check that parsing a JSON number at the double limits, cf. RFC-8259 section 9, returns the "
                   "correct value within its precision.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    // Given a simple JSON buffer and parsing it
    auto root{JsonParser{}.FromBuffer(kNumberLimitsJsonBuffer)};

    // Then parsing is successful
    EXPECT_TRUE(root.has_value());

    // When reading key values of an object that is interpreted as double
    auto min{GetNumberOfObject<double>(root.value(), "double_min")};
    auto max{GetNumberOfObject<double>(root.value(), "double_max")};

    // Then the correct values are returned
    CheckReturnedValues<double>(min, max);
}

}  // namespace
}  // namespace json
}  // namespace score
