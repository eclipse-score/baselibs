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

#include "score/json/internal/model/any.h"
#include "score/json/json_parser.h"

#include <algorithm>
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

TEST_F(ParserTest, CanParseObjectBool)
{
    RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    RecordProperty("Description", "Check that parsing a boolean value from a JSON buffer returns the correct value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a simple JSON buffer
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as bool
    auto value = GetValueOfObject<bool>(root.value(), "boolean");

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as std::string
    auto& value = GetValueOfObject<std::string>(root.value(), "color");

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as Null
    auto& value = GetValueOfObject<Null>(root.value(), "null");

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "number");

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as floating point number
    auto float_value = GetValueOfObject<float>(root.value(), "float");
    double double_value = GetValueOfObject<double>(root.value(), "double");
    auto double_as_float_value = root->As<score::json::Object>().value().get().at("double").As<float>().has_value();

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto& value = GetValueOfObject<Object>(root.value(), "object");

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
    auto root = JsonParser{}.FromBuffer(buffer_simple_json);

    // When reading a key of an object that is interpreted as number
    auto& value = GetValueOfObject<List>(*root, "list");

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
    auto root = JsonParser{}.FromBuffer(buffer);

    // When iterating over the unknown keys
    const auto& storage_list = root.value().As<Object>().value().get()["storage_list"];
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
    auto foo = JsonParser{}.FromFile("foo");

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
    auto root = JsonParser{}.FromFile(CreateJsonFile(file_name, buffer_simple_json));

    // Then json file is successfully parsed
    EXPECT_TRUE(root.has_value());

    // When reading a key of an object that is interpreted as bool
    auto value = GetValueOfObject<bool>(root.value(), "boolean");

    // Then the correct value is returned
    EXPECT_EQ(value, true);
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
    auto root = JsonParser{}.FromBuffer(buffer_json_hex);

    // When reading a key of an object that is interpreted as floating point number
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "hex");

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
    auto root = JsonParser{}.FromBuffer("{}");

    // Then parsing succeeds and returns an empty object
    ASSERT_TRUE(root.has_value());
    auto obj = root->As<Object>();
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
    auto root = JsonParser{}.FromBuffer(bom_json);

    // Then parsing succeeds and the value is correct
    ASSERT_TRUE(root.has_value());
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "key");
    EXPECT_EQ(value, 123U);
}

}  // namespace
}  // namespace json
}  // namespace score
