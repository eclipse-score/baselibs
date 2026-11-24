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
#include "score/filesystem/file_utils/i_file_utils.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(IFileUtils, InstanceResolution)
{
    RecordProperty("PartiallyVerifies", "comp_req__filesystem__file_utils");
    RecordProperty("Description",
                   "Check that the singleton file utilities interface is available and reports errors "
                   "for invalid input.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "design-analysis");
    auto& utils = IFileUtils::instance();
    const auto file = utils.CreateDirectories("", score::os::Stat::Mode::kNone);

    EXPECT_FALSE(file.has_value());
}

}  // namespace
}  // namespace filesystem
}  // namespace score
