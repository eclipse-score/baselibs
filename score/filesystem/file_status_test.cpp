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
#include "score/filesystem/file_status.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FileStatus, DefaultConstructedAsNone)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.DefaultConstructedAsNone' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus unit{};

    EXPECT_EQ(unit.Type(), FileType::kNone);
    EXPECT_EQ(unit.Permissions(), Perms::kUnknown);
}

TEST(FileStatus, ConstructWithTypeAndPerms)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description",
                   "Check 'FileStatus.ConstructWithTypeAndPerms' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanCopyConstruct)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.CanCopyConstruct' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    const FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit{other};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanMoveConstruct)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.CanMoveConstruct' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit{std::move(other)};

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanCopyAssign)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.CanCopyAssign' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    const FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit = other;

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, CanMoveAssign)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.CanMoveAssign' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus other{FileType::kRegular, Perms::kReadOthers};

    FileStatus unit = std::move(other);

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, SetType)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.SetType' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    unit.Type(FileType::kDirectory);

    EXPECT_EQ(unit.Type(), FileType::kDirectory);
    EXPECT_EQ(unit.Permissions(), Perms::kReadOthers);
}

TEST(FileStatus, SetPermissions)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.SetPermissions' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};

    unit.Permissions(Perms::kWriteOthers);

    EXPECT_EQ(unit.Type(), FileType::kRegular);
    EXPECT_EQ(unit.Permissions(), Perms::kWriteOthers);
}

TEST(FileStatus, EqualOperator)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check 'FileStatus.EqualOperator' behavior for the filesystem component.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FileStatus unit{FileType::kRegular, Perms::kReadOthers};
    FileStatus same{FileType::kRegular, Perms::kReadOthers};
    FileStatus other_file_type{FileType::kSocket, Perms::kReadOthers};
    FileStatus other_permission{FileType::kRegular, Perms::kWriteOthers};

    EXPECT_TRUE(unit == same);
    EXPECT_FALSE(unit == other_file_type);
    EXPECT_FALSE(unit == other_permission);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
