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
#include "score/filesystem/file_utils/file_utils_fake.h"

#include <gmock/gmock.h>

#include <iomanip>

using ::testing::_;
using ::testing::An;
using ::testing::ByMove;
using ::testing::Invoke;
using ::testing::Return;

namespace score
{
namespace filesystem
{

FileUtilsFake::FileUtilsFake(IStandardFilesystem& standard_filesystem, IFileFactory& file_factory) noexcept
    : file_utils_{standard_filesystem, file_factory}
{
    // Most of the FileUtils APIs are realized with POSIX calls. These are not easily implemented for a fake filesystem.
    // Hence, we just return errors.
    const auto error = MakeUnexpected(ErrorCode::kNotImplemented);

    ON_CALL(*this, ChangeGroup(_, An<const std::string&>())).WillByDefault(Return(error));
    ON_CALL(*this, ChangeGroup(_, An<gid_t>())).WillByDefault(Return(error));
    ON_CALL(*this, CreateDirectory(_, _)).WillByDefault(Invoke(&file_utils_, &FileUtils::CreateDirectory));
    ON_CALL(*this, CreateDirectories(_, _)).WillByDefault(Invoke(&file_utils_, &FileUtils::CreateDirectories));
    ON_CALL(*this, OpenUniqueFile(_, _)).WillByDefault(Return(ByMove(error)));
    ON_CALL(*this, FileContentsAreIdentical(_, _))
        .WillByDefault(Invoke(&file_utils_, &FileUtils::FileContentsAreIdentical));
    ON_CALL(*this, SyncDirectory(_)).WillByDefault(Return(error));
    ON_CALL(*this, ValidateGroup(_, _)).WillByDefault(Return(error));
}

}  // namespace filesystem
}  // namespace score
