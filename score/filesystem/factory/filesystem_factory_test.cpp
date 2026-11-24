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
#include "score/filesystem/factory/filesystem_factory.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FilesystemFactoryTest, InstanceResolution)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__api_abstraction");
    RecordProperty("Description", "Check that the production factory creates non-null filesystem interfaces.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    FilesystemFactory unit;

    const Filesystem& fs = unit.CreateInstance();

    EXPECT_NE(fs.standard.get(), nullptr);
    EXPECT_NE(fs.utils.get(), nullptr);
    EXPECT_NE(fs.streams.get(), nullptr);
}

}  // namespace
}  // namespace filesystem
}  // namespace score
