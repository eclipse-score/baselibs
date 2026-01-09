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
#ifndef SCORE_LIB_JSON_INTERNAL_PARSER_PARSER_TEST_SUITE_H
#define SCORE_LIB_JSON_INTERNAL_PARSER_PARSER_TEST_SUITE_H

#include "score/json/internal/model/any.h"

#include "gtest/gtest.h"

#include <fstream>
#include <string>

namespace score
{
namespace json
{
namespace
{

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

std::string buffer_simple_json{R"(
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

template <typename T>
class ParserTest : public testing::Test
{
  protected:
    std::string CreateJsonFile(const std::string_view filename, const std::string_view json_string)
    {
        const auto file_path = ::testing::TempDir() + std::string{filename};
        std::ofstream ofstream{file_path};
        ofstream << json_string.data();
        ofstream.close();
        return file_path;
    }
};
TYPED_TEST_SUITE_P(ParserTest);

TYPED_TEST_P(ParserTest, CanParseObjectBool)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse boolean value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as bool
    auto value = GetValueOfObject<bool>(root.value(), "boolean");

    // Then the correct value is returned
    EXPECT_EQ(value, true);
}

TYPED_TEST_P(ParserTest, CanParseObjectString)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse string value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as std::string
    auto& value = GetValueOfObject<std::string>(root.value(), "color");

    // Then the correct value is returned
    EXPECT_EQ(value, "gold");
}

TYPED_TEST_P(ParserTest, CanParseObjectNull)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse Null value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as Null
    auto& value = GetValueOfObject<Null>(root.value(), "null");

    // Then the correct value is returned
    EXPECT_EQ(value, Null{});
}

TYPED_TEST_P(ParserTest, CanParseObjectNumber)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse uint64_t value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "number");

    // Then the correct value is returned
    EXPECT_EQ(value, 123);
}

TYPED_TEST_P(ParserTest, CanParseObjectFloatingPointNumber)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse float value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as floating point number
    auto float_value = GetValueOfObject<float>(root.value(), "float");
    double double_value = GetValueOfObject<double>(root.value(), "double");
    auto double_as_float_value =
        root->template As<score::json::Object>().value().get().at("double").template As<float>().has_value();

    // Then the correct value is returned
    EXPECT_FLOAT_EQ(float_value, 2.3F);
    EXPECT_DOUBLE_EQ(double_value, 1.79769e+308);
    EXPECT_FALSE(double_as_float_value);
}

TYPED_TEST_P(ParserTest, CanParseObjectInObject)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse an object from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto& value = GetValueOfObject<Object>(root.value(), "object");

    // Then the correct value is returned
    EXPECT_EQ(value.at("a").template As<std::string>().value().get(), "b");
}

TYPED_TEST_P(ParserTest, CanParseListInObject)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse a list from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

    // Given a simple JSON buffer
    auto root = TypeParam::FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto& value = GetValueOfObject<List>(*root, "list");

    // Then the correct value is returned
    EXPECT_EQ(value[0].template As<std::string>().value().get(), "first");
    EXPECT_EQ(value[1].template As<std::uint64_t>().value(), 2UL);
    EXPECT_EQ(value[2].template As<std::string>().value().get(), "third");
}

TYPED_TEST_P(ParserTest, CanParseObjectInObjectAndIterateOverKeys)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "parse an object where the keys are unknown to the program, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "1");

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
    auto root = TypeParam::FromBuffer(buffer);

    // When iterating over the unknown keys
    const auto& storage_list = root.value().template As<Object>().value().get()["storage_list"];
    std::vector<std::string> collected_paths{};
    for (const auto& element : storage_list.template As<Object>().value().get())
    {
        collected_paths.push_back(
            element.second.template As<Object>().value().get().at("path").template As<std::string>().value().get());
    }

    // Then we can store them and thus also access them
    std::sort(collected_paths.begin(), collected_paths.end());
    EXPECT_EQ(collected_paths.at(0), "/opt/cdc/etc/config/key_value_storage");
    EXPECT_EQ(collected_paths.at(1), "/persistent/cdc/nvmblock/key_value_storage");
}

TYPED_TEST_P(ParserTest, EmitsErrorWhenParsingObjectWithValueButNoKey)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "failure in parsing an object with no keys, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "3");

    std::string buffer{R"(
{
false
}
)"};

    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = TypeParam::FromBuffer(buffer)));
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TYPED_TEST_P(ParserTest, EmitsErrorWhenParsingObjectWithBinaryValue)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "failure in parsing an object with binary value element, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "3");

    // Binary content - format specific to vaJSON Parser
    // First 4 bytes represent length of binary content in std::uint64_t
    // Following 8 bytes represent binary content
    // Hence, this is a binary content of one std::uint64_t with all ones
    std::uint8_t binary[12]{0, 0, 0, 1, 255, 255, 255, 255, 255, 255, 255, 255};

    // Stitch the JSON object together
    std::string buffer{R"(
{
  "binary": b)"};
    buffer += std::string{reinterpret_cast<score::StringLiteral>(binary), 12};
    buffer += R"(
}
)";

    // When trying to read this JSON object there is no fatal failure or exception and...
    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = TypeParam::FromBuffer(buffer)));

    // We get an error returned that specifies that parsing failed
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TYPED_TEST_P(ParserTest, EmitsErrorWhenParsingTooLargeNumber)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "failure in parsing an object with very large value element, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "3");

    std::string buffer{R"({ "number": 1e+500 })"};

    score::Result<Any> root{};
    EXPECT_NO_FATAL_FAILURE(EXPECT_NO_THROW(root = TypeParam::FromBuffer(buffer)));

    // We get an error returned that specifies that parsing failed
    EXPECT_FALSE(root.has_value());
    EXPECT_EQ(root.error(), score::json::Error::kParsingError);
}

TYPED_TEST_P(ParserTest, FromFileFail)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "Invalid file path returns error");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "3");
    // Pass invalid file path
    auto foo = TypeParam::FromFile("foo");

    EXPECT_FALSE(foo.has_value());
}

TYPED_TEST_P(ParserTest, ParsingFromFileWorks)
{
    this->RecordProperty("Verifies", "5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "Verifies that parsing JSON from a file works correctly");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "Analysis of requirements");
    this->RecordProperty("Priority", "3");
    // Given a simple JSON File
    std::string file_name{"test.json"};
    auto root = TypeParam::FromFile(this->CreateJsonFile(file_name, buffer_simple_json));

    // Then json file is successfully parsed
    EXPECT_TRUE(root.has_value());

    // When reading a key of an object that is interpreted as bool
    auto value = GetValueOfObject<bool>(root.value(), "boolean");

    // Then the correct value is returned
    EXPECT_EQ(value, true);
}

REGISTER_TYPED_TEST_SUITE_P(ParserTest,
                            CanParseObjectBool,
                            CanParseObjectString,
                            CanParseObjectNull,
                            CanParseObjectNumber,
                            CanParseObjectFloatingPointNumber,
                            CanParseObjectInObject,
                            CanParseListInObject,
                            CanParseObjectInObjectAndIterateOverKeys,
                            EmitsErrorWhenParsingObjectWithValueButNoKey,
                            EmitsErrorWhenParsingObjectWithBinaryValue,
                            EmitsErrorWhenParsingTooLargeNumber,
                            FromFileFail,
                            ParsingFromFileWorks);
}  // namespace
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_PARSER_PARSER_TEST_SUITE_H
