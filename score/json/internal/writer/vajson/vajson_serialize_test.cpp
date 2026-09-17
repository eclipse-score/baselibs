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
#include "score/json/internal/writer/vajson/vajson_serialize.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
namespace score
{
namespace json
{
namespace
{
TEST(VajsonSerializeTest, SerializesNestedAnyToCompactJson)
{
    Object nested_object{};
    nested_object["number"] = Any{std::int32_t{7}};
    List list{};
    list.emplace_back(Any{Null{}});
    list.emplace_back(Any{std::move(nested_object)});
    Object root{};
    root["boolean"] = Any{true};
    root["list"] = Any{std::move(list)};
    root["string"] = Any{std::string{"line1\n\"quoted\"\\line2"}};
    const auto result = VajsonToBuffer(Any{std::move(root)});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result,
              std::string{
                  "{\"boolean\":true,\"list\":[null,{\"number\":7}],\"string\":\"line1\\n\\\"quoted\\\"\\\\line2\"}"});
}
TEST(VajsonSerializeTest, SerializesObjectKeysUsingStringComparisonAdaptor)
{
    Object object{};
    object[std::string_view{"alpha"}] = Any{std::string{"a"}};
    object["beta"] = Any{std::uint32_t{2U}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"alpha\":\"a\",\"beta\":2}"});
}
// RFC 8259, section 7 does not allow characters in the range U+0000 to U+001F to appear unescaped in a string.
TEST(VajsonSerializeTest, EscapesControlCharactersWithoutShortEscapeSequence)
{
    Object object{};
    object["value"] = Any{std::string{"\x01\x0b\x1f"}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"value\":\"\\u0001\\u000b\\u001f\"}"});
}
TEST(VajsonSerializeTest, EscapesNullCharacterInsideString)
{
    Object object{};
    object["value"] = Any{std::string{std::string_view{"a\0b", 3U}}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"value\":\"a\\u0000b\"}"});
}
TEST(VajsonSerializeTest, EscapesControlCharactersInObjectKeys)
{
    Object object{};
    object[std::string{"a\x1e" "b"}] = Any{std::string{"v"}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"a\\u001eb\":\"v\"}"});
}
TEST(VajsonSerializeTest, PrefersShortEscapeSequencesOverUnicodeEscapes)
{
    Object object{};
    object["value"] = Any{std::string{"\b\f\n\r\t"}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"value\":\"\\b\\f\\n\\r\\t\"}"});
}
TEST(VajsonSerializeTest, EscapesEveryControlCharacterAndNothingElse)
{
    std::string value{};
    for (std::uint32_t character{0U}; character <= 0x7FU; ++character)
    {
        value.push_back(static_cast<char>(character));
    }
    Object object{};
    object["value"] = Any{value};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());

    // No unescaped control character may survive in the output.
    for (const char serialized : *result)
    {
        EXPECT_GE(std::char_traits<char>::to_int_type(serialized), 0x20)
            << "unescaped control character in serialized output";
    }
    // The printable characters are written as they are, with only the quote and the backslash escaped.
    EXPECT_NE(result->find("!\\\"#$%&'()*+,-./0123456789:;<=>?"), std::string::npos);
    EXPECT_NE(result->find("[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"), std::string::npos);
}
TEST(VajsonSerializeTest, PassesMultiByteUtf8CharactersThrough)
{
    Object object{};
    object["value"] = Any{std::string{"\u00e4\u20ac"}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"value\":\"\u00e4\u20ac\"}"});
}
TEST(VajsonSerializeTest, SerializesFiniteDouble)
{
    Object object{};
    object["number"] = Any{double{1.5}};
    const auto result = VajsonToBuffer(object);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"{\"number\":1.5}"});
}
// RFC 8259, section 6 has no representation for infinity or NaN, hence they cannot be serialized.
TEST(VajsonSerializeTest, RejectsInfiniteDouble)
{
    Object object{};
    object["number"] = Any{std::numeric_limits<double>::infinity()};
    EXPECT_FALSE(VajsonToBuffer(object).has_value());
}
TEST(VajsonSerializeTest, RejectsNegativeInfiniteFloat)
{
    Object object{};
    object["number"] = Any{-std::numeric_limits<float>::infinity()};
    EXPECT_FALSE(VajsonToBuffer(object).has_value());
}
TEST(VajsonSerializeTest, RejectsNotANumber)
{
    List list{};
    list.emplace_back(Any{std::numeric_limits<double>::quiet_NaN()});
    EXPECT_FALSE(VajsonToBuffer(list).has_value());
}
TEST(VajsonSerializeTest, RejectsNotANumberOnStream)
{
    Object object{};
    object["number"] = Any{std::numeric_limits<double>::quiet_NaN()};
    std::ostringstream out_stream{};
    VajsonSerialize serializer{out_stream};
    const auto result = serializer << object;
    EXPECT_FALSE(result.has_value());
    // The non-representable number itself must not have been written.
    EXPECT_EQ(out_stream.str().find("nan"), std::string::npos);
}
TEST(VajsonSerializeTest, SerializesTopLevelList)
{
    List list{};
    list.emplace_back(Any{std::uint8_t{5U}});
    list.emplace_back(Any{std::string{"value"}});
    const auto result = VajsonToBuffer(list);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, std::string{"[5,\"value\"]"});
}
}  // namespace
}  // namespace json
}  // namespace score
