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
#include "standard_filesystem_fake.h"

#include <score/assert.hpp>

namespace score
{
namespace filesystem
{

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

StandardFilesystemFake::StandardFilesystemFake()
{
    ON_CALL(*this, Exists(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeExists));
    ON_CALL(*this, CopyFile(_, _, _)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeCopyFile));
    ON_CALL(*this, CreateDirectory(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeCreateDirectory));
    ON_CALL(*this, CreateDirectories(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeCreateDirectories));
    ON_CALL(*this, LastWriteTime(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeLastWriteTime));
    ON_CALL(*this, Permissions(_, _, _)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakePermissions));
    ON_CALL(*this, Remove(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeRemove));
    ON_CALL(*this, RemoveAll(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeRemoveAll));
    ON_CALL(*this, Status(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeStatus));
    ON_CALL(*this, TempDirectoryPath()).WillByDefault(::testing::Return(Result<Path>{"/tmp"}));
    ON_CALL(*this, WeaklyCanonical(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeWeaklyCanonical));
    ON_CALL(*this, CurrentPath()).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeCurrentPath));
    ON_CALL(*this, CurrentPath(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeSetCurrentPath));
    ON_CALL(*this, CreateHardLink(_, _)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeCreateHardLink));
    ON_CALL(*this, HardLinkCount(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeHardLinkCount));
    ON_CALL(*this, IsDirectory(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeIsDirectory));
    ON_CALL(*this, IsRegularFile(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeIsRegularFile));
    ON_CALL(*this, IsSymlink(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeIsSymlink));
    ON_CALL(*this, IsEmpty(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeIsEmpty));
    ON_CALL(*this, Absolute(_)).WillByDefault(Invoke(this, &StandardFilesystemFake::FakeAbsolute));

    // non-implemented methods returns error. will be reworked at Ticket-55932
    ON_CALL(*this, Canonical(_))
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kCouldNotGetRealPath, "Not implemented")));
    ON_CALL(*this, CreateSymlink(_, _))
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kCouldNotCreateSymlink, "Not implemented")));
    ON_CALL(*this, ReadSymlink(_))
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kCouldNotReadSymlink, "Not implemented")));
    ON_CALL(*this, CreateDirectorySymlink(_, _))
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kCouldNotCreateSymlink, "Not implemented")));
    ON_CALL(*this, CopySymlink(_, _))
        .WillByDefault(Return(MakeUnexpected(ErrorCode::kCouldNotCreateSymlink, "Not implemented")));
    // Ticket-55932, todo: remove ON_CALL
    // Since symbolic links are not supported in StandardFilesystemFake,
    // and the RecursiveDirectoryIterator now uses SymlinkStatus(),
    // the SymlinkStatus() calls have been temporarily replaced by Status() calls.
    ON_CALL(*this, SymlinkStatus(_)).WillByDefault(Invoke([this](const Path& path) {
        return this->Status(path);
    }));

    ResetRootDirectory();

    dirent_fake_mock_ = std::make_unique<os::MockGuard<DirentFake>>(*this);
}

ResultBlank StandardFilesystemFake::CreateRegularFile(const Path& path, const Perms permissions) const noexcept
{
    const auto new_entry = CreateEntry(path, FileType::kRegular, permissions);
    if (!new_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateFile);
    }
    return {};
}

void StandardFilesystemFake::ResetRootDirectory() noexcept
{
    root_ = std::make_shared<Entry>(FileType::kDirectory, default_permissions_);
}

Result<std::list<std::string>> StandardFilesystemFake::GetFileList(const Path& path) const noexcept
{
    const auto& entry = FindEntry(path);
    if (!entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kParentDirectoryDoesNotExist);
    }
    if (!entry.value()->IsTypeDirectory())
    {
        return MakeUnexpected(ErrorCode::kParentDirectoryDoesNotExist);
    }
    return entry.value()->GetDirectory()->FileList();
}

DirentFake& StandardFilesystemFake::GetDirentFake() const noexcept
{
    return **dirent_fake_mock_;
}

StandardFilesystemFake::Entry::Entry(const FileType file_type, const Perms permissions)
{
    last_write_time_ = std::chrono::system_clock::now();
    file_status_ = FileStatus(file_type, permissions);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE((file_type == FileType::kRegular) || (file_type == FileType::kDirectory),
                       "Only regular files and directories are supported.");
    if (file_type == FileType::kRegular)
    {
        file_ = std::make_unique<std::stringstream>();
    }
    else
    {
        directory_ = std::make_unique<Directory>();
    }
}

const FileTime& StandardFilesystemFake::Entry::GetLastWriteTime() const noexcept
{
    return last_write_time_;
}

const FileStatus& StandardFilesystemFake::Entry::GetFileStatus() const noexcept
{
    return file_status_;
}

Perms StandardFilesystemFake::Entry::GetPermissions() const noexcept
{
    return file_status_.Permissions();
}

void StandardFilesystemFake::Entry::SetPermissions(const Perms perms) noexcept
{
    file_status_.Permissions(perms);
}

bool StandardFilesystemFake::Entry::IsTypeRegularFile() const noexcept
{
    return file_status_.Type() == FileType::kRegular;
}

bool StandardFilesystemFake::Entry::IsTypeDirectory() const noexcept
{
    return file_status_.Type() == FileType::kDirectory;
}

const std::unique_ptr<std::stringstream>& StandardFilesystemFake::Entry::GetFile() const noexcept
{
    return file_;
}

void StandardFilesystemFake::Entry::SetFile(std::unique_ptr<std::stringstream>&& new_file) noexcept
{
    file_ = std::move(new_file);
}

const std::unique_ptr<StandardFilesystemFake::Directory>& StandardFilesystemFake::Entry::GetDirectory() const noexcept
{
    return directory_;
}

Result<std::shared_ptr<StandardFilesystemFake::Entry>> StandardFilesystemFake::Directory::Find(
    const std::string& name) const noexcept
{
    const auto found_entry_iterator = entries_.find(name);
    if (found_entry_iterator == entries_.end())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
    }
    return found_entry_iterator->second;
}

Result<std::shared_ptr<StandardFilesystemFake::Entry>> StandardFilesystemFake::Directory::Create(
    const std::string& name,
    const FileType file_type,
    const Perms permissions) noexcept
{
    if (entries_.find(name) != entries_.end())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryAlreadyExists);
    }
    entries_[name] = std::make_shared<Entry>(file_type, permissions);
    return entries_[name];
}

ResultBlank StandardFilesystemFake::Directory::Erase(const std::string& name) noexcept
{
    if (entries_.find(name) == entries_.end())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
    }
    entries_.erase(name);
    return {};
}

bool StandardFilesystemFake::Directory::Empty() const noexcept
{
    return entries_.empty();
}

std::list<std::string> StandardFilesystemFake::Directory::FileList() const noexcept
{
    std::list<std::string> list;
    for (const auto& entry : entries_)
    {
        list.emplace_back(entry.first);
    }
    return list;
}

ResultBlank StandardFilesystemFake::Directory::AddHardLink(const std::string& name,
                                                           const std::shared_ptr<Entry>& entry) noexcept
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(entry.get() != nullptr, "The entry for hard link should contain value.");
    if (entries_.find(name) != entries_.end())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryAlreadyExists);
    }
    entries_[name] = entry;
    return {};
}

Result<std::shared_ptr<StandardFilesystemFake::Entry>>
StandardFilesystemFake::CreateEntry(const Path& path, const FileType file_type, const Perms permissions) const noexcept
{
    const auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_path = weakly_canonical_path_result.value();
    const std::string filename = weakly_canonical_path.Filename();
    if (filename.empty())
    {
        if (file_type == FileType::kDirectory)
        {
            return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
        }
        return MakeUnexpected(ErrorCode::kCouldNotCreateFile);
    }
    const auto parent_entry = FindEntry(weakly_canonical_path.ParentPath());
    if (!parent_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kParentDirectoryDoesNotExist);
    }
    if (!parent_entry.value()->IsTypeDirectory())
    {
        return MakeUnexpected(ErrorCode::kParentDirectoryDoesNotExist);
    }
    const auto new_entry = parent_entry.value()->GetDirectory()->Create(filename, file_type, permissions);
    if (!new_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateFile);
    }
    return new_entry.value();
}

Result<std::shared_ptr<StandardFilesystemFake::Entry>> StandardFilesystemFake::FindEntry(
    const Path& path) const noexcept
{
    if (path.Empty())
    {
        return MakeUnexpected(ErrorCode::kInvalidPath);
    }
    const auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_path = weakly_canonical_path_result.value();
    std::shared_ptr<Entry> current_entry = root_;
    auto path_iterator = weakly_canonical_path.begin();
    ++path_iterator;  // skip root folder name ("/")
    for (; (path_iterator != weakly_canonical_path.end()) && (!path_iterator->Empty()); ++path_iterator)
    {
        if (!current_entry->IsTypeDirectory())
        {
            return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
        }
        const auto next_entry = current_entry->GetDirectory()->Find(path_iterator->Native());
        if (!next_entry.has_value())
        {
            return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
        }
        current_entry = next_entry.value();
    }
    return current_entry;
}

Result<bool> StandardFilesystemFake::FakeExists(const Path& path) const noexcept
{
    return FindEntry(path).has_value();
}

ResultBlank StandardFilesystemFake::FakeCreateDirectory(const Path& path) const noexcept
{
    const auto entry = FindEntry(path);
    if (entry.has_value())
    {
        if (entry.value()->IsTypeDirectory())
        {
            return {};
        }
        else
        {
            return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Path exists but is not a directory");
        }
    }

    const auto new_entry = CreateEntry(path, FileType::kDirectory, default_permissions_);
    if (!new_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
    }
    return {};
}

ResultBlank StandardFilesystemFake::FakeCreateDirectories(const Path& path) const noexcept
{
    const auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_path = weakly_canonical_path_result.value();
    std::shared_ptr<Entry> current_entry = root_;
    auto path_iterator = weakly_canonical_path.begin();
    ++path_iterator;  // skip root folder name ("/")
    for (; path_iterator != weakly_canonical_path.end(); ++path_iterator)
    {
        if (!current_entry->IsTypeDirectory())
        {
            return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
        }
        auto next_entry = current_entry->GetDirectory()->Find(path_iterator->Native());
        if (!next_entry.has_value())
        {
            next_entry = current_entry->GetDirectory()->Create(
                path_iterator->Native(), FileType::kDirectory, default_permissions_);
        }
        current_entry = next_entry.value();
    }
    return {};
}

Result<FileTime> StandardFilesystemFake::FakeLastWriteTime(const Path& path) const noexcept
{
    auto entry = FindEntry(path);
    if (!entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
    }
    return entry.value()->GetLastWriteTime();
}

ResultBlank StandardFilesystemFake::CopyFileInternal(const Path& source, const Path& destination) const noexcept
{
    auto source_entry = FindEntry(source);
    const auto dest_exists_result = Exists(destination);
    // The following checks should exist in CopyFile functions. Here we double check it.
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(source_entry.has_value(), "Failed the source file existence check.");
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(source_entry.value()->IsTypeRegularFile(), "Failed the source file type check.");
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(dest_exists_result.has_value(), "Failed the destination file existence check.");
    //
    if (dest_exists_result.value())
    {
        if (!Remove(destination).has_value())
        {
            return MakeUnexpected(ErrorCode::kCopyFailed, "Failed to remove destination file.");
        }
    }
    auto destination_entry = CreateEntry(destination, FileType::kRegular, source_entry.value()->GetPermissions());
    // The following check should be successful in current implementation
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(destination_entry.has_value(), "Could not create destination file.");
    //
    auto file_copy = std::make_unique<std::stringstream>();
    *file_copy << source_entry.value()->GetFile()->rdbuf();
    destination_entry.value()->SetFile(std::move(file_copy));
    return {};
}

ResultBlank StandardFilesystemFake::FakeRemove(const Path& path) const noexcept
{
    auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_path = weakly_canonical_path_result.value();
    auto parent_entry = FindEntry(weakly_canonical_path.ParentPath());
    if (!parent_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    }
    if (!parent_entry.value()->IsTypeDirectory())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    }
    const Path filename = weakly_canonical_path.Filename();
    const auto entry = parent_entry.value()->GetDirectory()->Find(filename);
    if (!entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    }
    const bool is_regular = entry.value()->IsTypeRegularFile();
    const bool is_empty_directory = entry.value()->IsTypeDirectory() && entry.value()->GetDirectory()->Empty();
    if (is_regular || is_empty_directory)
    {
        return parent_entry.value()->GetDirectory()->Erase(filename);
    }
    return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
}

ResultBlank StandardFilesystemFake::FakeRemoveAll(const Path& path) const noexcept
{
    auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_path = weakly_canonical_path_result.value();
    auto parent_entry = FindEntry(weakly_canonical_path.ParentPath());
    if (!parent_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    }
    if (!parent_entry.value()->IsTypeDirectory())
    {
        return MakeUnexpected(ErrorCode::kCouldNotRemoveFileOrDirectory);
    }
    return parent_entry.value()->GetDirectory()->Erase(weakly_canonical_path.Filename());
}

Result<FileStatus> StandardFilesystemFake::FakeStatus(const Path& path) const noexcept
{
    auto entry = FindEntry(path);
    if (!entry.has_value())
    {
        return FileStatus{FileType::kNotFound};
    }
    return entry.value()->GetFileStatus();
}

ResultBlank StandardFilesystemFake::FakePermissions(const Path& path,
                                                    const Perms perms,
                                                    const PermOptions options) const noexcept
{
    auto entry = FindEntry(path);
    if (!entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotChangePermissions);
    }
    std::uint32_t new_perms = 0;
    switch (options)
    {
        case PermOptions::kReplace:
            new_perms = static_cast<std::uint32_t>(perms);
            break;
        case PermOptions::kAdd:
        {
            const std::uint32_t old_perms = static_cast<std::uint32_t>(entry.value()->GetPermissions());
            new_perms = old_perms | static_cast<std::uint32_t>(perms);
        }
        break;
        case PermOptions::kRemove:
        {
            const std::uint32_t old_perms = static_cast<std::uint32_t>(entry.value()->GetPermissions());
            new_perms = old_perms & (~static_cast<std::uint32_t>(perms));
        }
        break;
        case PermOptions::kNoFollow:  // Ticket-55932 todo: rework as in StandardFilesystem::Permissions
        default:
            return MakeUnexpected(ErrorCode::kCouldNotChangePermissions);
            break;
    }
    entry.value()->SetPermissions(static_cast<Perms>(new_perms));
    return {};
}

Result<Path> StandardFilesystemFake::FakeWeaklyCanonical(const Path& path) const noexcept
{
    if (path.Empty())
    {
        return path;
    }
    if (path.IsRelative())
    {
        return (current_directory_ / path).LexicallyNormal();
    }
    return path.LexicallyNormal();
}

Result<Path> StandardFilesystemFake::FakeCurrentPath() const noexcept
{
    return current_directory_;
}

ResultBlank StandardFilesystemFake::FakeSetCurrentPath(const Path& path) noexcept
{
    const auto exists_result = Exists(path);
    if (exists_result.has_value() && exists_result.value())
    {
        const auto weakly_canonical_path_result = FakeWeaklyCanonical(path);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_path_result.has_value(),
                           "In current version FakeWeaklyCanonical() always returns value.");
        current_directory_ = weakly_canonical_path_result.value();
        return {};
    }
    return MakeUnexpected(ErrorCode::kCouldNotSetCurrentPath);
}

score::Result<std::reference_wrapper<std::stringstream>> StandardFilesystemFake::OpenStringStream(
    const Path& path) noexcept
{
    auto entry_result = FindEntry(path);
    if (!entry_result.has_value())
    {
        entry_result = CreateEntry(path, FileType::kRegular, default_permissions_);
        if (!entry_result.has_value())
        {
            return MakeUnexpected(ErrorCode::kCouldNotCreateFile);
        }
    }
    else
    {
        if (!entry_result.value()->IsTypeRegularFile())
        {
            return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
        }
    }
    const auto entry = entry_result.value();
    return *entry->GetFile();
}

ResultBlank StandardFilesystemFake::FakeCreateHardLink(const Path& oldpath, const Path& newpath) noexcept
{
    const auto& oldpath_entry_result = FindEntry(oldpath);
    if (!oldpath_entry_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    const FileType& oldpath_type = oldpath_entry_result.value()->GetFileStatus().Type();
    if (oldpath_type != FileType::kRegular)
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    const auto weakly_canonical_newpath_result = FakeWeaklyCanonical(newpath);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(weakly_canonical_newpath_result.has_value(),
                       "In current version FakeWeaklyCanonical() always returns value.");
    const Path weakly_canonical_newpath = weakly_canonical_newpath_result.value();
    const std::string filename = weakly_canonical_newpath.Filename();
    if (filename.empty())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    const auto parent_entry = FindEntry(weakly_canonical_newpath.ParentPath());
    if (!parent_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    if (!parent_entry.value()->IsTypeDirectory())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    const auto new_entry = parent_entry.value()->GetDirectory()->AddHardLink(filename, oldpath_entry_result.value());
    if (!new_entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateFile);
    }
    return {};
}

Result<uint64_t> StandardFilesystemFake::FakeHardLinkCount(const Path& path) noexcept
{
    const auto& entry = FindEntry(path);
    if (!entry.has_value())
    {
        return MakeUnexpected(ErrorCode::kFileOrDirectoryDoesNotExist);
    }
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(entry.value()->IsTypeRegularFile() || entry.value()->IsTypeDirectory(),
                       "Only regular files and directories are supported.");
    if (entry.value()->IsTypeRegularFile())
    {
        return entry.value().use_count() - 1;
    }
    const auto& directory = entry.value()->GetDirectory();
    uint64_t hard_link_count = 2U;  // starts with 2 due to original directory entry and dot-entry
    for (const auto& name : directory->FileList())
    {
        auto dir_entry = directory->Find(name);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(dir_entry.has_value(), "The entry should exist");
        if (dir_entry.value()->IsTypeDirectory())
        {
            ++hard_link_count;  // +1 for each subdirectory in the original directory
        }
    }
    return hard_link_count;
}

Result<bool> StandardFilesystemFake::FakeIsDirectory(const Path& path) const noexcept
{
    const auto status = Status(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(status.has_value(), "Status always returns value.");
    return status.value().Type() == FileType::kDirectory;
}

Result<bool> StandardFilesystemFake::FakeIsRegularFile(const Path& path) const noexcept
{
    const auto status = Status(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(status.has_value(), "Status always returns value.");
    return status.has_value() && (status.value().Type() == FileType::kRegular);
}

Result<bool> StandardFilesystemFake::FakeIsSymlink(const Path& path) const noexcept
{
    const auto status = Status(path);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_MESSAGE(status.has_value(), "Status always returns value.");
    return status.has_value() && (status.value().Type() == FileType::kSymlink);
}

Result<bool> StandardFilesystemFake::FakeIsEmpty(const Path& path) const noexcept
{
    const auto entry = FindEntry(path);
    if (entry.has_value())
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT(entry.value()->IsTypeDirectory() || entry.value()->IsTypeRegularFile());
        if (entry.value()->IsTypeDirectory())
        {
            return entry.value()->GetDirectory()->FileList().size() == 0;
        }
        if (entry.value()->IsTypeRegularFile())
        {
            return entry.value()->GetFile()->str().size() == 0;
        }
    }
    return false;
}

score::ResultBlank StandardFilesystemFake::FakeCopyFile(const Path& from,
                                                      const Path& dest,
                                                      const CopyOptions copy_option) const noexcept
{
    const auto from_status = Status(from);
    if ((!from_status.has_value()) || (from_status.value().Type() != FileType::kRegular))
    {
        return MakeUnexpected(filesystem::ErrorCode::kFromFileDoesNotExist);
    }

    const auto dest_status = Status(dest);
    if (dest_status.has_value())
    {
        if (dest_status.value().Type() == FileType::kNotFound)
        {
            return CopyFileInternal(from, dest);
        }
        else
        {
            // missing check for if equivalent
            if ((dest_status.value().Type() != FileType::kRegular) || (copy_option == CopyOptions::kNone))
            {
                return MakeUnexpected(filesystem::ErrorCode::kCopyFailed, "Wrong arguments");
            }
            if (copy_option == CopyOptions::kSkipExisting)
            {
                return {};  // do nothing
            }
            else if (copy_option == CopyOptions::kOverwriteExisting)
            {
                return CopyFileInternal(from, dest);
            }
            else if (copy_option == CopyOptions::kUpdateExisting)
            {
                // only copy if from is newer then dest
                const auto from_last_modified = LastWriteTime(from);
                const auto dest_last_modified = LastWriteTime(dest);
                if (from_last_modified.has_value() && dest_last_modified.has_value() &&
                    (from_last_modified.value() > dest_last_modified.value()))
                {
                    return CopyFileInternal(from, dest);
                }
                else
                {
                    return MakeUnexpected(filesystem::ErrorCode::kFileNotCopied);
                }
            }
            else
            {
                return MakeUnexpected(filesystem::ErrorCode::kCopyFailed, "Unknown option");
            }
        }
    }

    return MakeUnexpected(filesystem::ErrorCode::kCouldNotAccessFileDuringCopy, "Destination");
}

Result<Path> StandardFilesystemFake::FakeAbsolute(const Path& path) const noexcept
{
    if (path.IsAbsolute())
    {
        return path;
    }
    const auto native_path = path.Native();
    if (native_path.empty())
    {
        return Path{};
    }
    const Result<Path> current_path_res = CurrentPath();
    if (!current_path_res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath);
    }
    return current_path_res.value() / native_path;
}

}  // namespace filesystem
}  // namespace score
