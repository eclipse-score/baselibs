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
#include "score/filesystem/iterator/directory_entry.h"

#include <include/gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(DirectoryEntry, CanEqualCompare)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.CanEqualCompare' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a default constructed entry and a path constructed one
    DirectoryEntry unit{};
    DirectoryEntry other{"/foo"};

    // When comparing them, they do not equal
    EXPECT_FALSE(unit == other);
    EXPECT_TRUE(unit != other);
}

TEST(DirectoryEntry, CanEqualCompareCopy)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.CanEqualCompareCopy' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a path constructed entry and a copy constructed one
    DirectoryEntry unit{"/foo"};
    DirectoryEntry other{unit};

    // When comparing them, they are equal
    EXPECT_TRUE(unit == other);
    EXPECT_FALSE(unit != other);
}

TEST(DirectoryEntry, GetPathOnMoveConstructedEntry)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description",
                   "Check 'DirectoryEntry.GetPathOnMoveConstructedEntry' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a path constructed entry that move constructs another one
    DirectoryEntry unit{"/foo"};
    DirectoryEntry other{std::move(unit)};

    // When getting the path, it equals the path constructed one
    EXPECT_EQ(other.GetPath(), Path{"/foo"});
}

TEST(DirectoryEntry, AssignOperators)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.AssignOperators' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a path constructed entry
    DirectoryEntry unit{"/foo"};

    // When copy-assigning it
    DirectoryEntry other = unit;
    // Then it equals the initial entry
    EXPECT_EQ(other, unit);

    // When move assigning it
    DirectoryEntry third = std::move(unit);
    // Then it equals the copied entry
    EXPECT_EQ(third, other);
}

TEST(DirectoryEntry, DirectoryEntryExists)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.DirectoryEntryExists' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a valid directory entry
    DirectoryEntry unit{"/proc"};

    // When checking for the existence
    const auto exists = unit.Exists();

    // Then the path exists
    ASSERT_TRUE(exists.has_value());
    EXPECT_TRUE(exists.value());
}

TEST(DirectoryEntry, DoesNotExist)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.DoesNotExist' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given an invalid path
    DirectoryEntry unit{"/foo/bar"};

    // When checking for the existence
    const auto exists = unit.Exists();

    // Then the path does not exist
    ASSERT_TRUE(exists.has_value());
    EXPECT_FALSE(exists.value());
}

TEST(DirectoryEntry, GetStatus)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__directory_iterators");
    RecordProperty("Description", "Check 'DirectoryEntry.GetStatus' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    // Given a valid entry
    DirectoryEntry unit{"/proc"};

    // When requesting the status of that file
    const auto status = unit.Status();

    // Then it is returned
    EXPECT_TRUE(status.has_value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
