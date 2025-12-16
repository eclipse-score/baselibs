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
#include "score/json/internal/model/object.h"

#include "gtest/gtest.h"

#include <functional>
#include <string>

namespace score
{
namespace json
{
namespace
{

class AttributeGettersTest : public ::testing::Test
{
  public:
    AttributeGettersTest() : test_data_{}, test_data_value_{test_data_}, test_data_result_{test_data_value_}
    {
        test_data_.insert(std::make_pair("string", std::string{"Lorem ipsum dolor sit amet"}));
        test_data_.insert(std::make_pair("int", std::uint64_t{42}));
        test_data_.insert(std::make_pair("float", 3.14159265359f));
        test_data_.insert(std::make_pair("bool", true));

        json::List test_list = List{};
        test_list.push_back(json::Any{std::string{"one"}});
        test_list.push_back(json::Any{std::string{"two"}});
        test_list.push_back(json::Any{std::string{"three"}});
        test_data_.insert(std::make_pair("list", std::move(test_list)));

        auto second_level_object = Object{};
        second_level_object.insert(std::make_pair("color", std::string{"blue"}));
        auto third_level_object = Object{};

        third_level_object.insert(std::make_pair("shape", std::string{"rectangle"}));
        auto fourth_level_object = Object{};
        fourth_level_object.insert(std::make_pair("width", std::uint64_t{20}));
        fourth_level_object.insert(std::make_pair("height", std::uint64_t{10}));
        fourth_level_object.insert(std::make_pair("unit", std::string{"px"}));

        third_level_object.insert(std::make_pair("size", std::move(fourth_level_object)));
        second_level_object.insert(std::make_pair("geometry", std::move(third_level_object)));

        test_data_.insert(std::make_pair("widget", std::move(second_level_object)));
    }

  protected:
    json::Object test_data_;
    const std::reference_wrapper<const json::Object> test_data_value_;
    const Result<std::reference_wrapper<const json::Object>> test_data_result_;
};

TEST_F(AttributeGettersTest, GetBoolUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get bool attribute using Result<> input.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    // Test passing Result:
    Result<bool> attr_result = GetAttribute<bool>(test_data_result_, "bool");
    ASSERT_TRUE(attr_result.has_value());
    EXPECT_TRUE(attr_result.value());
}

TEST_F(AttributeGettersTest, GetBoolDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get bool attribute from direct Object reference.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    // Test passing direct value:
    Result<bool> attr_result = GetAttribute<bool>(test_data_value_, "bool");
    ASSERT_TRUE(attr_result.has_value());
    EXPECT_TRUE(attr_result.value());
}

TEST_F(AttributeGettersTest, GetIntegerUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get signed/unsigned integer attribute using Result<> input.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<std::int64_t> attr_result_signed = GetAttribute<std::int64_t>(test_data_result_, "int");
    ASSERT_TRUE(attr_result_signed.has_value());
    EXPECT_EQ(attr_result_signed.value(), 42);

    Result<std::uint64_t> attr_result_unsigned = GetAttribute<std::uint64_t>(test_data_result_, "int");
    ASSERT_TRUE(attr_result_unsigned.has_value());
    EXPECT_EQ(attr_result_unsigned.value(), 42u);
}

TEST_F(AttributeGettersTest, GetIntegerDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get signed/unsigned integer attribute from direct Object reference.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<std::int64_t> attr_result_signed = GetAttribute<std::int64_t>(test_data_value_, "int");
    ASSERT_TRUE(attr_result_signed.has_value());
    EXPECT_EQ(attr_result_signed.value(), 42);

    Result<std::uint64_t> attr_result_unsigned = GetAttribute<std::uint64_t>(test_data_value_, "int");
    ASSERT_TRUE(attr_result_unsigned.has_value());
    EXPECT_EQ(attr_result_unsigned.value(), 42u);
}

TEST_F(AttributeGettersTest, GetFloatingPointUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get float attribute using Result<>; double retrieval is forbidden.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<float> float_result = GetAttribute<float>(test_data_result_, "float");
    ASSERT_TRUE(float_result.has_value());
    EXPECT_TRUE(abs(float_result.value() - 3.14159265359) < 1e-3);

    // Floating-point numbers are always stored as double. Having input as a float
    // and trying to get it as a double throws assertion since this should be unreachable case.
    ASSERT_DEATH(GetAttribute<double>(test_data_result_, "float"), "");
}

TEST_F(AttributeGettersTest, GetFloatingPointDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get float attribute from direct value; double retrieval is forbidden.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<float> float_result = GetAttribute<float>(test_data_value_, "float");
    ASSERT_TRUE(float_result.has_value());
    EXPECT_TRUE(abs(float_result.value() - 3.14159265359) < 1e-3);

    // Floating-point numbers are always stored as double. Having input as a float
    // and trying to get it as a double throws assertion since this should be unreachable case.
    ASSERT_DEATH(GetAttribute<double>(test_data_value_, "float"), "");
}

TEST_F(AttributeGettersTest, GetStringUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get string attribute using Result<> input.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<score::cpp::string_view> str_result = GetAttribute<score::cpp::string_view>(test_data_result_, "string");
    ASSERT_TRUE(str_result.has_value());

    const score::cpp::string_view str_value = str_result.value();
    EXPECT_EQ(std::string(str_value.data()), "Lorem ipsum dolor sit amet");
}

TEST_F(AttributeGettersTest, GetStringDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get string attribute from direct Object reference.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    Result<score::cpp::string_view> str_result = GetAttribute<score::cpp::string_view>(test_data_value_, "string");
    ASSERT_TRUE(str_result.has_value());

    const score::cpp::string_view str_value = str_result.value();
    EXPECT_EQ(std::string(str_value.data()), "Lorem ipsum dolor sit amet");
}

TEST_F(AttributeGettersTest, GetListUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get list attribute using Result<> input.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    auto list_result = GetAttribute<json::List>(test_data_result_, "list");
    ASSERT_TRUE(list_result.has_value());
    const auto& list = list_result.value().get();
    EXPECT_EQ(list[0].As<std::string>().value().get(), "one");
}

TEST_F(AttributeGettersTest, GetListDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get list attribute from direct Object reference.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    auto list_result = GetAttribute<json::List>(test_data_value_, "list");
    ASSERT_TRUE(list_result.has_value());
    const auto& list = list_result.value().get();
    EXPECT_EQ(list[0].As<std::string>().value().get(), "one");
}

TEST_F(AttributeGettersTest, GetObjectUsingResult)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get nested object attribute using Result<> input.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    auto sub_object_result = GetAttribute<json::Object>(test_data_result_, "widget");
    ASSERT_TRUE(sub_object_result.has_value());
    const auto sub_object = sub_object_result.value();
    const score::cpp::string_view color = GetAttribute<score::cpp::string_view>(sub_object, "color").value();
    EXPECT_EQ(std::string(color.data()), "blue");
}

TEST_F(AttributeGettersTest, GetObjectDirectValue)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Get nested object attribute from direct Object reference.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    auto sub_object_result = GetAttribute<json::Object>(test_data_value_, "widget");
    ASSERT_TRUE(sub_object_result.has_value());
    const auto sub_object = sub_object_result.value();
    const score::cpp::string_view color = GetAttribute<score::cpp::string_view>(sub_object, "color").value();
    EXPECT_EQ(std::string(color.data()), "blue");
}

TEST_F(AttributeGettersTest, CascadingAccessSuccessCase)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Traverse nested objects (widget/geometry/size/width) successfully.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    const auto widget = GetAttribute<json::Object>(test_data_value_, "widget");
    ASSERT_TRUE(widget.has_value());

    const auto geometry = GetAttribute<json::Object>(widget, "geometry");
    ASSERT_TRUE(geometry.has_value());

    const auto size = GetAttribute<json::Object>(geometry, "size");
    ASSERT_TRUE(size.has_value());

    const auto width = GetAttribute<std::uint64_t>(size, "width");
    ASSERT_TRUE(width.has_value());
    ASSERT_EQ(width.value(), 20u);
}

TEST_F(AttributeGettersTest, CascadingAccessErrorCase)
{
    RecordProperty("Verifies", "::score::json::GetAttribute");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Fail gracefully when traversing with invalid keys.");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Priority", "3");

    // Invalid key
    const auto widget = GetAttribute<json::Object>(test_data_value_, "bogus");
    EXPECT_FALSE(widget.has_value());

    const auto geometry = GetAttribute<json::Object>(widget, "geometry");
    EXPECT_FALSE(geometry.has_value());

    const auto size = GetAttribute<json::Object>(geometry, "size");
    EXPECT_FALSE(size.has_value());

    const auto width = GetAttribute<std::uint64_t>(size, "width");
    EXPECT_FALSE(widget.has_value());
    ASSERT_EQ(width.error().UserMessage(), "Key was not found on the object");
}

}  // namespace
}  // namespace json
}  // namespace score
