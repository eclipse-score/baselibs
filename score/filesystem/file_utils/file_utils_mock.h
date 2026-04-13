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
#ifndef SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_MOCK_H
#define SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_MOCK_H

#include "score/filesystem/file_utils/i_file_utils.h"

#include <gmock/gmock.h>

namespace score
{
namespace filesystem
{

class FileUtilsMock : public IFileUtils
{
  public:
    MOCK_METHOD(Result<void>, ChangeGroup, (const Path&, const std::string&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, ChangeGroup, (const Path&, const gid_t), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CheckFileSystem, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, RepairFileSystem, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateDirectory, (const Path&, const score::os::Stat::Mode), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateDirectories, (const Path&, const score::os::Stat::Mode), (const, noexcept, override));
    MOCK_METHOD((Result<std::pair<std::unique_ptr<std::iostream>, Path>>),
                OpenUniqueFile,
                (const Path&, std::ios_base::openmode),
                (const, noexcept, override));
    MOCK_METHOD(Result<bool>, FileContentsAreIdentical, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, FormatPartition, (const Path&), (const, noexcept, override));
    MOCK_METHOD(bool, IsQnxCompatible, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, SyncDirectory, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, ValidateGroup, (const Path&, const std::string&), (const, noexcept, override));
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_MOCK_H
