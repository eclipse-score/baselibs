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
#include "score/filesystem/factory/filesystem_factory_mock_objects.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FilesystemFactoryMockObjectsTest, InstanceResolution)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description", "Check that the mock-objects factory provides all injectable filesystem interfaces.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryMockObjects unit;

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_NE(fs.standard.get(), nullptr);
    EXPECT_NE(fs.utils.get(), nullptr);
    EXPECT_NE(fs.streams.get(), nullptr);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForStandard)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__full_testability");
    RecordProperty("Description", "Check that the standard-filesystem interface returns the injected mock instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryMockObjects unit;
    StandardFilesystemMock* mock = &unit.GetStandard();
    IStandardFilesystem* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.standard.get(), expected);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForUtils)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__full_testability");
    RecordProperty("Description", "Check that the file-utils interface returns the injected mock instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryMockObjects unit;
    FileUtilsMock* mock = &unit.GetUtils();
    IFileUtils* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.utils.get(), expected);
}

TEST(FilesystemFactoryMockObjectsTest, SameInstanceForStreams)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__full_testability");
    RecordProperty("Description", "Check that the filestream interface returns the injected mock instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryMockObjects unit;
    FileFactoryMock* mock = &unit.GetStreams();
    IFileFactory* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.streams.get(), expected);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
