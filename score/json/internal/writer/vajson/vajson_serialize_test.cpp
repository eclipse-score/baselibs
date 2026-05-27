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
              std::string{"{\"boolean\":true,\"list\":[null,{\"number\":7}],\"string\":\"line1\\n\\\"quoted\\\"\\\\line2\"}"});
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
