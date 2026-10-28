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

#include "score/json/internal/parser/parsers_test_suite.h"
#include "score/json/json_parser.h"

namespace score
{
namespace json
{
namespace
{

/// @brief Gives the typed test suite a static entry point to JsonParser.
struct JsonParserEntryPoint
{
    static auto FromFile(const std::string_view file_path) -> score::Result<Any>
    {
        return JsonParser{}.FromFile(file_path);
    }

    static auto FromBuffer(const std::string_view buffer) -> score::Result<Any>
    {
        return JsonParser{}.FromBuffer(buffer);
    }
};

TEST(JsonParserVajsonTest, CanParseObjectHexadecimalNumber)
{
    // vaJson accepts hexadecimal number syntax as a lenient extension beyond the RFC 8259 well-formedness
    // that comp_req__json__deserialization mandates, so this documents the parser's actual behavior rather than
    // normative text.
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that parsing a hexadecimal number, an accepted extension beyond RFC-8259, returns "
                         "the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "design-analysis");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty(
        "Description",
        "Check that parsing a two-byte JSON object, shorter than the 3-byte UTF-8 BOM, returns the correct value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

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
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that parsing a JSON buffer prefixed with a UTF-8 BOM consumes the BOM correctly.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a JSON buffer with a UTF-8 BOM prefix
    std::string bom_json{"\xEF\xBB\xBF{\"key\": 123}"};
    auto root = JsonParser{}.FromBuffer(bom_json);

    // Then parsing succeeds and the value is correct
    ASSERT_TRUE(root.has_value());
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "key");
    EXPECT_EQ(value, 123U);
}

INSTANTIATE_TYPED_TEST_SUITE_P(Test, ParserTest, JsonParserEntryPoint, /*unused*/);

}  // namespace
}  // namespace json
}  // namespace score
