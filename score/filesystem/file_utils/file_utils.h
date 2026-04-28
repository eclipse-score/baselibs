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
#ifndef SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_H
#define SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_H

#include "score/filesystem/file_utils/i_file_utils.h"
#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/i_standard_filesystem.h"

#include <mutex>

namespace score
{
namespace filesystem
{

/// @brief Production implementation of IFileUtils.
class FileUtils final : public IFileUtils
{
  public:
    FileUtils(IStandardFilesystem& standard_filesystem, IFileFactory& file_factory) noexcept;

    Result<void> ChangeGroup(const Path& path, const std::string& group_name) const noexcept override;
    Result<void> ChangeGroup(const Path& path, const gid_t group_id) const noexcept override;
    Result<void> CreateDirectory(const Path& path, const score::os::Stat::Mode perms) const noexcept override;
    Result<void> CreateDirectories(const Path& path, const score::os::Stat::Mode perms) const noexcept override;
    Result<std::pair<std::unique_ptr<std::iostream>, Path>> OpenUniqueFile(const Path& path,
                                                                           std::ios_base::openmode mode) const override;
    Result<bool> FileContentsAreIdentical(const Path& path1, const Path& path2) const noexcept override;
    Result<void> SyncDirectory(const Path& dirname) const noexcept override;
    Result<bool> ValidateGroup(const Path& path, const std::string& group_name) const noexcept override;

  private:
    Result<bool> FilesExist(const Path& path1, const Path& path2) const noexcept;
    Result<void> CreatePath(const Path& final_path,
                            const Path& cur_path,
                            const score::os::Stat::Mode perms,
                            const std::int64_t creation_delay_ns,
                            const std::int64_t creation_retry_factor,
                            std::uint32_t creation_retry_counter) const noexcept;

    mutable std::mutex groupname_mutex_;

    IStandardFilesystem& standard_filesystem_;
    IFileFactory& file_factory_;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILE_UTILS_FILE_UTILS_H
