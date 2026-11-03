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
/*!        \file
 *        \brief  Contains tests concerning the parsing of strings.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/reader/json_data.h"
#include "score/json/internal/parser/vajson/util/json_error_domain.h"
#include <optional>
#include <string>
#include <vector>

namespace score
{
namespace json
{
namespace vajson
{
namespace unit_test
{
namespace
{

using Strings = std::vector<std::string>;

class DynamicStringParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnString(StringView str) noexcept -> ParserResult override
    {
        this->strings.push_back(std::string(str));
        return ParserState::kRunning;
    }

    auto OnKey(StringView key) noexcept -> ParserResult override
    {
        this->keys.push_back(std::string(key));
        return ParserState::kRunning;
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }

    Strings strings;
    Strings keys;
};

class ErrorStringParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnString(StringView) noexcept -> ParserResult override
    {
        return MakeErrorResult<ParserState>(JsonErrc::kUserValidationFailed, "OnString called.");
    }

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

/// Helper: parse a document that consists of a single string and return the decoded value.
auto ParseString(const std::string_view json) -> std::optional<std::string>
{
    auto data = JsonData::FromBuffer(json);
    if (!data.has_value())
    {
        return std::nullopt;
    }
    DynamicStringParser parser{data.value()};
    if (!parser.Parse().has_value() || (parser.strings.size() != 1U))
    {
        return std::nullopt;
    }
    return parser.strings.front();
}

}  // namespace

/*!
 * Test that string callbacks are called for a string array.
 * \trace           CREQ-Json-EventCallbacks
 */
TEST(UT__Parser__String, Dynamic__StringArray)
{
    auto data = JsonData::FromBuffer(std::string_view{R"(["hello","world"])"});
    ASSERT_TRUE(data.has_value());
    DynamicStringParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    EXPECT_EQ(parser.strings, (Strings{"hello", "world"}));
}

/*!
 * Tests that when the overridden OnString method returns an error, that error is propagated.
 * \trace           score::json::vajson::internal::StructureParserBase::ParseUnescapedString
 */
TEST(UT__Parser__String, Dynamic__ParseUnescapedString__Error)
{
    auto doc = JsonData::FromBuffer(std::string_view{R"("SomeString")"});
    ASSERT_TRUE(doc.has_value());
    ErrorStringParser parser(doc.value());

    const auto parser_result = parser.Parse();
    ASSERT_FALSE(parser_result.has_value());
    ASSERT_EQ(parser_result.error(), JsonErrc::kUserValidationFailed);
    ASSERT_EQ(parser_result.error().UserMessage(), std::string_view{"OnString called."});
}

/*!
 * Tests that the two-character JSON string escapes are decoded before the string callback.
 */
TEST(UT__Parser__String, Dynamic__TwoCharacterEscapes)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that standard JSON string escapes are decoded before the callback.");

    const auto parsed = ParseString(R"("\"\\\/\b\f\n\r\t")");
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed.value(), (std::string{"\"\\/\b\f\n\r\t"}));
}

/*!
 * Tests that Unicode escapes are decoded into UTF-8, for every UTF-8 sequence length.
 */
TEST(UT__Parser__String, Dynamic__UnicodeEscape)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "Check that Unicode escapes are decoded into UTF-8 for every sequence length.");

    // One byte: the control characters, the quotation mark and the reverse solidus.
    const auto single_byte = ParseString(R"("\u0001\u001f\u0022\u005C")");
    ASSERT_TRUE(single_byte.has_value());
    EXPECT_EQ(single_byte.value(), (std::string{"\x01\x1F\"\\"}));

    // Two bytes: U+00E4 LATIN SMALL LETTER A WITH DIAERESIS.
    const auto two_bytes = ParseString(R"("\u00e4")");
    ASSERT_TRUE(two_bytes.has_value());
    EXPECT_EQ(two_bytes.value(), (std::string{"\xC3\xA4"}));

    // Three bytes: U+20AC EURO SIGN.
    const auto three_bytes = ParseString(R"("\u20AC")");
    ASSERT_TRUE(three_bytes.has_value());
    EXPECT_EQ(three_bytes.value(), (std::string{"\xE2\x82\xAC"}));

    // Four bytes: U+1F600 GRINNING FACE, escaped as a UTF-16 surrogate pair.
    const auto four_bytes = ParseString(R"("\ud83d\ude00")");
    ASSERT_TRUE(four_bytes.has_value());
    EXPECT_EQ(four_bytes.value(), (std::string{"\xF0\x9F\x98\x80"}));
}

/*!
 * Tests Unicode escapes in context, including boundary values and non-recursive decoding.
 */
TEST(UT__Parser__String, Dynamic__UnicodeEscapeInContext)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check Unicode escapes within strings and ensure decoded backslashes are not reprocessed.");

    const auto control_character = ParseString(R"("a\u0001z")");
    ASSERT_TRUE(control_character.has_value());
    EXPECT_EQ(control_character.value(), (std::string{"a\x01z"}));

    const auto mixed_escapes = ParseString(R"("\u0041\n\u0042")");
    ASSERT_TRUE(mixed_escapes.has_value());
    EXPECT_EQ(mixed_escapes.value(), (std::string{"A\nB"}));

    const auto escaped_backslash = ParseString(R"("\u005Cu0041")");
    ASSERT_TRUE(escaped_backslash.has_value());
    EXPECT_EQ(escaped_backslash.value(), std::string{"\\u0041"});

    // U+007F is the last code point that UTF-8 encodes in a single byte, U+0080 the first one that needs two.
    const auto single_byte_boundary = ParseString(R"("\u007f")");
    ASSERT_TRUE(single_byte_boundary.has_value());
    EXPECT_EQ(single_byte_boundary.value(), std::string{"\x7f"});

    const auto two_byte_boundary = ParseString(R"("\u0080")");
    ASSERT_TRUE(two_byte_boundary.has_value());
    EXPECT_EQ(two_byte_boundary.value(), (std::string{"\xC2\x80"}));
}

/*!
 * Tests that a Unicode escape is decoded when it appears in an object key.
 */
TEST(UT__Parser__String, Dynamic__UnicodeEscapeInKey)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a required control-character escape in an object key is decoded.");

    auto data = JsonData::FromBuffer(std::string_view{R"({"a\u001eb":"v"})"});
    ASSERT_TRUE(data.has_value());
    DynamicStringParser parser{data.value()};
    ASSERT_TRUE(parser.Parse().has_value());
    ASSERT_EQ(parser.keys.size(), 1U);
    EXPECT_EQ(parser.keys.front(),
              (std::string{"a\x1e"
                           "b"}));
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
