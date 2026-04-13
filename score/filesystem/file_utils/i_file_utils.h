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
#ifndef SCORE_LIB_FILESYSTEM_FILE_UTILS_I_FILE_UTILS_H
#define SCORE_LIB_FILESYSTEM_FILE_UTILS_I_FILE_UTILS_H

#include "score/filesystem/error.h"
#include "score/filesystem/path.h"
#include "score/os/ObjectSeam.h"
#include "score/os/stat.h"

namespace score
{
namespace filesystem
{

/// @brief Contains helper functions.
class IFileUtils : public os::ObjectSeam<IFileUtils>
{
  public:
    IFileUtils() = default;

    /// @brief Access method for migration purpose to dependency injection
    static IFileUtils& instance() noexcept;

    /// \brief Changes group ownership of the file to group_name.
    /// \param path The path to file
    /// \param group_name Name of the group
    /// \details Also changes the owner of the file to the real user.
    virtual Result<void> ChangeGroup(const Path& path, const std::string& group_name) const noexcept = 0;

    /// \brief Changes group ownership of the file to group_id.
    /// \param path The path to file
    /// \param group_id Id of the group
    /// \details Also changes the owner of the file to the real user.
    virtual Result<void> ChangeGroup(const Path& path, const gid_t group_id) const noexcept = 0;

    /// \brief Check if filesystem is corrupted
    /// \param   partition   device to check (e.g. /dev/mmcblk0p1)
    /// \return  true        if not corrupted
    ///          false       if corrupted
    virtual Result<void> CheckFileSystem(const Path& partition) const noexcept = 0;

    /// \brief Do automatic filesystem repair
    /// \param   partition   device to repair (e.g. /dev/mmcblk0p1)
    /// @return Returns either success @ref Result<void>, or error @ref ErrorCode
    virtual Result<void> RepairFileSystem(const Path& partition) const noexcept = 0;

    /// \brief Creates the directory p as if by POSIX mkdir() with a second argument of perms.
    /// The parent directory must already exist.
    /// If the function fails because p resolves to an existing directory, no error is reported.
    /// Otherwise on failure an error is reported.
    /// \details If the perms parameter is not required then use IStandardFilesystem::CreateDirectory.
    virtual Result<void> CreateDirectory(const Path& path, const score::os::Stat::Mode perms) const noexcept = 0;

    /// \brief Executes CreateDirectory() for every element of p that does not already exist.
    /// If p already exists, the function does nothing (this condition is not treated as an error).
    /// \details If the perms parameter is not required then use IStandardFilesystem::CreateDirectories.
    virtual Result<void> CreateDirectories(const Path& path, const score::os::Stat::Mode perms) const noexcept = 0;

    /// \brief Returns an iostream to a file with a unique filename ending opened with the specified mode.
    virtual Result<std::pair<std::unique_ptr<std::iostream>, Path>> OpenUniqueFile(
        const Path& path,
        std::ios_base::openmode mode) const = 0;

    /// \brief Compares two files by content.
    /// Returns true if the contents of the file are identical, false if file contents are not identical.
    /// \note Returns an error if the file does not exist or cannot be opened.
    virtual Result<bool> FileContentsAreIdentical(const Path& path1, const Path& path2) const noexcept = 0;

    ///  \brief Format partition to EXT2 with QNX compatible settings.
    ///  Large files and inode resize are disabled.
    ///  \param   partition   device to format (e.g. /dev/mmcblk0p1)
    ///  \return  true        if succeeded
    ///           false       if failed
    virtual Result<void> FormatPartition(const Path& partition) const noexcept = 0;

    ///  \brief Check if device filesystem is QNX compatible
    ///  \param   partition   device to check (e.g. /dev/mmcblk0p1)
    ///  \return  true        if compatible
    ///           false       if not compatible
    virtual bool IsQnxCompatible(const Path& partition) const = 0;

    /// \brief Synchronizes directory entries (filenames, inodes, etc.).
    /// Can be used to ensure that a newly created file entry is fully synchronized with disk
    /// \note Does not synchronize file contents. Use other ways to synchronize file content.
    virtual Result<void> SyncDirectory(const Path& dirname) const noexcept = 0;

    /// \brief Verifies if given files identified by @path ownership permission is set to @group_name
    /// \param path The path to file
    /// \param group_name Name of the group
    /// \return True if file @path ownership matches @group_name
    virtual Result<bool> ValidateGroup(const Path& path, const std::string& group_name) const noexcept = 0;

    virtual ~IFileUtils() noexcept;

    IFileUtils(const IFileUtils&) = delete;
    IFileUtils(const IFileUtils&&) noexcept = delete;
    IFileUtils& operator=(const IFileUtils&) = delete;
    IFileUtils& operator=(const IFileUtils&&) noexcept = delete;
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_FILE_UTILS_I_FILE_UTILS_H
