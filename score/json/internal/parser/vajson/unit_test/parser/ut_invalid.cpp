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
 *        \brief  Contains tests concerning the parsing of invalid JSON.
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "gtest/gtest.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/internal/parsers/virtual_parser.h"
#include "score/json/internal/parser/vajson/vajson_impl/reader/json_data.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"

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

class DynamicInvalidDetectionParser final : public internal::VirtualParser
{
  public:
    using internal::VirtualParser::VirtualParser;

    auto OnUnexpectedEvent() noexcept -> ParserResult override
    {
        return ParserState::kRunning;
    }
};

/// Helper: assert that the given JSON string is rejected with kInvalidJson.
void AssertInvalid(std::string_view json)
{
    auto data = JsonData::FromBuffer(json);
    ASSERT_TRUE(data.has_value());
    DynamicInvalidDetectionParser parser{data.value()};
    const auto result = parser.Parse();
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), JsonErrc::kInvalidJson);
}

}  // namespace

// ---- Empty document -------------------------------------------------------

TEST(UT__Parser__Invalid, Dynamic__EmptyDocument)
{
    AssertInvalid("");
}

TEST(UT__Parser__Invalid, Dynamic__NoData)
{
    AssertInvalid("");
}

// ---- Malformed JSON keywords (wrong length) --------------------------------

TEST(UT__Parser__Invalid, Dynamic__UnknownType3_truee)
{
    AssertInvalid("truee");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType4_falsee)
{
    AssertInvalid("falsee");
}

// ---- Malformed JSON keywords (wrong case) ----------------------------------

TEST(UT__Parser__Invalid, Dynamic__UnknownType6_False)
{
    AssertInvalid("False");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType7_True)
{
    AssertInvalid("True");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType8_Null)
{
    AssertInvalid("Null");
}

// ---- Malformed JSON keywords (correct length/case, wrong chars) ------------

TEST(UT__Parser__Invalid, Dynamic__UnknownType9_nxyl)
{
    AssertInvalid("nxyl");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType10_n00l)
{
    AssertInvalid("n00l");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType11_txye)
{
    AssertInvalid("txye");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType12_t12e)
{
    AssertInvalid("t12e");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType13_fxyze)
{
    AssertInvalid("fxyze");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownType14_f456e)
{
    AssertInvalid("f456e");
}

// ---- Invalid JSON sequences ------------------------------------------------

TEST(UT__Parser__Invalid, Dynamic__KeyWithoutValue)
{
    AssertInvalid(R"({ "key": , "another_key": )");
}

TEST(UT__Parser__Invalid, Dynamic__ValueWithoutKey)
{
    AssertInvalid(R"({ "key": "value", "value", "key": 1 })");
}

TEST(UT__Parser__Invalid, Dynamic__EofAfterKey)
{
    AssertInvalid(R"({"key")");
}

TEST(UT__Parser__Invalid, Dynamic__UnexpectedObjectClose1)
{
    AssertInvalid(R"({ "key": } )");
}

TEST(UT__Parser__Invalid, Dynamic__UnexpectedObjectClose2)
{
    AssertInvalid(R"([ } ])");
}

TEST(UT__Parser__Invalid, Dynamic__UnexpectedArrayOpen)
{
    AssertInvalid(R"({ [ })");
}

TEST(UT__Parser__Invalid, Dynamic__UnexpectedArrayClose)
{
    AssertInvalid(R"({ ] })");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingString1)
{
    AssertInvalid(R"("This goes on until the end of file)");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingString2)
{
    AssertInvalid(R"({ "this is a key: 1, "k": 1 } )");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingNumber1)
{
    AssertInvalid(R"(-)");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingNumber2)
{
    AssertInvalid(R"(000)");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingNumber3)
{
    AssertInvalid(R"(0a)");
}

TEST(UT__Parser__Invalid, Dynamic__NonterminatingNumber4)
{
    AssertInvalid(R"(0z)");
}

TEST(UT__Parser__Invalid, Dynamic__NonTerminatingObject)
{
    AssertInvalid("{");
}

TEST(UT__Parser__Invalid, Dynamic__NonTerminatingArray)
{
    AssertInvalid("[");
}

TEST(UT__Parser__Invalid, Dynamic__TopLevelKeys)
{
    AssertInvalid(R"("key": 2)");
}

TEST(UT__Parser__Invalid, Dynamic__MultipleTopLevelElements)
{
    AssertInvalid(R"(2, 3, 4)");
}

TEST(UT__Parser__Invalid, Dynamic__MultipleTopLevelObjects)
{
    AssertInvalid(R"({}{})");
}

TEST(UT__Parser__Invalid, Dynamic__MultipleValues)
{
    AssertInvalid(R"(2 3 4)");
}

TEST(UT__Parser__Invalid, Dynamic__UnicodeEscape)
{
    AssertInvalid(R"("test\u1234")");
}

TEST(UT__Parser__Invalid, Dynamic__UnknownEscape)
{
    AssertInvalid(R"("test\{1234\}")");
}

TEST(UT__Parser__Invalid, Dynamic__OnlyWhitespace)
{
    AssertInvalid(" \t\n\r");
}

TEST(UT__Parser__Invalid, Dynamic__OnlyArrayClose)
{
    AssertInvalid("]");
}

TEST(UT__Parser__Invalid, Dynamic__OnlyObjectClose)
{
    AssertInvalid("}");
}

TEST(UT__Parser__Invalid, Dynamic__OctalNumber1)
{
    AssertInvalid("0123");
}

TEST(UT__Parser__Invalid, Dynamic__OctalNumber2)
{
    AssertInvalid("-010");
}

TEST(UT__Parser__Invalid, Dynamic__ObjectOpenInsteadOfKey)
{
    AssertInvalid("{{");
}

TEST(UT__Parser__Invalid, Dynamic__ArrayOpenInsteadOfKey)
{
    AssertInvalid("{[");
}

TEST(UT__Parser__Invalid, Dynamic__StringInsteadOfKey)
{
    AssertInvalid(R"({"true", ""})");
}

TEST(UT__Parser__Invalid, Dynamic__ValueInsteadOfKey)
{
    AssertInvalid(R"({"true" })");
}

TEST(UT__Parser__Invalid, Dynamic__NumberAndString)
{
    AssertInvalid(R"({1234"true" })");
}

TEST(UT__Parser__Invalid, Dynamic__MissingCommaInObject)
{
    AssertInvalid(R"({"key":1 "Key":2})");
}

TEST(UT__Parser__Invalid, Dynamic__MissingCommaInArray)
{
    AssertInvalid(R"(["abc" "def"])");
}

TEST(UT__Parser__Invalid, Dynamic__UnquotedKey)
{
    AssertInvalid(R"({unquoted_key: "keys must be quoted"})");
}

TEST(UT__Parser__Invalid, Dynamic__CommaAfterClose)
{
    AssertInvalid(R"(["Comma after the close"],)");
}

TEST(UT__Parser__Invalid, Dynamic__ExtraArrayAfterClose)
{
    AssertInvalid(R"(["Extra close"]])");
}

TEST(UT__Parser__Invalid, Dynamic__ExtraValueAfterClose)
{
    AssertInvalid(R"({"Extra value after close": true} "misplaced quoted value")");
}

TEST(UT__Parser__Invalid, Dynamic__IllegalExpression)
{
    AssertInvalid(R"({"Illegal expression": 1 + 2})");
}

TEST(UT__Parser__Invalid, Dynamic__IllegalInvocation)
{
    AssertInvalid(R"({"Illegal invocation": alert()})");
}

TEST(UT__Parser__Invalid, Dynamic__IllegalEscape)
{
    AssertInvalid(R"(["Illegal backslash escape: \x15"])");
}

TEST(UT__Parser__Invalid, Dynamic__Naked)
{
    AssertInvalid(R"([\naked])");
}

TEST(UT__Parser__Invalid, Dynamic__IllegalEscapeNumber)
{
    AssertInvalid(R"(["Illegal backslash escape: \017"])");
}

TEST(UT__Parser__Invalid, Dynamic__MissingColon)
{
    AssertInvalid(R"({"Missing colon" null})");
}

TEST(UT__Parser__Invalid, Dynamic__DoubleColon)
{
    AssertInvalid(R"({"Double colon":: null})");
}

TEST(UT__Parser__Invalid, Dynamic__CommaInsteadOfColon)
{
    AssertInvalid(R"({"Comma instead of colon", null})");
}

TEST(UT__Parser__Invalid, Dynamic__ColonInsteadOfComma)
{
    AssertInvalid(R"(["Colon instead of comma": false])");
}

TEST(UT__Parser__Invalid, Dynamic__BadValue)
{
    AssertInvalid(R"(["Bad value", truth])");
}

TEST(UT__Parser__Invalid, Dynamic__SingleQuote)
{
    AssertInvalid(R"(['single quote'])");
}

TEST(UT__Parser__Invalid, Dynamic__BackslashInString)
{
    AssertInvalid("[\"tab\\   character\\   in\\  string\\  \"]");
}

TEST(UT__Parser__Invalid, Dynamic__BackslashLineBreak)
{
    AssertInvalid("[\"line\\\nbreak\"]");
}

TEST(UT__Parser__Invalid, Dynamic__CommaInsteadOfClosing)
{
    AssertInvalid(R"({"Comma instead if closing brace": true,)");
}

TEST(UT__Parser__Invalid, Dynamic__ArrayWithoutComma)
{
    AssertInvalid(R"([{}{}])");
}

TEST(UT__Parser__Invalid, Dynamic__TrailingValue)
{
    AssertInvalid(R"([]123)");
}

TEST(UT__Parser__Invalid, Dynamic__TrailingKey)
{
    AssertInvalid(R"([]"key":)");
}

}  // namespace unit_test
}  // namespace vajson
}  // namespace json
}  // namespace score
