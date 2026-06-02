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

#ifdef VAJSON
#include "score/json/internal/parser/parsers_test_suite.h"
#include "score/json/internal/parser/vajson/vajson_parser.h"

namespace score
{
namespace json
{
namespace
{

TEST(VajsonParser, CanParseObjectHexadecimalNumber)
{
    this->RecordProperty("Verifies", "SCR-5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description", "parse hex value from json buffer, cf. RFC-8259 section 9");
    this->RecordProperty("TestType", "requirements-based");                // requirements test
    this->RecordProperty("DerivationTechnique", "requirements-analysis");  // requirements

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
    auto root = VajsonParser::FromBuffer(buffer_json_hex);

    // When reading a key of an object that is interpreted as floating point number
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "hex");

    // Then the correct value is returned
    EXPECT_EQ(value, 0xdd);
}

TEST(VajsonParser, CanParseTwoByteBuffer)
{
    this->RecordProperty("Verifies", "SCR-5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "parse a two-byte JSON object from a buffer shorter than the 3-byte UTF-8 BOM");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-value-analysis");

    // Given a valid JSON buffer of exactly 2 bytes (shorter than BOM)
    auto root = VajsonParser::FromBuffer("{}");

    // Then parsing succeeds and returns an empty object
    ASSERT_TRUE(root.has_value());
    auto obj = root->As<Object>();
    ASSERT_TRUE(obj.has_value());
    EXPECT_TRUE(obj->get().empty());
}

TEST(VajsonParser, CanParseBufferWithUtf8Bom)
{
    this->RecordProperty("Verifies", "SCR-5310867");
    this->RecordProperty("ASIL", "B");
    this->RecordProperty("Description",
                         "parse a JSON buffer prefixed with a UTF-8 BOM, ensuring BOM is consumed correctly");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "requirements-analysis");

    // Given a JSON buffer with a UTF-8 BOM prefix
    std::string bom_json{"\xEF\xBB\xBF{\"key\": 123}"};
    auto root = VajsonParser::FromBuffer(bom_json);

    // Then parsing succeeds and the value is correct
    ASSERT_TRUE(root.has_value());
    auto value = GetValueOfObject<std::uint64_t>(root.value(), "key");
    EXPECT_EQ(value, 123U);
}

INSTANTIATE_TYPED_TEST_SUITE_P(Test, ParserTest, VajsonParser, /*unused*/);

}  // namespace
}  // namespace json
}  // namespace score
#else
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ParserTest);
#endif
