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
#include "score/filesystem/file_utils/file_utils_mock.h"

#include <gtest/gtest.h>

namespace score
{
namespace filesystem
{
namespace
{

TEST(FileUtilsMock, CheckReturnValues)
{
    FileUtilsMock unit;

    EXPECT_EQ(Result<void>{}, unit.ChangeGroup({}, std::string{}));
    EXPECT_EQ(Result<void>{}, unit.ChangeGroup({}, 0));
    EXPECT_EQ(Result<void>{}, unit.CheckFileSystem({}));
    EXPECT_EQ(Result<void>{}, unit.CreateDirectory({}, {}));
    EXPECT_EQ(Result<void>{}, unit.CreateDirectories({}, {}));
    EXPECT_EQ(Result<bool>{}, unit.FileContentsAreIdentical({}, {}));
    EXPECT_EQ(Result<void>{}, unit.FormatPartition({}));
    EXPECT_EQ(bool{}, unit.IsQnxCompatible({}));
    EXPECT_EQ(Result<void>{}, unit.SyncDirectory({}));
    EXPECT_EQ(Result<bool>{}, unit.ValidateGroup({}, {}));
}

}  // namespace
}  // namespace filesystem
}  // namespace score
