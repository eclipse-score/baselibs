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

#include <array>
#include <string>
#include <string_view>
#include <utility>

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

TEST(JsonParserVajsonTest, AcceptsNumbersMatchingRfc8259Grammar)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__json__deserialization");
    this->RecordProperty("Description",
                         "Check that numbers at the edges of the RFC 8259 number grammar are parsed to their value.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "boundary-values");

    const std::array<std::pair<std::string_view, double>, 9> numbers{{{"0", 0.0},
                                                                      {"-0", 0.0},
                                                                      {"0.5", 0.5},
                                                                      {"0e0", 0.0},
                                                                      {"0E+1", 0.0},
                                                                      {"10", 10.0},
                                                                      {"1E+2", 100.0},
                                                                      {"-1.5e-3", -1.5e-3},
                                                                      {"123.456E2", 12345.6}}};

    for (const auto& [literal, expected] : numbers)
    {
        SCOPED_TRACE(literal);
        const std::string buffer{R"({"number": )" + std::string{literal} + "}"};

        const auto root = JsonParser{}.FromBuffer(buffer);

        ASSERT_TRUE(root.has_value());
        EXPECT_DOUBLE_EQ(GetValueOfObject<double>(root.value(), "number"), expected);
    }
}

TEST(JsonParserVajsonTest, RejectsNumbersOutsideRfc8259Grammar)
{
    this->RecordProperty("PartiallyVerifies", "comp_req__vajson__number_grammar, comp_req__vajson__validation");
    this->RecordProperty("Description",
                         "Check that number literals outside the RFC 8259 number grammar, e.g. hexadecimal, leading "
                         "zeros or incomplete fractions and exponents, are rejected as malformed JSON.");
    this->RecordProperty("TestType", "requirements-based");
    this->RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::array<std::string_view, 18> literals{"0xdd",
                                                    "0XDD",
                                                    "-0x1",
                                                    "0x1p3",
                                                    "01",
                                                    "-01",
                                                    "00",
                                                    "1.",
                                                    "-.5",
                                                    "1.e5",
                                                    "1e",
                                                    "1e+",
                                                    "1E-",
                                                    "--1",
                                                    "-inf",
                                                    "-nan",
                                                    "1abc",
                                                    "1e5.0"};

    for (const auto literal : literals)
    {
        SCOPED_TRACE(literal);
        const std::string buffer{R"({"number": )" + std::string{literal} + "}"};

        const auto root = JsonParser{}.FromBuffer(buffer);

        EXPECT_FALSE(root.has_value());
    }
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
