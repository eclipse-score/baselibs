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
#include "score/json/internal/writer/json_serialize/json_serialize.h"

#include <gtest/gtest.h>

#include <cfloat>
#include <iostream>

namespace score
{
namespace json
{
namespace
{

template <typename T>
void TestInput(const std::string& input, const T& json)
{
    std::ostringstream stream{};
    JsonSerialize serializer{stream};
    serializer << json;
    EXPECT_EQ(stream.str(), input);
}

TEST(JsonSerializeTest, SerializeString)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with string elements and serializing it, cf. RFC-8259 section 7");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "string1": "foo",
    "string2": "bar"
})"};
    score::json::Object json{};
    json["string1"] = std::string{"foo"};
    json["string2"] = std::string{"bar"};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeStringWithSpecialChars)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with string elements and serializing it, cf. RFC-8259 section 7 "
                   "considering special characters");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string expected_json_string{R"({
    "string1": "String with \"special\" characters like \\",
    "string2": "Normal string"
})"};
    score::json::Object json{};
    json["string1"] = std::string{R"(String with "special" characters like \)"};
    json["string2"] = std::string{"Normal string"};

    TestInput(expected_json_string, json);
}

TEST(JsonSerializeTest, SerializeInt8)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Int8 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(INT8_MIN) +
                           R"(,
    "num2": )" + std::to_string(INT8_MAX) +
                           R"(,
    "num3": 0,
    "num4": 1
})"};
    score::json::Object json{};
    json["num1"] = std::int8_t{INT8_MIN};
    json["num2"] = std::int8_t{INT8_MAX};
    json["num3"] = std::int8_t{0};
    json["num4"] = std::int8_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeInt16)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Int16 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(INT16_MIN) +
                           R"(,
    "num2": )" + std::to_string(INT16_MAX) +
                           R"(,
    "num3": 0,
    "num4": 1
})"};
    score::json::Object json{};
    json["num1"] = std::int16_t{INT16_MIN};
    json["num2"] = std::int16_t{INT16_MAX};
    json["num3"] = std::int16_t{0};
    json["num4"] = std::int16_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeInt32)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Int32 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(INT32_MIN) +
                           R"(,
    "num2": )" + std::to_string(INT32_MAX) +
                           R"(,
    "num3": 0,
    "num4": 1
})"};
    score::json::Object json{};
    json["num1"] = std::int32_t{INT32_MIN};
    json["num2"] = std::int32_t{INT32_MAX};
    json["num3"] = std::int32_t{0};
    json["num4"] = std::int32_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeInt64)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Int64 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(INT64_MIN) +
                           R"(,
    "num2": )" + std::to_string(INT64_MAX) +
                           R"(,
    "num3": 0,
    "num4": 1
})"};
    score::json::Object json{};
    json["num1"] = std::int64_t{INT64_MIN};
    json["num2"] = std::int64_t{INT64_MAX};
    json["num3"] = std::int64_t{0};
    json["num4"] = std::int64_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeUint8)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Uint8 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(UINT8_MAX) +
                           R"(,
    "num2": 0,
    "num3": 1
})"};
    score::json::Object json{};
    json["num1"] = std::uint8_t{UINT8_MAX};
    json["num2"] = std::uint8_t{0};
    json["num3"] = std::uint8_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeUint16)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Uint16 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(UINT16_MAX) +
                           R"(,
    "num2": 0,
    "num3": 1
})"};
    score::json::Object json{};
    json["num1"] = std::uint16_t{UINT16_MAX};
    json["num2"] = std::uint16_t{0};
    json["num3"] = std::uint16_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeUint32)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Uint32 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(UINT32_MAX) +
                           R"(,
    "num2": 0,
    "num3": 1
})"};
    score::json::Object json{};
    json["num1"] = std::uint32_t{UINT32_MAX};
    json["num2"] = std::uint32_t{0};
    json["num3"] = std::uint32_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeUint64)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Uint64 elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "num1": )" + std::to_string(UINT32_MAX) +
                           R"(,
    "num2": 0,
    "num3": 1
})"};
    score::json::Object json{};
    json["num1"] = std::uint32_t{UINT32_MAX};
    json["num2"] = std::uint32_t{0};
    json["num3"] = std::uint32_t{1};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeFloat)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with float elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::ostringstream string_stream{};
    string_stream.precision(std::numeric_limits<float>::max_digits10);
    string_stream << R"({
    "num1": )" << -FLT_MAX;
    string_stream << R"(,
    "num2": )" << FLT_MAX;
    string_stream << R"(,
    "num3": 0,
    "num4": 1
})";
    std::string input_json{string_stream.str()};

    score::json::Object json{};
    json["num1"] = float{-FLT_MAX};
    json["num2"] = float{FLT_MAX};
    json["num3"] = float{0.0f};
    json["num4"] = float{1.0f};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeDouble)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with double elements and serializing it, cf. RFC-8259 section 6");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::ostringstream string_stream{};
    string_stream.precision(std::numeric_limits<double>::max_digits10);
    string_stream << R"({
    "num1": )" << -DBL_MAX;
    string_stream << R"(,
    "num2": )" << DBL_MAX;
    string_stream << R"(,
    "num3": 0,
    "num4": 1
})";
    std::string input_json{string_stream.str()};

    score::json::Object json{};
    json["num1"] = double{-DBL_MAX};
    json["num2"] = double{DBL_MAX};
    json["num3"] = double{0.0f};
    json["num4"] = double{1.0f};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeBool)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with bool elements and serializing it, cf. RFC-8259 section 3");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "bool1": 1,
    "bool2": 0
})"};
    score::json::Object json{};
    json["bool1"] = true;
    json["bool2"] = false;

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeNull)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with Null element and serializing it, cf. RFC-8259 section 3");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
    "null": null
})"};
    score::json::Object json{};
    json["null"] = score::json::Null{};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeMultipleTypes)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with multiple types elements and serializing it, cf. RFC-8259 section 3");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::ostringstream string_stream{};
    string_stream.precision(std::numeric_limits<float>::max_digits10);
    string_stream << R"({
    "float": )" << 3.14f;
    string_stream << R"(,
    "null": null,
    "num": 1,
    "string": "foo"
})";
    std::string input_json{string_stream.str()};

    score::json::Object json{};
    json["num"] = 1;
    json["float"] = 3.14f;
    json["null"] = score::json::Null{};
    json["string"] = std::string{"foo"};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeList)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with list of elements and serializing it, cf. RFC-8259 section 5");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"([
    5,
    "string"
])"};
    score::json::List list{};
    list.emplace_back(score::json::Any{std::uint8_t{5}});
    list.emplace_back(score::json::Any{std::string{"string"}});

    TestInput(input_json, list);
}

TEST(JsonSerializeTest, SerializeEmptyList)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with empty list and serializing it, cf. RFC-8259 section 5");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"([
])"};
    score::json::List list{};

    TestInput(input_json, list);
}

TEST(JsonSerializeTest, SerializeEmptyObject)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "constructing json object with empty object and serializing it, cf. RFC-8259 section 3");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"({
})"};

    score::json::Object json{};

    TestInput(input_json, json);
}

TEST(JsonSerializeTest, SerializeNestedObjectAndList)
{
    RecordProperty("Verifies", "SCR-5310867");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "constructing json object with nested object and list and serializing it, cf. RFC-8259 section 4 and 5");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::string input_json{R"([
    {
        "object1": {
            "object2": {
                "num2": 5,
                "object3": {
                    "num1": 4
                }
            }
        }
    }
])"};
    score::json::Object object3{};
    object3["num1"] = std::uint8_t{4};
    score::json::Object object2{};
    object2["object3"] = std::move(object3);
    object2["num2"] = uint8_t{5};
    score::json::Object object1{};
    object1["object2"] = std::move(object2);
    score::json::Object object{};
    object["object1"] = std::move(object1);

    score::json::List list{};
    list.emplace_back(std::move(object));

    TestInput(input_json, list);
}

TEST(JsonSerializeAnyTest, SerializeAny)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test serializing json::Any with all type variants, cf. RFC-8259 section 3-7");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::pair<std::string, score::json::Any>> v;

    // boolean
    v.emplace_back(std::make_pair("1", score::json::Any{true}));
    // integer
    v.emplace_back(std::make_pair("5", score::json::Any{5}));
    // float
    v.emplace_back(std::make_pair("5.5", score::json::Any{float(5.5f)}));
    // double
    v.emplace_back(std::make_pair("5.5", score::json::Any{double(5.5f)}));
    // null
    v.emplace_back(std::make_pair("null", score::json::Any{score::json::Null{}}));
    // string
    v.emplace_back(std::make_pair(R"("string")", score::json::Any{std::string{"string"}}));

    // object
    score::json::Object json{};
    json["string1"] = std::string{"foo"};
    json["string2"] = std::string{"bar"};

    v.emplace_back(std::make_pair(R"({
    "string1": "foo",
    "string2": "bar"
})",
                                  score::json::Any{std::move(json)}));

    // list
    score::json::List list{};
    list.emplace_back(score::json::Any{std::uint8_t{5}});
    list.emplace_back(score::json::Any{std::string{"string"}});

    v.emplace_back(std::make_pair(R"([
    5,
    "string"
])",
                                  score::json::Any{std::move(list)}));

    for (const auto& i : v)
    {
        TestInput(i.first, i.second);
    }
}

}  // namespace
}  // namespace json
}  // namespace score
