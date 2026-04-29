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
#ifndef SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_MOCK_H
#define SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_MOCK_H

#include "score/filesystem/i_standard_filesystem.h"

#include <gmock/gmock.h>

namespace score
{
namespace filesystem
{

class StandardFilesystemMock : public IStandardFilesystem
{
  public:
    MOCK_METHOD(Result<Path>, Absolute, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<Path>, Canonical, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, Exists, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CopyFile, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CopyFile, (const Path&, const Path&, const CopyOptions), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateDirectory, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateDirectories, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<FileTime>, LastWriteTime, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, Permissions, (const Path&, const Perms), (const, noexcept, override));
    MOCK_METHOD(Result<void>, Permissions, (const Path&, const Perms, const PermOptions), (const, noexcept, override));
    MOCK_METHOD(Result<Path>, ReadSymlink, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, Remove, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, RemoveAll, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<FileStatus>, Status, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<FileStatus>, SymlinkStatus, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<Path>, TempDirectoryPath, (), (const, noexcept, override));
    MOCK_METHOD(Result<Path>, WeaklyCanonical, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<Path>, CurrentPath, (), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CurrentPath, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateHardLink, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateSymlink, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CreateDirectorySymlink, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<void>, CopySymlink, (const Path&, const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<uint64_t>, HardLinkCount, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, IsDirectory, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, IsRegularFile, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, IsSymlink, (const Path&), (const, noexcept, override));
    MOCK_METHOD(Result<bool>, IsEmpty, (const Path&), (const, noexcept, override));
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_MOCK_H
