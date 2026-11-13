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

#include <sstream>
#include <string>
#include <string_view>

#include "score/utils/string_registry/registry_string_view.h"
#include "score/utils/string_registry/string_registry.h"

namespace score::utils::string_registry
{

TEST(RegistryStringViewTest, Iterator)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "Check that Begin/End and CBegin/CEnd iterators traverse the registered string character by character.");

    StringRegistry registry;
    const char* data = "test_string";
    RegistryStringView string_view = registry.Insert(std::string(data)).first;

    /// Verify the existence of the type score::utils::string_registry::RegistryStringView interface.
    /// Verify the existence of the type score::utils::string_registry::RegistryStringView interface.
    /// Iterators to the beginning provide correct results.
    /// Iterators to the the end provide correct results.
    auto i = 0;
    for (auto it = string_view.Begin(); it != string_view.End(); ++it, ++i)
    {
        EXPECT_EQ(*it, data[i]);
    }

    /// Const iterators to the beginning provide correct results.
    /// Const iterators to the end provide correct results.
    i = 0;
    for (auto it = string_view.CBegin(); it != string_view.CEnd(); ++it, ++i)
    {
        EXPECT_EQ(*it, data[i]);
    }
}

TEST(RegistryStringViewTest, ConsistentHash)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that std::hash on a RegistryStringView returns the same value on repeated calls.");

    StringRegistry registry;
    RegistryStringView string_view = registry.Insert(std::string("test_string")).first;
    auto hash_fn = std::hash<RegistryStringView>{};

    size_t hash1 = hash_fn(string_view);
    size_t hash2 = hash_fn(string_view);

    /// Hash method on a RegistryStringView is consistent for the same string.
    EXPECT_EQ(hash1, hash2);
}

/// same string value.
TEST(RegistryStringViewTest, DifferentRegistryStringViewDifferentHashes)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that equal strings stored in different registries produce different hash values due to "
                   "pointer-based hashing.");

    StringRegistry registry1;
    RegistryStringView string_view1 = registry1.Insert(std::string("test_string")).first;
    StringRegistry registry2;
    RegistryStringView string_view2 = registry2.Insert(std::string("test_string")).first;

    auto hash_fn = std::hash<RegistryStringView>{};
    size_t hash1 = hash_fn(string_view1);
    size_t hash2 = hash_fn(string_view2);

    // Since the std:hash return value is based on memory location, we get different hash values
    // for the same string in different RegistryStringView objects.
    EXPECT_NE(hash1, hash2);
}

TEST(RegistryStringViewTest, StreamOutput)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "Check that the ostream operator<< writes the correct string content.");

    StringRegistry registry;
    RegistryStringView string_view = registry.Insert(std::string("test_string")).first;
    std::ostringstream oss;

    // Use the overloaded << operator to write the view to the output stream
    oss << string_view;

    /// Ostream operator works.
    // Check that the output matches the expected string
    EXPECT_EQ(oss.str(), "test_string");
}

TEST(RegistryStringViewTest, GlobalCompareOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that operator== uses pointer equality and that relational operators (<, <=, >, >=) compare "
                   "by string content.");

    StringRegistry registry;
    RegistryStringView string_view1 = registry.Insert(std::string("test_string")).first;
    RegistryStringView string_view2 = registry.Insert(std::string("test_string2")).first;
    StringRegistry registry2;
    RegistryStringView string_view3 = registry2.Insert(std::string("test_string")).first;

    /// Strings added to the registry from two different memory locations compare as same.
    EXPECT_FALSE(string_view1 == string_view3);

    /// String added twice to the registry from the same memory locations compare as same.
    EXPECT_TRUE(string_view1 == string_view1);
    /// Different strings added to the registry compare as different.
    EXPECT_TRUE(string_view1 != string_view2);

    /// Less than operator works correctly on two RegistryStringViews.
    EXPECT_TRUE(string_view1 < string_view2);
    EXPECT_FALSE(string_view2 < string_view1);

    /// Less than or equal to operator works correctly on two RegistryStringViews.
    EXPECT_TRUE(string_view1 <= string_view2);
    EXPECT_TRUE(string_view1 <= string_view1);
    EXPECT_FALSE(string_view2 <= string_view1);

    /// Greater than operator works correctly on two RegistryStringViews.
    EXPECT_TRUE(string_view2 > string_view1);
    EXPECT_FALSE(string_view1 > string_view2);

    /// Greater than or equal to operator works correctly on two RegistryStringViews.
    EXPECT_TRUE(string_view2 >= string_view1);
    EXPECT_TRUE(string_view1 >= string_view1);
    EXPECT_FALSE(string_view1 >= string_view2);
}

TEST(RegistryStringViewTest, StringLength)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that Length() and Size() return the correct character count of the registered string.");

    StringRegistry registry;
    RegistryStringView string_view = registry.Insert(std::string("test_string")).first;
    auto length = std::string("test_string").length();

    /// Length() returns the length of the added string.
    /// Size() returns the length of the added string.
    EXPECT_EQ(string_view.Length(), length);
    EXPECT_EQ(string_view.Size(), length);
}

TEST(RegistryStringViewTest, StringEmpty)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that Empty() returns true for an empty registered string and false for a non-empty one.");

    StringRegistry registry;
    RegistryStringView empty_string_view = registry.Insert(std::string("")).first;
    RegistryStringView non_empty_string_view = registry.Insert(std::string("non-empty string")).first;

    /// Empty() returns true for an empty string added to the registry and false for a non-empty
    /// one.
    EXPECT_TRUE(empty_string_view.Empty());
    EXPECT_FALSE(non_empty_string_view.Empty());
}

TEST(RegistryStringViewTest, Data)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "Check that Data() returns the same pointer as the underlying string_view.");

    StringRegistry registry;
    RegistryStringView string_view = registry.Insert(std::string("test_string")).first;

    /// Data() returns the const pointer to the string data.
    EXPECT_TRUE((std::is_same<std::remove_cv_t<std::remove_reference_t<decltype(string_view.StringView())>>,
                              std::string_view>::value));
    EXPECT_EQ(string_view.Data(), string_view.StringView().data());
}

TEST(RegistryStringViewTest, MoveConstructor)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "Check that move construction transfers the string_view data to the new object.");

    StringRegistry registry;
    {
        RegistryStringView original_string_view = registry.Insert(std::string("test_string")).first;

        /// Move construct a new RegistryStringView.
        RegistryStringView moved_string_view(std::move(original_string_view));

        /// The moved-to object should have the same data as the original.
        EXPECT_EQ(moved_string_view.StringView(), std::string_view("test_string"));
    }

    /// Destroy RegistryStringView.
    /// RegistryStringView instances runs out-of-scope so the destructor is called.
}

TEST(RegistryStringViewTest, MoveAssignmentOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that move assignment replaces the target's string_view data with the source's.");

    StringRegistry registry;
    RegistryStringView original_string_view = registry.Insert(std::string("test_string")).first;
    RegistryStringView another_string_view = registry.Insert(std::string("another_string")).first;

    /// Move assign the original string view to another string view.
    another_string_view = std::move(original_string_view);

    /// The moved-to object should have the same data as the original.
    EXPECT_EQ(another_string_view.StringView(), std::string_view("test_string"));
}

TEST(RegistryStringViewTest, CopyConstructor)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "Check that copy construction produces a view with identical string_view data.");

    StringRegistry registry;
    RegistryStringView original_string_view = registry.Insert(std::string("test_string")).first;

    /// Copy construct a new RegistryStringView.
    RegistryStringView copied_string_view(original_string_view);

    /// The copied object has the same data as the original.
    EXPECT_EQ(copied_string_view.StringView(), std::string_view("test_string"));
}

TEST(RegistryStringViewTest, CopyAssignmentOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__registry_string_view");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that copy assignment replaces the target's string_view data with the source's.");

    StringRegistry registry;
    RegistryStringView original_string_view = registry.Insert(std::string("test_string")).first;
    RegistryStringView another_string_view = registry.Insert(std::string("another_string")).first;

    /// Copy assign the original string view to another string view
    another_string_view = original_string_view;

    /// The assigned object has the same data as the original.
    EXPECT_EQ(another_string_view.StringView(), std::string_view("test_string"));
}

}  // namespace score::utils::string_registry
