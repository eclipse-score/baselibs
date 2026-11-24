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
/// @file
/// @brief  Contains tests concerning the parsing of invalid JSON.

#include "score/json/internal/parser/vajson/structure_parser.h"

#include <initializer_list>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/json_data.h"
#include "score/json/internal/parser/vajson/json_error_domain.h"
#include "score/json/internal/parser/vajson/structure_parser_stub.h"

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

/// Helper: assert that the given JSON string is rejected with kInvalidJson.
void AssertInvalid(std::string_view json)
{
    auto data{JsonData::FromBuffer(json)};
    ASSERT_TRUE(data.has_value());
    StructureParserStub parser{data.value()};
    const auto result{parser.Parse()};
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
}

}  // namespace

// ---- Empty document -------------------------------------------------------

TEST(StructureParserInvalidTest, RejectsEmptyDocument)
{
    AssertInvalid("");
}

TEST(StructureParserInvalidTest, RejectsDocumentWithoutData)
{
    AssertInvalid("");
}

// ---- Malformed JSON keywords (wrong length) --------------------------------

TEST(StructureParserInvalidTest, RejectsTrueWithTrailingLetter)
{
    AssertInvalid("truee");
}

TEST(StructureParserInvalidTest, RejectsFalseWithTrailingLetter)
{
    AssertInvalid("falsee");
}

// ---- Malformed JSON keywords (wrong case) ----------------------------------

TEST(StructureParserInvalidTest, RejectsCapitalizedFalse)
{
    AssertInvalid("False");
}

TEST(StructureParserInvalidTest, RejectsCapitalizedTrue)
{
    AssertInvalid("True");
}

TEST(StructureParserInvalidTest, RejectsCapitalizedNull)
{
    AssertInvalid("Null");
}

// ---- Malformed JSON keywords (correct length/case, wrong chars) ------------

TEST(StructureParserInvalidTest, RejectsNullWithWrongLetters)
{
    AssertInvalid("nxyl");
}

TEST(StructureParserInvalidTest, RejectsNullWithDigits)
{
    AssertInvalid("n00l");
}

TEST(StructureParserInvalidTest, RejectsTrueWithWrongLetters)
{
    AssertInvalid("txye");
}

TEST(StructureParserInvalidTest, RejectsTrueWithDigits)
{
    AssertInvalid("t12e");
}

TEST(StructureParserInvalidTest, RejectsFalseWithWrongLetters)
{
    AssertInvalid("fxyze");
}

TEST(StructureParserInvalidTest, RejectsFalseWithDigits)
{
    AssertInvalid("f456e");
}

TEST(StructureParserInvalidTest, RejectsTruncatedInput)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that documents ending inside a literal or right after an escape character are rejected as "
                   "invalid JSON.");

    for (const std::string_view json : {"n", "nul", "tru", "fals", R"([true, fals)", R"({"a": nul)", R"("abc\)"})
    {
        SCOPED_TRACE(json);
        AssertInvalid(json);
    }
}

// ---- Invalid JSON sequences ------------------------------------------------

TEST(StructureParserInvalidTest, RejectsKeyWithoutValue)
{
    AssertInvalid(R"({ "key": , "another_key": )");
}

TEST(StructureParserInvalidTest, RejectsValueWithoutKey)
{
    AssertInvalid(R"({ "key": "value", "value", "key": 1 })");
}

TEST(StructureParserInvalidTest, RejectsEndOfInputAfterKey)
{
    AssertInvalid(R"({"key")");
}

TEST(StructureParserInvalidTest, RejectsObjectCloseInsteadOfValue)
{
    AssertInvalid(R"({ "key": } )");
}

TEST(StructureParserInvalidTest, RejectsObjectCloseInArray)
{
    AssertInvalid(R"([ } ])");
}

TEST(StructureParserInvalidTest, RejectsArrayOpenFollowedByObjectClose)
{
    AssertInvalid(R"({ [ })");
}

TEST(StructureParserInvalidTest, RejectsArrayCloseInObject)
{
    AssertInvalid(R"({ ] })");
}

TEST(StructureParserInvalidTest, RejectsUnterminatedString)
{
    AssertInvalid(R"("This goes on until the end of file)");
}

TEST(StructureParserInvalidTest, RejectsKeyWithMissingClosingQuote)
{
    AssertInvalid(R"({ "this is a key: 1, "k": 1 } )");
}

TEST(StructureParserInvalidTest, RejectsLoneMinusSign)
{
    AssertInvalid(R"(-)");
}

TEST(StructureParserInvalidTest, RejectsNumberWithLeadingZeros)
{
    AssertInvalid(R"(000)");
}

TEST(StructureParserInvalidTest, RejectsNumberFollowedByHexadecimalLetter)
{
    AssertInvalid(R"(0a)");
}

TEST(StructureParserInvalidTest, RejectsNumberFollowedByNonHexadecimalLetter)
{
    AssertInvalid(R"(0z)");
}

TEST(StructureParserInvalidTest, RejectsUnclosedObject)
{
    AssertInvalid("{");
}

TEST(StructureParserInvalidTest, RejectsUnclosedArray)
{
    AssertInvalid("[");
}

TEST(StructureParserInvalidTest, RejectsTopLevelKey)
{
    AssertInvalid(R"("key": 2)");
}

TEST(StructureParserInvalidTest, RejectsCommaSeparatedTopLevelValues)
{
    AssertInvalid(R"(2, 3, 4)");
}

TEST(StructureParserInvalidTest, RejectsMultipleTopLevelObjects)
{
    AssertInvalid(R"({}{})");
}

TEST(StructureParserInvalidTest, RejectsWhitespaceSeparatedTopLevelValues)
{
    AssertInvalid(R"(2 3 4)");
}

TEST(StructureParserInvalidTest, RejectsTruncatedUnicodeEscape)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode, comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that an incomplete Unicode escape at end of input is rejected.");

    AssertInvalid(R"("test\u12)");
}

TEST(StructureParserInvalidTest, RejectsNonHexadecimalUnicodeEscape)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode, comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a Unicode escape with a non-hexadecimal digit is rejected.");

    AssertInvalid(R"("test\u12g4")");
}

TEST(StructureParserInvalidTest, RejectsLoneHighSurrogate)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a high surrogate without a following low surrogate is rejected.");

    AssertInvalid(R"("test\ud83d")");
}

TEST(StructureParserInvalidTest, RejectsHighSurrogateFollowedByNonSurrogate)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a high surrogate followed by a non-surrogate escape is rejected.");

    AssertInvalid(R"("test\ud83d\u0041")");
}

TEST(StructureParserInvalidTest, RejectsHighSurrogateFollowedByOtherEscape)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a high surrogate followed by an escape other than \\u is rejected.");

    AssertInvalid(R"("test\ud83d\n")");
}

TEST(StructureParserInvalidTest, RejectsLoneLowSurrogate)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__unicode");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that a low surrogate without a preceding high surrogate is rejected.");

    AssertInvalid(R"("test\ude00")");
}

TEST(StructureParserInvalidTest, RejectsUnknownEscape)
{
    AssertInvalid(R"("test\{1234\}")");
}

TEST(StructureParserInvalidTest, RejectsUnescapedControlCharacterInString)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that a string value containing any unescaped character from U+0000 to U+001F is rejected.");

    for (char control_character{'\x00'}; control_character < '\x20'; ++control_character)
    {
        SCOPED_TRACE(static_cast<int>(control_character));
        const std::string json{std::string{R"(["a)"} + control_character + R"(b"])"};
        AssertInvalid(json);
    }
}

TEST(StructureParserInvalidTest, RejectsUnescapedControlCharacterInKey)
{
    RecordProperty("PartiallyVerifies", "comp_req__vajson__validation");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "Check that an object key containing an unescaped tab character is rejected.");

    AssertInvalid("{\"a\tb\":1}");
}

TEST(StructureParserInvalidTest, RejectsDocumentWithOnlyWhitespace)
{
    AssertInvalid(" \t\n\r");
}

TEST(StructureParserInvalidTest, RejectsLoneArrayClose)
{
    AssertInvalid("]");
}

TEST(StructureParserInvalidTest, RejectsLoneObjectClose)
{
    AssertInvalid("}");
}

TEST(StructureParserInvalidTest, RejectsOctalNumber)
{
    AssertInvalid("0123");
}

TEST(StructureParserInvalidTest, RejectsNegativeOctalNumber)
{
    AssertInvalid("-010");
}

TEST(StructureParserInvalidTest, RejectsObjectOpenInsteadOfKey)
{
    AssertInvalid("{{");
}

TEST(StructureParserInvalidTest, RejectsArrayOpenInsteadOfKey)
{
    AssertInvalid("{[");
}

TEST(StructureParserInvalidTest, RejectsKeyFollowedByCommaAndString)
{
    AssertInvalid(R"({"true", ""})");
}

TEST(StructureParserInvalidTest, RejectsKeyFollowedByObjectClose)
{
    AssertInvalid(R"({"true" })");
}

TEST(StructureParserInvalidTest, RejectsNumberAsKey)
{
    AssertInvalid(R"({1234"true" })");
}

TEST(StructureParserInvalidTest, RejectsMissingCommaInObject)
{
    AssertInvalid(R"({"key":1 "Key":2})");
}

TEST(StructureParserInvalidTest, RejectsMissingCommaInArray)
{
    AssertInvalid(R"(["abc" "def"])");
}

TEST(StructureParserInvalidTest, RejectsUnquotedKey)
{
    AssertInvalid(R"({unquoted_key: "keys must be quoted"})");
}

TEST(StructureParserInvalidTest, RejectsCommaAfterTopLevelArray)
{
    AssertInvalid(R"(["Comma after the close"],)");
}

TEST(StructureParserInvalidTest, RejectsExtraArrayClose)
{
    AssertInvalid(R"(["Extra close"]])");
}

TEST(StructureParserInvalidTest, RejectsValueAfterTopLevelObject)
{
    AssertInvalid(R"({"Extra value after close": true} "misplaced quoted value")");
}

TEST(StructureParserInvalidTest, RejectsExpressionAsValue)
{
    AssertInvalid(R"({"Illegal expression": 1 + 2})");
}

TEST(StructureParserInvalidTest, RejectsFunctionCallAsValue)
{
    AssertInvalid(R"({"Illegal invocation": alert()})");
}

TEST(StructureParserInvalidTest, RejectsHexadecimalEscape)
{
    AssertInvalid(R"(["Illegal backslash escape: \x15"])");
}

TEST(StructureParserInvalidTest, RejectsEscapeOutsideString)
{
    AssertInvalid(R"([\naked])");
}

TEST(StructureParserInvalidTest, RejectsOctalEscape)
{
    AssertInvalid(R"(["Illegal backslash escape: \017"])");
}

TEST(StructureParserInvalidTest, RejectsMissingColon)
{
    AssertInvalid(R"({"Missing colon" null})");
}

TEST(StructureParserInvalidTest, RejectsDoubleColon)
{
    AssertInvalid(R"({"Double colon":: null})");
}

TEST(StructureParserInvalidTest, RejectsCommaInsteadOfColon)
{
    AssertInvalid(R"({"Comma instead of colon", null})");
}

TEST(StructureParserInvalidTest, RejectsColonInArray)
{
    AssertInvalid(R"(["Colon instead of comma": false])");
}

TEST(StructureParserInvalidTest, RejectsUnknownLiteral)
{
    AssertInvalid(R"(["Bad value", truth])");
}

TEST(StructureParserInvalidTest, RejectsSingleQuotedString)
{
    AssertInvalid(R"(['single quote'])");
}

TEST(StructureParserInvalidTest, RejectsBackslashFollowedBySpace)
{
    AssertInvalid("[\"tab\\   character\\   in\\  string\\  \"]");
}

TEST(StructureParserInvalidTest, RejectsBackslashFollowedByLineBreak)
{
    AssertInvalid("[\"line\\\nbreak\"]");
}

TEST(StructureParserInvalidTest, RejectsCommaInsteadOfObjectClose)
{
    AssertInvalid(R"({"Comma instead if closing brace": true,)");
}

TEST(StructureParserInvalidTest, RejectsMissingCommaBetweenObjectsInArray)
{
    AssertInvalid(R"([{}{}])");
}

TEST(StructureParserInvalidTest, RejectsValueAfterTopLevelArray)
{
    AssertInvalid(R"([]123)");
}

TEST(StructureParserInvalidTest, RejectsKeyAfterTopLevelArray)
{
    AssertInvalid(R"([]"key":)");
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
