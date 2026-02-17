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
#include "score/json/internal/model/any.h"

#include "gtest/gtest.h"

#include <cstdint>

namespace score
{
namespace json
{
namespace
{

TEST(Any, CanDefaultConstruct)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Empty-constructed json objects have null value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with no specific value
    Any unit{};

    // When trying to access the value
    const auto value = unit.As<Null>();

    // Then it is a score::json::Null
    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanConstructFromBool)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Bool-constructed json objects have bool value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "1");

    // Given Any with a specific value
    Any unit{true};

    // When trying to access the value
    const auto value = unit.As<bool>();

    // Then it equals the previous stored one.
    EXPECT_EQ(value.value(), true);
}

TEST(Any, CanConstructFromFloat)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Float-constructed json objects have float value, cf. RFC-8259 section 3, 6, and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with a specific value
    Any unit{4.2F};

    // When trying to access the value
    const auto value = unit.As<float>();

    // Then it equals the previous stored one.
    EXPECT_EQ(value.value(), 4.2F);
}

TEST(Any, CanConstructFromFloatNumber)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Float-number-constructed json objects have float value, cf. RFC-8259 section 3, 6, and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with a specific value
    Any unit{Number{4.2F}};

    // When trying to access the value
    const auto value = unit.As<float>();

    // Then it equals the previous stored one.
    EXPECT_EQ(value.value(), 4.2F);
}

TEST(Any, CanConstructFromUint64)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Int64-constructed json objects have int64 value, cf. RFC-8259 section 3, 6, and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with a specific value
    Any unit{std::uint64_t{42}};

    // When trying to access the value
    const auto value = unit.As<std::uint64_t>();

    // Then it equals the previous stored one.
    EXPECT_EQ(value.value(), std::uint64_t{42});
}

TEST(Any, CanConstructFromIntegralNumber)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Integral-number-constructed json objects have int value, cf. RFC-8259 section 3, 6, and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with a specific value
    Any unit{Number{42UL}};

    // When trying to access the value
    const auto value = unit.As<uint64_t>();

    // Then it equals the previous stored one.
    EXPECT_EQ(value.value(), std::uint64_t{42});
}

TEST(Any, CanConstructFromString)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "String-constructed json objects have string value, cf. RFC-8259 section 3, 7 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{std::string{"foo"}};

    const auto value = unit.As<std::string>();

    EXPECT_EQ(value.value().get(), std::string{"foo"});
}

TEST(Any, CanConstructFromNull)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Null-constructed json objects have null value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{Null{}};

    const auto value = unit.As<Null>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanConstructFromObject)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Object-constructed json objects have object value, cf. RFC-8259 section 3, 4 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{Object{}};

    const auto value = unit.As<Object>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanConstructFromList)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "List-constructed json objects have list value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{List{}};

    const auto value = unit.As<List>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanAssignBool)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Bool-assigned json objects have bool value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    // Given Any with no specific value
    Any unit{};

    // to which we assign a boolean
    unit = true;

    // When trying to access the value
    const auto value = unit.As<bool>();

    // Then it equals the assigned one.
    EXPECT_EQ(value.value(), true);
}

TEST(Any, CanAssignFloat)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Float-assigned json objects have float value, cf. RFC-8259 section 3, 6 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = 4.2F;

    const auto value = unit.As<float>();

    EXPECT_EQ(value.value(), 4.2F);
}

TEST(Any, CanAssignFloatNumber)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Float-number-assigned json objects have float value, cf. RFC-8259 section 3, 6 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = Number{4.2F};

    const auto value = unit.As<float>();

    EXPECT_EQ(value.value(), 4.2F);
}

TEST(Any, CanAssignUint64)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Int64-assigned json objects have int64 value, cf. RFC-8259 section 3, 6 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = std::uint64_t{42};

    const auto value = unit.As<std::uint64_t>();

    EXPECT_EQ(value.value(), std::uint64_t{42});
}

TEST(Any, CanAssignUint64Number)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Int64-number-assigned json objects have int64 value, cf. RFC-8259 section 3, 6 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = Number{42UL};

    const auto value = unit.As<std::uint64_t>();

    EXPECT_EQ(value.value(), std::uint64_t{42});
}

TEST(Any, CanAssignString)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "String-assigned json objects have string value, cf. RFC-8259 section 3, 7 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = std::string{"foo"};

    const auto value = unit.As<std::string>();

    EXPECT_EQ(value.value().get(), std::string{"foo"});
}

TEST(Any, CanAssignNull)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Null-assigned json objects have null value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{true};

    unit = Null{};

    const auto value = unit.As<Null>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanAssignObject)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Object-assigned json objects have object value, cf. RFC-8259 section 3, 4 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = Object{};

    const auto value = unit.As<Object>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanAssignList)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "List-assigned json objects have list value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");
    Any unit{};

    unit = List{};

    const auto value = unit.As<List>();

    EXPECT_TRUE(value.has_value());
}

TEST(Any, CanRetrieveNumberForBool)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Bool-constructed json objects have numerical value, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    Any unit{false};

    EXPECT_EQ(unit.As<bool>().value(), false);

    const auto value = unit.As<std::uint64_t>();

    EXPECT_EQ(value.value(), std::uint64_t{0});
}

TEST(Any, CanNotRetrieveWronglyTypedValue)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "list json object can't be converted to string, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    // Given Any with a specific value
    Any unit{List{}};

    // When trying to get the value as a different type
    const auto value = unit.As<std::string>();

    // Then no value is returned, rather the respective error
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), Error::kWrongType);
}

TEST(Any, CanNotRetrieveWronglyTypedReference)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "bool json object can't be converted to list, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    // Given Any with a specific value
    Any unit{bool{}};

    // When trying to get the value as a List
    const auto value = unit.As<List>();

    // Then no value is returned, rather the respective error
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), Error::kWrongType);
}

TEST(Any, CanNotRetrieveWronglyTypedValueConst)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "const list json object can't be converted to const bool object, cf. RFC-8259 section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    // Given Any with a specific value
    const Any unit{List{}};

    // When trying to get the value as a different type
    const auto value = unit.As<bool>();

    // Then no value is returned, rather the respective error
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), Error::kWrongType);
}

TEST(Any, CanNotRetrieveWronglyTypedReferenceConst)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "const bool json object can't be converted to const list object, cf. RFC-8259 "
                   "section 3 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    // Given Any with a specific value
    const Any unit{bool{}};

    // When trying to get the value as a List
    const auto value = unit.As<List>();

    // Then no value is returned, rather the respective error
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), Error::kWrongType);
}

TEST(Any, CanAccessStringAsStdStringView)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "String-constructed json objects can be accessed as string_view, cf. RFC-8259 section 3, 7 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    Any unit{std::string{"foo"}};

    const auto value = unit.As<std::string_view>();

    EXPECT_TRUE(value.value() == std::string_view{"foo"});
}

TEST(Any, CanNotRetrieveWronglyTypedStringView)
{
    RecordProperty("Verifies", "5310867");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "non String-constructed json objects can not be accessed as string_view, cf. RFC-8259 section 3, 7 and 9");
    RecordProperty("TestType", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    RecordProperty("Priority", "3");

    // Given Any with a specific value
    Any unit{List{}};

    // When trying to get the value as a different type
    const auto value = unit.As<std::string_view>();

    // Then no value is returned, rather the respective error
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), Error::kWrongType);
}

template <typename T1, typename T2>
void TestEqualOperator(T1 lhs, T2 rhs, bool result)
{
    EXPECT_EQ(json::Any(std::move(lhs)) == json::Any(std::move(rhs)), result);
}

TEST(Any, CheckEqualOperator)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Tests the equal comparator of Any.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("Verifies", "::score::json::Any::operator==");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");
    {
        std::string content = "content";
        std::string wrong_content = "wrong_content";
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, Null{}, false);
    }
    {
        bool content = true;
        bool wrong_content = false;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, Null{}, false);
    }
    {
        int8_t content = -42;
        int8_t wrong_content = -43;
        TestEqualOperator(content, content, true);
        TestEqualOperator(content, wrong_content, false);
        TestEqualOperator(content, Null{}, false);
    }
    {
        TestEqualOperator(Null{}, Null{}, true);
        TestEqualOperator(Null{}, 42, false);
    }
    {
        {
            Object content;
            content.insert(std::make_pair("X", 42));
            content.insert(std::make_pair("Y", 42));
            Object correct_content;
            correct_content.insert(std::make_pair("X", 42));
            correct_content.insert(std::make_pair("Y", 42));
            TestEqualOperator(std::move(content), std::move(correct_content), true);
        }
        {
            Object content;
            content.insert(std::make_pair("X", 42));
            content.insert(std::make_pair("Y", 42));
            Object wrong_content;
            wrong_content.insert(std::make_pair("X", 42));
            wrong_content.insert(std::make_pair("Y", 43));
            TestEqualOperator(std::move(content), std::move(wrong_content), false);
        }
        TestEqualOperator(Object{}, Null{}, false);
    }
    {
        List wrong_content = List{};
        wrong_content.emplace_back(Any{});
        TestEqualOperator(List{}, List{}, true);
        TestEqualOperator(List{}, std::move(wrong_content), false);
        TestEqualOperator(List{}, Null{}, false);
    }
}

TEST(Any, CloneBool)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves boolean value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    // Given a Any object holing a boolean
    Any unit{true};

    // When cloning the the object
    Any cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

TEST(Any, CloneNumber)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves numeric value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    // Given a Any object holing a number
    Any unit{std::uint16_t{42U}};

    // When cloning the the object
    Any cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

TEST(Any, CloneNull)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves null value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    // Given a Any object holing nothing
    Any unit{};

    // When cloning the the object
    Any cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

TEST(Any, CloneString)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves string value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "2");

    // Given a Any object holing a string
    Any unit{std::string{"foo"}};

    // When cloning the the object
    Any cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

TEST(Any, CloneObject)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves object value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    // Given a Any object holing a json::Object
    Object object;
    object["number_field"] = Any{42U};
    object["boolean_field"] = Any{true};
    Any unit{std::move(object)};

    // When cloning the the object
    auto cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

TEST(Any, CloneList)
{
    RecordProperty("Verifies", "::score::json::Any::CloneByValue");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "CloneByValue preserves list value equality");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of equivalence classes and boundary values");
    RecordProperty("Priority", "3");

    // Given a Any object holing a List
    List list{};
    list.push_back(Any{42U});
    Any unit{List{std::move(list)}};

    // When cloning the the object
    auto cloned_unit = unit.CloneByValue();

    // Then the cloned Any must contain the same value
    EXPECT_EQ(unit, cloned_unit);
}

}  // namespace
}  // namespace json
}  // namespace score
