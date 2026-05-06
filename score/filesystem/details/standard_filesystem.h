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
#ifndef SCORE_LIB_FILESYSTEM_DETAILS_STANDARD_FILESYSTEM_H
#define SCORE_LIB_FILESYSTEM_DETAILS_STANDARD_FILESYSTEM_H

#include "score/filesystem/i_standard_filesystem.h"

namespace score
{
namespace filesystem
{

class StandardFilesystem final : public IStandardFilesystem
{
  public:
    Result<Path> Absolute(const Path& path) const noexcept override;
    Result<Path> Canonical(const Path& path) const noexcept override;
    Result<bool> Exists(const Path& path) const noexcept override;
    Result<void> CopyFile(const Path& from, const Path& to) const noexcept override;
    Result<void> CopyFile(const Path& from, const Path& to, const CopyOptions copy_option) const noexcept override;
    Result<void> CreateDirectory(const Path& path) const noexcept override;
    Result<void> CreateDirectories(const Path& path) const noexcept override;
    Result<FileTime> LastWriteTime(const Path&) const noexcept override;
    Result<void> Permissions(const Path& path, const Perms permissions) const noexcept override;
    Result<void> Permissions(const Path& path,
                             const Perms permissions,
                             const PermOptions options) const noexcept override;
    Result<Path> ReadSymlink(const Path& path) const noexcept override;
    Result<void> Remove(const Path& path) const noexcept override;
    Result<void> RemoveAll(const Path& path) const noexcept override;
    Result<FileStatus> Status(const Path&) const noexcept override;
    Result<FileStatus> SymlinkStatus(const Path&) const noexcept override;
    Result<Path> TempDirectoryPath() const noexcept override;
    Result<Path> WeaklyCanonical(const Path& path) const noexcept override;
    Result<Path> CurrentPath() const noexcept override;
    Result<void> CurrentPath(const Path& path) const noexcept override;
    Result<void> CreateHardLink(const Path& oldpath, const Path& newpath) const noexcept override;
    Result<void> CreateSymlink(const Path& target, const Path& linkpath) const noexcept override;
    Result<void> CreateDirectorySymlink(const Path& target, const Path& linkpath) const noexcept override;
    Result<void> CopySymlink(const Path& from, const Path& dest) const noexcept override;
    Result<uint64_t> HardLinkCount(const Path& path) const noexcept override;
    Result<bool> IsDirectory(const Path& path) const noexcept override;
    Result<bool> IsRegularFile(const Path& path) const noexcept override;
    Result<bool> IsSymlink(const Path& path) const noexcept override;
    Result<bool> IsEmpty(const Path& path) const noexcept override;

  private:
    Result<void> RemoveContentFromExistingDirectory(const Path& path) const noexcept;
    Result<void> CopyKnownFileType(const Path& from,
                                   const Path& dest,
                                   const CopyOptions copy_option,
                                   const Result<FileStatus>& dest_status) const noexcept;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_DETAILS_STANDARD_FILESYSTEM_H
