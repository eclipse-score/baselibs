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
#ifndef SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_FAKE_H
#define SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_FAKE_H

#include "score/filesystem/details/standard_filesystem.h"
#include "score/filesystem/error.h"
#include "score/filesystem/filestream/i_string_stream_collection.h"
#include "score/filesystem/iterator/dirent_fake.h"
#include "score/filesystem/standard_filesystem_mock.h"

#include <gmock/gmock.h>

#include <list>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace score
{
namespace filesystem
{

/// @brief StandardFilesystemFake is a fake object, simplified version of StandardFilesystem.
///
/// StandardFilesystemFake is implemented using the in-memory in-app file system.
/// Implemented support for directories, regular files and hard links.
/// Symbolic links and other file types are not supported.
/// Permissions can be set and obtained, but do not affect file operations.
/// The implemented interface IStringStreamCollection allows to interact with FileFactoryFake
/// (a fake object for file factory).
class StandardFilesystemFake final : public StandardFilesystemMock, public IStringStreamCollection
{
  public:
    /// @brief Constructs a fake filesystem object: initializes an empty root folder and sets the current path.
    StandardFilesystemFake();

    /// @brief Creates a regular file using the specified path and permissions. The parent folder should exist.
    /// @note If there is no error, returns an empty result, otherwise returns the error details.
    Result<void> CreateRegularFile(const Path& path, const Perms permissions) const noexcept;

    /// @brief Initializes an empty root folder.
    void ResetRootDirectory() noexcept;

    /// @brief Returns a list of files for the directory specified in the path if there is no error,
    ///        otherwise returns the error details.
    /// @note dot ('.') and dot-dot ('..') are not included into list of files.
    Result<std::list<std::string>> GetFileList(const Path& path) const noexcept;

    /// @brief Returns the reference to DirentFake object
    DirentFake& GetDirentFake() const noexcept;

  private:
    // helper classes
    class Directory;

    /// @brief Entry represents the record for in-memory filesystem.
    ///
    /// Keeps the file attributes: file type, permissions, last write type and the content
    /// (data for regular file, file list for directory).
    /// @note Since it is an implementation detail (and shall not be used by any occurrence outside),
    /// we hide it as inner-class.
    class Entry
    {
      public:
        Entry(const FileType file_type, const Perms permissions);
        const FileTime& GetLastWriteTime() const noexcept;
        const FileStatus& GetFileStatus() const noexcept;
        Perms GetPermissions() const noexcept;
        void SetPermissions(const Perms perms) noexcept;
        bool IsTypeRegularFile() const noexcept;
        bool IsTypeDirectory() const noexcept;
        const std::unique_ptr<std::stringstream>& GetFile() const noexcept;
        void SetFile(std::unique_ptr<std::stringstream>&& new_file) noexcept;
        const std::unique_ptr<Directory>& GetDirectory() const noexcept;

      private:
        FileTime last_write_time_;
        FileStatus file_status_;
        std::unique_ptr<std::stringstream> file_;
        std::unique_ptr<Directory> directory_;
    };

    /// @brief Directory contains a list of files and provides methods for manipulation.
    ///
    /// @note Since it is an implementation detail (and shall not be used by any occurrence outside),
    /// we hide it as inner-class.
    class Directory
    {
      public:
        Result<std::shared_ptr<Entry>> Find(const std::string& name) const noexcept;
        Result<std::shared_ptr<Entry>> Create(const std::string& name,
                                              const FileType file_type,
                                              const Perms permissions) noexcept;
        Result<void> AddHardLink(const std::string& name, const std::shared_ptr<Entry>& entry) noexcept;
        Result<void> Erase(const std::string& name) noexcept;
        bool Empty() const noexcept;
        std::list<std::string> FileList() const noexcept;

      private:
        std::unordered_map<std::string, std::shared_ptr<Entry>> entries_;  // name -> entry
    };

    // helper methods
    Result<std::shared_ptr<Entry>> CreateEntry(const Path& path,
                                               const FileType file_type,
                                               const Perms permissions) const noexcept;
    Result<std::shared_ptr<Entry>> FindEntry(const Path& path) const noexcept;
    Result<void> CopyFileInternal(const Path& source, const Path& destination) const noexcept;

    // fake methods
    Result<Path> FakeAbsolute(const Path& path) const noexcept;
    // Result<Path> FakeCanonical(const Path& path) const noexcept; // Ticket-55932
    Result<bool> FakeExists(const Path& path) const noexcept;
    Result<void> FakeCreateDirectory(const Path& path) const noexcept;
    Result<void> FakeCreateDirectories(const Path& path) const noexcept;
    Result<FileTime> FakeLastWriteTime(const Path& path) const noexcept;
    Result<void> FakeCopyFile(const Path& from, const Path& dest, const CopyOptions copy_option) const noexcept;
    Result<void> FakeRemove(const Path& path) const noexcept;
    Result<void> FakeRemoveAll(const Path& path) const noexcept;
    Result<FileStatus> FakeStatus(const Path& path) const noexcept;
    Result<void> FakePermissions(const Path& path, const Perms perms, const PermOptions options) const noexcept;
    Result<Path> FakeWeaklyCanonical(const Path& path) const noexcept;
    Result<Path> FakeCurrentPath() const noexcept;
    Result<void> FakeSetCurrentPath(const Path& path) noexcept;
    Result<void> FakeCreateHardLink(const Path& oldpath, const Path& newpath) noexcept;
    // Result<void> FakeCreateSymlink(const Path& target, const Path& linkpath) const noexcept; // Ticket-55932
    Result<uint64_t> FakeHardLinkCount(const Path& path) noexcept;
    Result<bool> FakeIsDirectory(const Path& path) const noexcept;
    Result<bool> FakeIsRegularFile(const Path& path) const noexcept;
    Result<bool> FakeIsSymlink(const Path& path) const noexcept;
    Result<bool> FakeIsEmpty(const Path& path) const noexcept;

    // the methods of IStringStreamCollection interface
    score::Result<std::reference_wrapper<std::stringstream>> OpenStringStream(const Path& path) noexcept override;

    // data members
    std::shared_ptr<Entry> root_{};
    Path current_directory_{"/"};
    Perms default_permissions_ =
        Perms::kReadWriteExecUser | Perms::kReadGroup | Perms::kExecGroup | Perms::kReadOthers | Perms::kExecOthers;
    std::unique_ptr<os::MockGuard<DirentFake>> dirent_fake_mock_{};
};

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_STANDARD_FILESYSTEM_FAKE_H
