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
#ifndef SCORE_LIB_FILESYSTEM_I_STANDARD_FILESYSTEM_H
#define SCORE_LIB_FILESYSTEM_I_STANDARD_FILESYSTEM_H

#include "score/filesystem/file_status.h"
#include "score/filesystem/path.h"

#include "score/os/ObjectSeam.h"
#include "score/result/result.h"

#include <chrono>

namespace score
{
namespace filesystem
{

using FileTime = std::chrono::time_point<std::chrono::system_clock>;

enum class CopyOptions : std::uint16_t
{
    kNone = 0,
    // options controlling CopyFile() when the file already exists
    // kNone - Report an error (default behavior)
    kSkipExisting = 1,
    kOverwriteExisting = 2,
    kUpdateExisting = 4,
    // options controlling the effects of Copy() on subdirectories
    // kNone - Skip subdirectories (default behavior)
    kRecursive = 8,
    // options controlling the effects of Copy() on symbolic links
    // kNone - Follow symlinks (default behavior)
    kCopySymlinks = 16,
    kSkipSymlinks = 32,
    // options controlling the kind of copying Copy() does
    // kNone - Copy file content (default behavior)
    kDirectoriesOnly = 64,
    kCreateSymlinks = 128,
    kCreateHardLinks = 256
};

enum class PermOptions : std::uint32_t
{
    kReplace = 1,
    kAdd = 2,
    kRemove = 4,
    kNoFollow = 8,  // Ticket-55932: not suppported in StandardFilesystemFake
};

bool IsValid(const PermOptions perm_options) noexcept;

/// \brief T
class IStandardFilesystem : public os::ObjectSeam<IStandardFilesystem>
{
  public:
    IStandardFilesystem() = default;
    static IStandardFilesystem& instance() noexcept;

    virtual ~IStandardFilesystem() noexcept;

    IStandardFilesystem(const IStandardFilesystem&) = delete;
    IStandardFilesystem(const IStandardFilesystem&&) noexcept = delete;
    IStandardFilesystem& operator=(const IStandardFilesystem&) = delete;
    IStandardFilesystem& operator=(const IStandardFilesystem&&) noexcept = delete;

    /// \brief Returns an absolute path.
    /// Absolute path is equivalent to std::filesystem::current_path() / p.
    /// For an empty path, returns an empty path.
    virtual Result<Path> Absolute(const Path&) const noexcept = 0;

    /// \brief Returns a canonical absolute path.
    /// Returns an absolute path that has no dot, dot-dot elements or symbolic links.
    /// The path must exist or an error will be returned.
    virtual Result<Path> Canonical(const Path&) const noexcept = 0;

    /// \brief Checks if the given file status or path corresponds to an existing file or directory.
    virtual Result<bool> Exists(const Path&) const noexcept = 0;

    /// \brief Copies a single file from from to to
    virtual Result<void> CopyFile(const Path& from, const Path& to) const noexcept = 0;

    /// \brief Copies a single file from from to to, using the copy options indicated by options. The behavior is
    /// undefined if there is more than one option in any of the copy_options option group present in options (even in
    /// the groups not relevant to CopyFile)
    virtual Result<void> CopyFile(const Path& from, const Path& to, const CopyOptions) const noexcept = 0;

    /// \brief Creates the directory p as if by POSIX mkdir() with a second argument of
    /// static_cast<int>(std::filesystem::perms::all) (the parent directory must already exist). If the function fails
    /// because p resolves to an existing directory, no error is reported. Otherwise on failure an error is reported.
    virtual Result<void> CreateDirectory(const Path&) const noexcept = 0;

    /// \brief  Executes CreateDirectory() for every element of p that does not already exist. If p already exists, the
    /// function does nothing (this condition is not treated as an error).
    virtual Result<void> CreateDirectories(const Path&) const noexcept = 0;

    /// \brief Returns the time of the last modification of p, determined as if by accessing the member st_mtime of the
    /// POSIX stat (symlinks are followed).
    virtual Result<FileTime> LastWriteTime(const Path&) const noexcept = 0;

    /// \brief Changes access permissions of the file to which p resolves.
    /// Symlinks are followed.
    virtual Result<void> Permissions(const Path&, const Perms) const noexcept = 0;

    /// \brief Changes access permissions of the file to which p resolves.
    /// Symlinks are followed unless PermOptions::kNoFollow is set in opts.
    virtual Result<void> Permissions(const Path&, const Perms, const PermOptions) const noexcept = 0;

    /// \brief If the path p refers to a symbolic link, returns a new path object which refers to the target of that
    /// symbolic link. Otherwise and on failure an error is reported.
    virtual Result<Path> ReadSymlink(const Path&) const noexcept = 0;

    /// \brief The file or empty directory identified by the path p is deleted as if by the POSIX remove. Symlinks are
    /// not followed (symlink is removed, not its target)
    virtual Result<void> Remove(const Path&) const noexcept = 0;

    /// \brief  Deletes the contents of p (if it is a directory) and the contents of all its subdirectories,
    /// recursively, then deletes p itself as if by repeatedly applying the POSIX remove. Symlinks are not followed
    /// (symlink is removed, not its target).
    virtual Result<void> RemoveAll(const Path&) const noexcept = 0;

    /// \brief Determines the type and attributes of the filesystem object identified by p as if by POSIX stat (symlinks
    /// are followed to their targets).
    virtual Result<FileStatus> Status(const Path&) const noexcept = 0;

    /// \brief Determines the type and attributes of the filesystem object identified by p as if by POSIX stat
    /// (symlinks are NOT followed to their targets).
    virtual Result<FileStatus> SymlinkStatus(const Path&) const noexcept = 0;

    /// \brief Returns the directory location suitable for temporary files.
    virtual Result<Path> TempDirectoryPath() const noexcept = 0;

    /// \brief Returns the weakly canonical path
    /// (an absolute path that has no dot, dot-dot elements. weakly means that the path may not exist).
    /// See details: https://en.cppreference.com/w/cpp/filesystem/canonical
    virtual Result<Path> WeaklyCanonical(const Path& path) const noexcept = 0;

    /// \brief Returns the current path.
    virtual Result<Path> CurrentPath() const noexcept = 0;

    /// \brief Sets the current path.
    virtual Result<void> CurrentPath(const Path& path) const noexcept = 0;

    /// @brief Creates a hard link.
    virtual Result<void> CreateHardLink(const Path& oldpath, const Path& newpath) const noexcept = 0;

    /// @brief Creates a symbolic link.
    virtual Result<void> CreateSymlink(const Path& target, const Path& linkpath) const noexcept = 0;

    /// @brief Creates a symbolic link to a directory.
    /// Note: Portable code should use CreateDirectorySymlink to create directory symlinks
    /// rather than CreateSymlink, even though there is no distinction on POSIX systems.
    virtual Result<void> CreateDirectorySymlink(const Path& target, const Path& linkpath) const noexcept = 0;

    /// \brief Copies a symlink to another location.
    virtual Result<void> CopySymlink(const Path& from, const Path& dest) const noexcept = 0;

    /// @brief Returns hard link count.
    virtual Result<uint64_t> HardLinkCount(const Path& path) const noexcept = 0;

    /// @brief Checks if the given path corresponds to a directory.
    virtual Result<bool> IsDirectory(const Path& path) const noexcept = 0;

    /// @brief Checks if the given path corresponds to a regular file.
    virtual Result<bool> IsRegularFile(const Path& path) const noexcept = 0;

    /// @brief Checks if the given path corresponds to a regular file.
    virtual Result<bool> IsSymlink(const Path& path) const noexcept = 0;

    /// @brief Checks whether the given path refers to an empty file or directory.
    virtual Result<bool> IsEmpty(const Path& path) const noexcept = 0;
};

}  // namespace filesystem
}  // namespace score

/* KW_SUPPRESS_START:MISRA.NS.GLOBAL: */
/* operator in local namespace will lead to build error due to name lookup algorithm */
// coverity[autosar_cpp14_m7_3_1_violation] see above
inline score::filesystem::PermOptions operator|(const score::filesystem::PermOptions l,
                                              const score::filesystem::PermOptions r) noexcept
{
    using EnumUnderlyingType = std::underlying_type_t<score::filesystem::PermOptions>;
    static_assert(std::is_unsigned_v<EnumUnderlyingType>);
    // check ensures the return value is of underlying type
    // coverity[autosar_cpp14_a7_2_1_violation]
    return static_cast<score::filesystem::PermOptions>(static_cast<EnumUnderlyingType>(l) |
                                                     static_cast<EnumUnderlyingType>(r));
}
/* KW_SUPPRESS_END:MISRA.NS.GLOBAL: */

#endif  // SCORE_LIB_FILESYSTEM_I_STANDARD_FILESYSTEM_H
