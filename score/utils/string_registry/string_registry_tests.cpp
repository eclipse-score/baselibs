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

#include "score/utils/string_registry/string_registry.h"

namespace score::utils::string_registry
{

TEST(StringRegistryTest, InsertMultipleStrings)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that inserting the same std::string twice returns false on the second insert and the same "
                   "backing storage.");

    StringRegistry registry;

    /// Verify the existence of the type score::utils::string_registry::StringRegistry interface.
    const auto insert1 = registry.Insert(std::string("TestString"));
    /// String from an std::String is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// StringView of the correct string is returned.
    EXPECT_TRUE(insert1.first.Data() == std::string("TestString"));

    const auto insert2 = registry.Insert(std::string("TestString"));
    /// String from an std::String is not added to the registry when it is already there.
    EXPECT_FALSE(insert2.second);
    /// StringView of the correct string is returned.
    EXPECT_TRUE(insert2.first.Data() == std::string("TestString"));
}

TEST(StringRegistryTest, InsertMultipleStringViews)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that inserting equal std::string_views deduplicates to a single backing entry.");

    StringRegistry registry;
    const std::string_view string_view("TestString");
    const std::string_view second_string_view("TestString");

    const auto insert1 = registry.Insert(string_view);
    /// String from a std::string_view is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert1.first.StringView() == string_view);

    const auto insert2 = registry.Insert(string_view);
    /// String from a std::string_view is not added to registry when it is already there.
    EXPECT_FALSE(insert2.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert2.first.StringView() == string_view);

    const auto insert3 = registry.Insert(second_string_view);
    /// String from a different std::string_view is not added to the registry when an identical
    /// string is already there.
    EXPECT_FALSE(insert3.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert3.first.StringView() == string_view);
}

TEST(StringRegistryTest, InsertMultipleStringViewLiterals)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that LiteralTag insertions of equal string_views deduplicate to a single entry without heap "
                   "allocation.");

    StringRegistry registry;
    const std::string_view string_view("TestStringLiteral");
    const std::string_view second_string_view("TestStringLiteral");

    const auto insert1 = registry.Insert(string_view, LiteralTag{});
    /// String from a std::string_view literal is added to the registry when it is not there yet.
    EXPECT_TRUE(insert1.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert1.first.StringView() == string_view);

    const auto insert2 = registry.Insert(string_view, LiteralTag{});
    /// String from a std::string_view literal is not added to the registry when it is already
    /// there.
    EXPECT_FALSE(insert2.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert2.first.StringView() == string_view);

    const auto insert3 = registry.Insert(second_string_view, LiteralTag{});
    /// String from a different instance of std::string_view literal is not added to the registry
    /// when an identical string is already there.
    EXPECT_FALSE(insert3.second);
    /// std::string_view of the correct string is returned.
    EXPECT_TRUE(insert3.first.StringView() == string_view);
}

TEST(StringRegistryTest, EqualStringsFromSeparateAllocationsArePointerIdentical)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that two independently heap-allocated strings with equal content map to the same backing "
                   "pointer after registry insertion.");

    StringRegistry registry;

    // Two independently heap-allocated strings with equal content
    const auto [view1, inserted1] = registry.Insert(std::string{"Deduplicated"});
    EXPECT_TRUE(inserted1);

    const auto [view2, inserted2] = registry.Insert(std::string{"Deduplicated"});
    EXPECT_FALSE(inserted2);

    // Registry contract: equal content must map to identical backing storage
    EXPECT_EQ(view1, view2);
    EXPECT_EQ(view1.Data(), view2.Data());
}

TEST(StringRegistryTest, StringViewThenStringOverloadYieldPointerIdenticalViews)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "Check that inserting equal content via the string_view and string&& overloads returns the same "
                   "backing pointer.");

    StringRegistry registry;

    // First insertion via string_view: allocates a copy in the registry
    const auto [view1, inserted1] = registry.Insert(std::string_view{"CrossOverload"});
    EXPECT_TRUE(inserted1);

    // Second insertion via string&&: must find the existing entry, not create a new allocation
    const auto [view2, inserted2] = registry.Insert(std::string{"CrossOverload"});
    EXPECT_FALSE(inserted2);

    EXPECT_EQ(view1, view2);
    EXPECT_EQ(view1.Data(), view2.Data());
}

TEST(StringRegistryTest, LiteralThenStringOverloadYieldPointerIdenticalViews)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "Check that inserting equal content via LiteralTag and string&& overloads returns the same backing pointer.");

    StringRegistry registry;

    // LiteralTag insertion: stores a view directly into static storage — no heap allocation
    const std::string_view literal{"CrossLiteral"};
    const auto [view1, inserted1] = registry.Insert(literal, LiteralTag{});
    EXPECT_TRUE(inserted1);

    // string&& insertion: must find the existing literal-backed entry
    const auto [view2, inserted2] = registry.Insert(std::string{"CrossLiteral"});
    EXPECT_FALSE(inserted2);

    EXPECT_EQ(view1, view2);
    EXPECT_EQ(view1.Data(), view2.Data());
}

TEST(StringRegistryTest, ViewIdentityIsStableAfterRegistryGrowth)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "Check that the backing pointer of a registered string remains stable after the registry grows and rehashes.");

    StringRegistry registry;

    const auto [view1, inserted1] = registry.Insert(std::string{"Stable"});
    EXPECT_TRUE(inserted1);

    // Grow the registry to trigger potential unordered_set rehashing
    for (int i = 0; i < 128; ++i)
    {
        registry.Insert(std::string{"Growth"} + std::to_string(i));
    }

    // The original entry must still be found and return the same backing storage
    const auto [view2, inserted2] = registry.Insert(std::string{"Stable"});
    EXPECT_FALSE(inserted2);

    EXPECT_EQ(view1, view2);
    EXPECT_EQ(view1.Data(), view2.Data());
}

TEST(StringRegistryTest, SlicedLiteralAndFullStringAreDistinct)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that a sliced string_view literal and the full string are treated as distinct entries.");

    StringRegistry registry;

    // Register only "abc" (first 3 chars of a longer buffer) via LiteralTag
    const auto insert1 = registry.Insert(std::string_view{"abcdef", 3}, LiteralTag{});
    EXPECT_TRUE(insert1.second);
    EXPECT_EQ(insert1.first.StringView(), std::string_view{"abc"});

    // Inserting the full 6-char string must NOT return the existing 3-char entry
    const auto insert2 = registry.Insert(std::string{"abcdef"});
    EXPECT_TRUE(insert2.second);
    EXPECT_EQ(insert2.first.StringView(), std::string_view{"abcdef"});
}

TEST(StringRegistryTest, EmbeddedNulsAreDistinct)
{
    RecordProperty("PartiallyVerifies", "comp_req__utils__string_registry__string_registry");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "Check that strings containing embedded NUL characters are distinct from shorter strings sharing "
                   "the same prefix.");

    StringRegistry registry;

    // Register a 5-char string containing an embedded NUL
    const auto insert1 = registry.Insert(std::string("ab\0cd", 5));
    EXPECT_TRUE(insert1.second);
    EXPECT_EQ(insert1.first.StringView().size(), 5U);

    // "ab" (2 chars) must not match "ab\0cd" (5 chars)
    const auto insert2 = registry.Insert(std::string{"ab"});
    EXPECT_TRUE(insert2.second);
    EXPECT_EQ(insert2.first.StringView().size(), 2U);

    // Re-inserting the same 5-char string must find the existing entry
    const auto insert3 = registry.Insert(std::string("ab\0cd", 5));
    EXPECT_FALSE(insert3.second);
    EXPECT_EQ(insert3.first.StringView().size(), 5U);
}

}  // namespace score::utils::string_registry
