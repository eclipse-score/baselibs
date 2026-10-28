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

#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "score/string_manipulation/string_registry.h"

namespace score::string_manipulation
{

TEST(StringRegistryTest, InsertMultipleStrings)
{
    String_registry registry;

    /// Verify the existence of the type cara::core::util::String_registry interface.
    const auto insert1 = registry.insert(std::string("TestString"));
    /// String from an std::String is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// StringView of the correct string is returned.
    EXPECT_TRUE(insert1.first.data() == std::string("TestString"));

    const auto insert2 = registry.insert(std::string("TestString"));
    /// String from an std::String is not added to the registry when it is already there.
    EXPECT_FALSE(insert2.second);
    /// StringView of the correct string is returned.
    EXPECT_TRUE(insert2.first.data() == std::string("TestString"));
}

TEST(StringRegistryTest, InsertMultipleStringViews)
{
    String_registry registry;
    const std::string_view string_view("TestString");
    const std::string_view second_string_view("TestString");

    const auto insert1 = registry.insert(string_view);
    /// String from a std::string_view is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert1.first.string_view() == string_view);

    const auto insert2 = registry.insert(string_view);
    /// String from a std::string_view is not added to registry when it is already there.
    EXPECT_FALSE(insert2.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert2.first.string_view() == string_view);

    const auto insert3 = registry.insert(second_string_view);
    /// String from a different std::string_view is not added to the registry when an identical
    /// string is already there.
    EXPECT_FALSE(insert3.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert3.first.string_view() == string_view);
}

TEST(StringRegistryTest, InsertMultipleStringViewLiterals)
{
    String_registry registry;
    const std::string_view string_view("TestStringLiteral");
    const std::string_view second_string_view("TestStringLiteral");

    const auto insert1 = registry.insert(string_view, Literal_tag{});
    /// String from a std::string_view literal is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert1.first.string_view() == string_view);

    const auto insert2 = registry.insert(string_view, Literal_tag{});
    /// String from a std::string_view literal is not added to the registry when it is already
    /// there.
    EXPECT_FALSE(insert2.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert2.first.string_view() == string_view);

    const auto insert3 = registry.insert(second_string_view, Literal_tag{});
    /// String from a different instance of std::string_view literal is not added to the registry
    /// when an identical string is already there.
    EXPECT_FALSE(insert3.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert3.first.string_view() == string_view);
}

}  // namespace score::string_manipulation
