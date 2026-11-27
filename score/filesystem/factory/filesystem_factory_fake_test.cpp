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
#include "score/filesystem/factory/filesystem_factory_fake.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FilesystemFactoryFakeTest, InstanceResolution)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description", "Check that the fake factory provides all filesystem interfaces for test use.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryFake unit;

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_NE(fs.standard.get(), nullptr);
    EXPECT_NE(fs.utils.get(), nullptr);
    EXPECT_NE(fs.streams.get(), nullptr);
}

TEST(FilesystemFactoryFakeTest, SameInstanceForStandard)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description", "Check that the standard-filesystem interface returns the fake instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryFake unit;
    StandardFilesystemFake* fake = &unit.GetStandard();
    IStandardFilesystem* expected{fake};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.standard.get(), expected);
}

TEST(FilesystemFactoryFakeTest, SameInstanceForUtils)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description", "Check that the file-utils interface returns the configured mock instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryFake unit;
    FileUtilsMock* mock = &unit.GetUtils();
    IFileUtils* expected{mock};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.utils.get(), expected);
}

TEST(FilesystemFactoryFakeTest, SameInstanceForStreams)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description", "Check that the filestream interface returns the fake instance.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryFake unit;
    FileFactoryFake* fake = &unit.GetStreams();
    IFileFactory* expected{fake};

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_EQ(fs.streams.get(), expected);
}

TEST(FilesystemFactoryFakeTest, StandardAndStreamsAreConnected)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__mock_fake");
    RecordProperty("Description",
                   "Check that fake standard filesystem and fake streams share a consistent backend state.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactoryFake unit;
    ASSERT_TRUE(unit.GetStandard().CreateDirectory("/tmp"));

    const Filesystem& fs = unit.CreateInstance();
    const auto open_result = fs.streams->Open("/tmp/file", std::ios_base::in | std::ios_base::out);
    const auto exists_result = fs.standard->Exists("/tmp/file");

    EXPECT_TRUE(open_result.has_value());
    EXPECT_TRUE(exists_result.has_value());
    EXPECT_TRUE(exists_result.value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
