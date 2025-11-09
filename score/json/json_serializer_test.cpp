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
#include "score/json/json_serializer.h"

#include <visitor/visit.h>
#include <visitor/visit_as_struct.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <charconv>
#include <cstdint>
#include <iomanip>
#include <string>
#include <vector>

namespace completely::different
{

struct CustomSerializable
{
    std::vector<std::uint8_t> data{};

    [[nodiscard]] static ::score::Result<CustomSerializable> FromAny(::score::json::Any any)
    {
        if (auto str_result = any.As<std::string>())
        {
            const auto& str = str_result->get();
            std::vector<std::uint8_t> result(str.size() / 2, 0);
            for (std::size_t pos = 0; pos < str.size() / 2; ++pos)
            {
                const auto conversion_result = std::from_chars(&str[pos * 2], &str[(pos + 1) * 2], result[pos], 16);
                if (std::make_error_code(conversion_result.ec))
                {
                    return ::score::MakeUnexpected(::score::json::Error::kParsingError, "Expected hex string");
                }
            }
            return {{result}};
        }
        else
        {
            return ::score::MakeUnexpected(::score::json::Error::kParsingError, "Expected hex string");
        }
    }

    [[nodiscard]] ::score::json::Any ToAny() const
    {
        std::ostringstream ostr{};
        for (const auto byte : data)
        {
            ostr << std::hex << std::setw(2) << std::setfill('0') << static_cast<std::uint32_t>(byte);
        }
        return ::score::json::Any{std::move(ostr).str()};
    }
};

}  // namespace completely::different

namespace score::json::test
{

// This one cannot be inside the unnamed namespace as we do not use the STRUCT_VISITABLE facility. This, however, makes
// clang-tidy complain since we now have unused declarations. Outside the unnamed namespace, the compiler cannot
// determine whether they're used and this silences clang-tidy.
struct VisitableTypeWithCustomSerialization
{
    std::uint32_t integer_val{};
    std::string string_val{};

    [[nodiscard]] Any ToAny() const
    {
        // Custom serialization function for VisitableTypeWithCustomSerialization
        Object obj;
        obj["foo"] = Any{integer_val};
        obj["bar"] = string_val;
        return Any{std::move(obj)};
    }
};

STRUCT_VISITABLE(VisitableTypeWithCustomSerialization, integer_val, string_val)

namespace
{

struct NestedType
{
    std::uint8_t nested_int{};
    bool nested_bool{};
    std::vector<std::uint8_t> nested_array{};
};

STRUCT_VISITABLE(NestedType, nested_int, nested_bool, nested_array)

struct TypeToSerialize
{
    std::uint32_t integer_val{};
    std::string string_val{};
    NestedType nested_val{};
};

STRUCT_VISITABLE(TypeToSerialize, integer_val, string_val, nested_val)

TEST(JsonSerializerTest, TestSerialization)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::ToJsonAny");
    RecordProperty("Description", "Test the serialization of a visitable structure and all its contents");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given an instance of a serializable struct
    TypeToSerialize unit{42, "Blubb", NestedType{43, true, {44, 45}}};

    // Wne serialized to JSON
    auto serialized = ToJsonAny(unit);

    // Then it contains all attributes from the struct, set to the values the instance had before serialization.
    auto obj_result = serialized.As<Object>();
    ASSERT_TRUE(obj_result.has_value());

    auto& obj = obj_result->get();
    ASSERT_EQ(obj.count("integer_val"), 1);
    auto num = obj.at("integer_val").As<std::uint32_t>();
    ASSERT_TRUE(num.has_value());
    EXPECT_EQ(*num, 42U);

    ASSERT_EQ(obj.count("string_val"), 1);
    auto str = obj.at("string_val").As<std::string>();
    ASSERT_TRUE(str.has_value());
    EXPECT_EQ(str->get(), "Blubb");

    ASSERT_EQ(obj.count("nested_val"), 1);
    auto nested_result = obj.at("nested_val").As<Object>();
    ASSERT_TRUE(nested_result.has_value());

    auto nested_int_result = nested_result->get().at("nested_int").As<std::uint32_t>();
    ASSERT_TRUE(nested_int_result.has_value());
    EXPECT_EQ(*nested_int_result, 43);

    auto nested_bool_result = nested_result->get().at("nested_bool").As<bool>();
    ASSERT_TRUE(nested_bool_result.has_value());
    EXPECT_EQ(nested_bool_result.value(), true);

    auto nested_array_result = nested_result->get().at("nested_array").As<List>();
    ASSERT_TRUE(nested_array_result.has_value());
    std::array<std::uint8_t, 2> expected_values{44, 45};
    auto expected_iter = expected_values.begin();
    for (auto& entry : nested_array_result->get())
    {
        auto val = entry.As<std::uint8_t>();
        ASSERT_TRUE(val.has_value());
        EXPECT_EQ(*expected_iter++, *val);
    }
}

TEST(JsonSerializerTest, TestDeserialization)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description", "Test the deserialization of a visitable structure and all its contents");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON that matches the structure of a serializable struct
    auto source = R"(
{
    "integer_val": 42,
    "string_val": "Blubb",
    "nested_val": {
            "nested_int": 43,
            "nested_bool": true,
            "nested_array": [44,45]
    }
}
)"_json;

    // When deserializing the JSON into the struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then no error occurred and all attributes are set to the values from the JSON
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit->integer_val, 42);
    EXPECT_EQ(unit->string_val, "Blubb");
    EXPECT_EQ(unit->nested_val.nested_int, 43);
    EXPECT_EQ(unit->nested_val.nested_bool, true);
    EXPECT_THAT(unit->nested_val.nested_array, ::testing::ElementsAre(44, 45));
}

TEST(JsonSerializerTest, TestFailingDeserialization)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Check whether the deserialization of a JSON that does not match a visitable structure fails with "
                   "the correct error code");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON that does not match the structure of a serializable struct
    auto source = R"(
{
    "integer_val": 42,
    "string_val": ["Blubb"],
    "nested_val": {
            "nested_int": 43,
            "nested_bool": true,
            "nested_array": [44,45]
    }
}
)"_json;

    // When deserializing the JSON into the struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then an error occurs
    EXPECT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

struct TypeWithCustomSerializable
{
    ::completely::different::CustomSerializable custom_type;
};

STRUCT_VISITABLE(TypeWithCustomSerializable, custom_type)

TEST(JsonSerializerTest, UserProvidedDeserialization)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Test that the deserialization of a struct works that defines a custom deserialization scheme");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON that matches the structure of a serializable struct with a custom serializable type
    auto source = R"(
{
    "custom_type": "1337"
}
)"_json;

    // When deserializing the JSON into the struct
    auto result = FromJsonAny<TypeWithCustomSerializable>(std::move(source));

    // Then no error occurred and the custom type is set to the value from the JSON
    ASSERT_TRUE(result.has_value());
    EXPECT_THAT(result->custom_type.data, ::testing::ElementsAre(0x13, 0x37));
}

TEST(JsonSerializerTest, UserProvidedSerialization)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::ToJsonAny");
    RecordProperty("Description",
                   "Test that the serialization of a struct works that defines a custom serialization scheme");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given an instance of a serializable struct with a custom serializable type
    TypeWithCustomSerializable source{{{0x01, 0x03, 0x05, 0x07, 0x11}}};

    // When serialized to JSON
    const auto result = ToJsonAny(std::move(source));

    // Then the custom type is serialized to the expected value
    auto obj = result.As<Object>();
    ASSERT_TRUE(obj.has_value());
    const auto& field = obj->get().at("custom_type");
    auto str = field.As<std::string>();
    ASSERT_TRUE(str.has_value());
    EXPECT_EQ(str->get(), "0103050711");
}

struct TypeWithOptionalValue
{
    std::uint32_t mandatory_val{};
    std::optional<std::uint32_t> never_ever_val{};
    std::optional<std::uint32_t> optional_val{};
};

STRUCT_VISITABLE(TypeWithOptionalValue, mandatory_val, never_ever_val, optional_val)

TEST(JsonSerializerTest, DeserializeOptionalFields)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty(
        "Description",
        "Test the deserialization of a visitable structure that contains an optional attribute. In case this attribute "
        "has a value in the JSON, the deserialization should work in a transparent way");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON that matches the structure of a serializable struct with an optional field
    auto source = R"(
{
    "mandatory_val": 42,
    "optional_val": 43
}
)"_json;

    // When deserializing the JSON into the struct
    auto unit{FromJsonAny<TypeWithOptionalValue>(std::move(source))};

    // Then no error occurred and all attributes are set to the values from the JSON
    EXPECT_TRUE(unit.has_value());
    EXPECT_EQ(unit->mandatory_val, 42);
    ASSERT_TRUE(unit->optional_val.has_value());
    EXPECT_EQ(unit->optional_val.value(), 43);
}

TEST(JsonSerializerTest, NoErrorOnMissingOptionalFields)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty(
        "Description",
        "Test the deserialization of a visitable structure with an optional attribute where "
        "there is no data in the JSON. In this case, deserialization should work and the attribute shall be set to "
        "nullopt");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "1");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON that matches the structure of a serializable struct with an optional field and the optional field
    // isn't set in the JSON
    auto source = R"(
{
    "mandatory_val": 42
}
)"_json;

    // When deserializing the JSON into the struct
    auto unit{FromJsonAny<TypeWithOptionalValue>(std::move(source))};

    // Then no error occurred and the optional field is empty
    EXPECT_TRUE(unit.has_value());
    EXPECT_FALSE(unit->optional_val);
    EXPECT_EQ(unit->mandatory_val, 42);
}

TEST(JsonSerializerTest, ErrorOnMissingMandatoryFields)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty(
        "Description",
        "Test the deserialization of a visitable structure with a mandatory field where the JSON does not have any "
        "data for it. In this case, deserialization should fail with the correct error code");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON that matches the structure of a serializable struct with a mandatory field and the mandatory field
    // is not set in the JSON
    auto source = R"(
{
    "optional_val": 43
}
)"_json;

    // When deserializing the JSON into the struct
    auto unit{FromJsonAny<TypeWithOptionalValue>(std::move(source))};

    // Then a KeyNotFound error occurs
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kKeyNotFound);
}

TEST(JsonSerializerTest, SerializingStructWithUnusedOptionalDoesntEmitField)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::ToJsonAny");
    RecordProperty(
        "Description",
        "Test the serialization of a visitable structure with an optional attribute. In case the optional attribute is "
        "set to nullopt, the respective JSON attribute shall not be set at all");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given an instance of a serializable struct with an optional field that is not set
    TypeWithOptionalValue source{42, std::nullopt, std::nullopt};

    // When serialized to JSON
    auto unit{ToJsonAny(source)};

    // Then the optional field is not present in the JSON
    auto object = unit.As<Object>();
    ASSERT_TRUE(object.has_value());
    EXPECT_EQ(0, object->get().count("optional_val"));
}

TEST(JsonSerializerTest, DeserializingStructFromNonObjectFails)
{
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("Description",
                   "Verify that deserializing a struct does not work if the JSON is not an object but a number");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON that is not an object
    auto source = R"(42)"_json;

    // When trying to deserialize the JSON into a struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then deserialization will fail with a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, FailToDeserializeWrongNumberType)
{
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty(
        "Description",
        "Verify that deserializing an attribute of an integer type fails if the JSON instead contains a float number");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON with a float number inside a field that expects an integer
    auto source = R"({
"integer_val": 42.3,
"string_val": "Blah",
"nested_val": {
    "nested_int": 43,
    "nested_bool": true,
    "nested_array": [44, 45]
    }
} )"_json;

    // When trying to deserilalize the JSON into a struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, FailToDeserializeANonBooleanTypeToBool)
{
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("Description",
                   "Verify that deserializing an attribute of a bool type fails if the JSON instead contains a string");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON with a float number inside a field that expects an integer
    auto source = R"({
"integer_val": 42,
"string_val": "Blah",
"nested_val": {
    "nested_int": 43,
    "nested_bool": "string",
    "nested_array": [44, 45]
    }
} )"_json;

    // When trying to deserilalize the JSON into a struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, FailToDeserializeANonNumberTypeToAnInteger)
{
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("Description",
                   "Verify that deserializing an attribute of an integer type fails if the JSON instead contains a "
                   "string");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON with a string inside a field that expects an integer
    auto source = R"({
"integer_val": "Not a number!",
"string_val": "Blah",
"nested_val": {
    "nested_int": 43,
    "nested_bool": true,
    "nested_array": [44, 45]
    }
} )"_json;

    // When trying to deserilalize the JSON into a struct
    auto unit{FromJsonAny<TypeToSerialize>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, FailToDeserializeToVectorIfJSONIsNotAList)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Verify that deserializing a vector will fail if the JSON does not contain an array as the value "
                   "for a given key");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON that is not a list
    auto source = R"("Not a list")"_json;

    // When trying to deserialize the JSON into a vector
    auto unit{FromJsonAny<std::vector<std::uint8_t>>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, FailToDeserializeIntoAVectorIfJSONListHasMixedTypes)
{
    RecordProperty("TestType", "Verification of the control flow and data flow");
    RecordProperty("Description",
                   "Verify that deserializing a vector of numbers won't work in case the JSON list contains other "
                   "entries than integers");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON list that contains a string and a number
    auto source = R"(["Not a number", 42])"_json;

    // When trying to deserialize the JSON into a vector
    auto unit{FromJsonAny<std::vector<std::uint8_t>>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, SucceedDeserializeListIntoVectorIfListConsistsOfSameType)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Verify that deserializing a vector of numbers works if the JSON list contains only integers");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON list that contains only numbers
    auto source = R"([42, 43, 44])"_json;

    // When trying to deserialize the JSON into a vector
    auto unit{FromJsonAny<std::vector<std::uint8_t>>(std::move(source))};

    // Then this works and the vector contains the numbers from the JSON
    ASSERT_TRUE(unit.has_value());
    EXPECT_THAT(*unit, ::testing::ElementsAre(42, 43, 44));
}

TEST(JsonSerializerTest, SuceedDeserializeListIntoVectorOfAnyEvenOnMixedTypes)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Verify that deserializing a vector of Any works even if the JSON list contains mixed types");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a JSON list that contains a string and a number
    auto source = R"(["Not a number", 42])"_json;

    // When trying to deserialize the JSON into a vector of Any
    auto unit{FromJsonAny<std::vector<::score::json::Any>>(std::move(source))};

    // Then this works and the vector contains the Any objects from the JSON
    ASSERT_TRUE(unit.has_value());
    ASSERT_EQ(unit->size(), 2);
    auto str = unit->at(0).As<std::string>();
    ASSERT_TRUE(str.has_value());
    EXPECT_EQ(str->get(), "Not a number");
    auto num = unit->at(1).As<std::uint32_t>();
    ASSERT_TRUE(num.has_value());
    EXPECT_EQ(num.value(), 42);
}

TEST(JsonSerializerTest, SerializeOptionalValuesIfTheyContainAValue)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::ToJsonAny");
    RecordProperty("Description", "Verify that optional values are added as JSON object keys if they contain a value");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given an instance of a serializable struct with an optional field that is set
    TypeWithOptionalValue source{42, std::nullopt, 43};

    // When serialized to JSON
    auto unit{ToJsonAny(source)};

    // Then the optional field is present in the JSON
    auto object = unit.As<Object>();
    ASSERT_TRUE(object.has_value());
    EXPECT_EQ(1, object->get().count("optional_val"));
    auto val = object->get().at("optional_val").As<std::uint32_t>();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(43, *val);
}

TEST(JsonSerializerTest, FailToDeserializeOptionalValueIfEnclosedTypesDoNotMatch)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::FromJsonAny");
    RecordProperty("Description",
                   "Verify that deserializing an optional value fails if the enclosed type does not match");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "3");
    RecordProperty("DerivationTechnique", "Error guessing");

    // Given a JSON that contains a string where an optional integer is expected
    auto source = R"({"mandatory_val": 42, "optional_val": [43]})"_json;

    // When deserializing to a struct
    auto unit{FromJsonAny<TypeWithOptionalValue>(std::move(source))};

    // Then this fails, returning a WrongType error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error(), Error::kWrongType);
}

TEST(JsonSerializerTest, SerializeConstantObjects)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::ToJsonAny");
    RecordProperty("Description", "Verify that constant objects can be serialized");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given a constant object
    const TypeToSerialize source{42, "Blubb", NestedType{43, false, {44, 45}}};

    // When serialized to JSON
    auto unit{ToJsonAny(source)};

    // Then the serialization worked and all the values correspond to the values of the constant object.
    auto object = unit.As<Object>();
    ASSERT_TRUE(object.has_value());
    EXPECT_EQ(3, object->get().size());
    const auto integer_val = object->get().at("integer_val").As<std::uint32_t>();
    ASSERT_TRUE(integer_val.has_value());
    EXPECT_EQ(42, integer_val.value());
    const auto string_val = object->get().at("string_val").As<std::string>();
    ASSERT_TRUE(string_val.has_value());
    EXPECT_EQ("Blubb", string_val->get());
    const auto nested_val = object->get().at("nested_val").As<Object>();
    ASSERT_TRUE(nested_val.has_value());
    const auto nested_int = nested_val->get().at("nested_int").As<std::uint32_t>();
    ASSERT_TRUE(nested_int.has_value());
    EXPECT_EQ(43, nested_int.value());
    const auto nested_bool = nested_val->get().at("nested_bool").As<bool>();
    ASSERT_TRUE(nested_bool.has_value());
    EXPECT_EQ(nested_bool, false);
    const auto nested_array = nested_val->get().at("nested_array").As<List>();
    ASSERT_TRUE(nested_array.has_value());
    EXPECT_EQ(2, nested_array->get().size());
    const auto first = nested_array->get().at(0).As<std::uint8_t>();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(44, first.value());
    const auto second = nested_array->get().at(1).As<std::uint8_t>();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(45, second.value());
}

TEST(JsonSerializerTest, UseCustomSerializationOnVisitableStruct)
{
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "JsonSerializer");
    RecordProperty("Description",
                   "This test verifies that the custom serialization function of a struct is used, even if the struct "
                   "is visitable using STRUCT_VISITABLE.");
    RecordProperty("ASIL", "QM");
    RecordProperty("Priority", "2");
    RecordProperty("DerivationTechnique", "Analyzing architecture and design");

    // Given an instance of a visitable struct with a custom serialization function
    VisitableTypeWithCustomSerialization unit{1, "234"};
    // When serialized to JSON
    auto serialized = ToJsonAny(unit);
    // Then the custom serialization function is used and the JSON is built as the custom function defines it.
    ASSERT_TRUE(serialized.As<Object>().has_value());
    EXPECT_EQ(serialized.As<Object>()->get().count("foo"), 1);
}

}  // namespace
}  // namespace score::json::test
