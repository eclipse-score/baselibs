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
#include "score/filesystem/details/standard_filesystem.h"

#include "score/filesystem/error.h"
#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/iterator/directory_iterator.h"
#include "score/filesystem/iterator/recursive_directory_iterator.h"

#include "score/os/stat.h"
#include "score/os/stdio.h"
#include "score/os/stdlib.h"
#include "score/os/unistd.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <stack>

// Remove after reproduction (Ticket-243648)
#include <iostream>

namespace score
{
namespace filesystem
{

namespace
{

// Suppress "AUTOSAR C++14 A15-5-3" rule finding. This rule states: "The std::terminate() function shall
// not be called implicitly". Since path_.has_value() is checked before calling path_.value(),
// std::bad_optional_access should never be thrown. This is false positive.
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
score::ResultBlank CopyFileInternal(const Path& source, const Path& destination) noexcept
{
    auto source_file = IFileFactory::instance().Open(source, std::ios::binary | ::std::ios::in);
    auto destination_file = IFileFactory::instance().Open(destination, std::ios::binary | ::std::ios::out);

    if (!source_file.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotAccessFileDuringCopy, "Source");
    }

    if (!destination_file.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotAccessFileDuringCopy, "Dest");
    }

    *destination_file.value() << source_file.value()->rdbuf();

    if (destination_file.value()->bad() || source_file.value()->bad())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCopyFailed);
    }

    os::StatBuffer buffer{};
    const auto status = score::os::Stat::instance().stat(source.CStr(), buffer, true);
    if (!status.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotAccessFileDuringCopy, "Source");
    }

    const auto chmod_result =
        score::os::Stat::instance().chmod(destination.CStr(), score::os::IntegerToMode(buffer.st_mode));

    if (!chmod_result.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotSetPermissions);
    }

    return {};
}

// Suppressed complexity warning as this method directly maps file types using well-defined macros,
// ensuring explicit and clear handling of all cases. Refactoring would introduce unnecessary
// abstractions, reducing maintainability and clarity without functional benefits.
// SCORE_CCM_NO_LINT
Result<FileStatus> StatusInternal(const Path& path, const bool resolve_symlinks)
{
    os::StatBuffer buffer{};

    const auto result = os::Stat::instance().stat(path.CStr(), buffer, resolve_symlinks);

    if (!result.has_value())
    {
        // LCOV_EXCL_BR_START caused by exception in result.error(), but here result.has_value()==false
        if (result.error() == score::os::Error::Code::kNoSuchFileOrDirectory)
        // LCOV_EXCL_BR_STOP
        {
            return FileStatus{FileType::kNotFound};
        }
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    }

    const Perms permissions = score::os::IntegerToMode(buffer.st_mode);
    /* caused by S_IS*-macroses */
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    if (S_ISREG(buffer.st_mode))
    {
        return FileStatus{FileType::kRegular, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISDIR(buffer.st_mode))
    {
        return FileStatus{FileType::kDirectory, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISBLK(buffer.st_mode))
    {
        return FileStatus{FileType::kBlock, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISCHR(buffer.st_mode))
    {
        return FileStatus{FileType::kCharacter, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISFIFO(buffer.st_mode))
    {
        return FileStatus{FileType::kFifo, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISSOCK(buffer.st_mode))
    {
        return FileStatus{FileType::kSocket, permissions};
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISLNK(buffer.st_mode))
    {
        return FileStatus{FileType::kSymlink, permissions};
    }
    else
    {
        return FileStatus{FileType::kUnknown, permissions};
    }
}

}  // namespace

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> StandardFilesystem::Exists(const Path& path) const noexcept
{
    const auto status = Status(path);
    if (status.has_value())
    {
        return status.value().Type() != FileType::kNotFound;
    }
    return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
}

score::ResultBlank StandardFilesystem::CopyFile(const Path& from, const Path& to) const noexcept
{
    return this->CopyFile(from, to, CopyOptions::kNone);
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
score::ResultBlank StandardFilesystem::CopyFile(const Path& from,
                                              const Path& to,
                                              const CopyOptions copy_option) const noexcept
{
    const auto from_status = Status(from);
    if ((!from_status.has_value()) || (from_status.value().Type() != FileType::kRegular))
    {
        return MakeUnexpected(filesystem::ErrorCode::kFromFileDoesNotExist);
    }

    const auto dest_status = Status(to);
    if (dest_status.has_value())
    {
        if (dest_status.value().Type() == FileType::kNotFound)
        {
            return CopyFileInternal(from, to);
        }
        else
        {
            return CopyKnownFileType(from, to, copy_option, dest_status);
        }
    }

    return MakeUnexpected(filesystem::ErrorCode::kCouldNotAccessFileDuringCopy, "Destination");
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<Blank> StandardFilesystem::CopyKnownFileType(const score::filesystem::Path& from,
                                                    const score::filesystem::Path& dest,
                                                    const score::filesystem::CopyOptions copy_option,
                                                    const Result<FileStatus>& dest_status) const noexcept
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
        if ((from_last_modified.has_value() && dest_last_modified.has_value()) &&
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

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
score::ResultBlank StandardFilesystem::CreateDirectory(const Path& path) const noexcept
{
    constexpr auto mode =
        os::Stat::Mode::kReadWriteExecUser | os::Stat::Mode::kReadWriteExecGroup | os::Stat::Mode::kReadWriteExecOthers;
    const auto result = os::Stat::instance().mkdir(path.CStr(), mode);
    if (result.has_value())
    {
        return {};
    }

    if (result.error() == score::os::Error::Code::kObjectExists)
    {
        const auto is_directory_result = IsDirectory(path);
        if (!is_directory_result.has_value())
        {
            return MakeUnexpected(filesystem::ErrorCode::kCouldNotCreateDirectory,
                                  "Path exists but could not check if it is a directory");
        }

        if (is_directory_result.value())
        {
            return {};
        }
        else
        {
            return MakeUnexpected(filesystem::ErrorCode::kCouldNotCreateDirectory,
                                  "Path exists but is not a directory");
        }
    }

    // Remove after reproduction (Ticket-243648)
    std::cerr << "mkdir failed with error code: " << result.error().ToString() << "\n";
    return MakeUnexpected(filesystem::ErrorCode::kCouldNotCreateDirectory);
}

score::ResultBlank StandardFilesystem::CreateDirectories(const Path& path) const noexcept
{
    const Path native_path = path.LexicallyNormal();
    if (native_path.Empty())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotCreateDirectory, "Empty path");
    }
    Path parent_path;
    bool do_skip_root_path = native_path.IsAbsolute();
    for (const Path& part_path : native_path)
    {
        if (part_path.Empty())
        {
            continue;  // for last empty part of path (for example, 'for/bar/')
        }
        parent_path /= part_path;
        if (do_skip_root_path)
        {
            do_skip_root_path = false;
            continue;
        }
        const auto result = CreateDirectory(parent_path);
        if (!result.has_value())
        {
            return result;
        }
    }
    return {};
}

score::Result<FileTime> StandardFilesystem::LastWriteTime(const Path& path) const noexcept
{
    os::StatBuffer buffer{};

    const auto result = os::Stat::instance().stat(path.CStr(), buffer, true);

    if (result.has_value())
    {
        return std::chrono::system_clock::from_time_t(buffer.mtime);
    }

    return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
}

score::ResultBlank StandardFilesystem::Permissions(const Path& path, const Perms permissions) const noexcept
{
    return this->Permissions(path, permissions, PermOptions::kReplace);
}

score::ResultBlank StandardFilesystem::Permissions(const Path& path,
                                                 const Perms permissions,
                                                 const PermOptions options) const noexcept
{
    if (!IsValid(options))
    {
        return score::MakeUnexpected(filesystem::ErrorCode::kCouldNotChangePermissions, "Invalid PermOptions.");
    }
    const auto HasOption = [options](const PermOptions& option) noexcept {
        return static_cast<bool>(static_cast<std::uint32_t>(option) & static_cast<std::uint32_t>(options));
    };
    const bool do_not_follow_symlink = HasOption(PermOptions::kNoFollow);
    // coverity[autosar_cpp14_a0_1_1_violation] explicitly default-initialized to silence clang warning
    Perms new_permissions{};
    const auto status = do_not_follow_symlink ? SymlinkStatus(path) : Status(path);
    if (!status.has_value())
    {
        return score::MakeUnexpected(filesystem::ErrorCode::kCouldNotChangePermissions, "Failed stat()/lstat().");
    }
    if (HasOption(PermOptions::kReplace))
    {
        new_permissions = permissions;
    }
    else
    {
        // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(HasOption(PermOptions::kAdd) || HasOption(PermOptions::kRemove));
        // LCOV_EXCL_BR_STOP
        new_permissions = status->Permissions();
        if (HasOption(PermOptions::kAdd))
        {
            new_permissions |= permissions;
        }
        else  // PermOptions::kRemove
        {
            new_permissions &= ~permissions;
        }
    }
    if (new_permissions == status->Permissions())
    {
        return {};
    }

    // note: since lchmod() does not exist in QNX, fchmodat() is used instead of lchmod().
    const bool resolve_symlinks = !do_not_follow_symlink;
    const auto result = score::os::Stat::instance().fchmodat(AT_FDCWD, path.CStr(), new_permissions, resolve_symlinks);
    if (!result.has_value())
    {
        return score::MakeUnexpected(filesystem::ErrorCode::kCouldNotChangePermissions, "Failed chmod()/lchmod.");
    }

    return {};
}

score::ResultBlank StandardFilesystem::Remove(const Path& path) const noexcept
{
    const auto result = score::os::Stdio::instance().remove(path.CStr());
    if (!result.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory);
    }

    return {};
}

score::ResultBlank StandardFilesystem::RemoveAll(const Path& path) const noexcept
{
    ResultBlank result{};

    const Result<FileStatus> path_status = SymlinkStatus(path);
    if (!path_status.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Failed to get status for path.");
    }
    if (path_status->Type() == FileType::kNotFound)
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Path not found.");
    }
    if (path_status->Type() != FileType::kDirectory)
    {
        const auto remove_result = Remove(path);
        if (!remove_result.has_value())
        {
            return MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory,
                                  "Failed to remove file for path.");
        }
        return {};
    }

    result = RemoveContentFromExistingDirectory(path);

    return result;
}

Result<Blank> StandardFilesystem::RemoveContentFromExistingDirectory(const Path& path) const noexcept
{
    ResultBlank result{};

    std::stack<Path> directories{};
    score::cpp::ignore = directories.emplace(path);
    const auto iterator = RecursiveDirectoryIterator{path};
    for (const auto& entry : iterator)
    {
        const Result<FileStatus> entry_status = entry.SymlinkStatus();
        if (entry_status.has_value())
        {
            if (entry_status->Type() == FileType::kDirectory)
            {
                score::cpp::ignore = directories.emplace(entry.GetPath());
            }
            else
            {
                const auto remove_result = Remove(entry.GetPath());
                if (!remove_result.has_value())
                {
                    result =
                        MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Failed to remove file.");
                }
            }
        }
        else
        {
            result = MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Failed to get status.");
        }
    }
    if (!iterator.HasValue())
    {
        result =
            MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Failed to iterate through folder.");
    }

    while (!directories.empty())
    {
        const ResultBlank remove_result = Remove(directories.top());
        if (!remove_result.has_value())
        {
            result = MakeUnexpected(filesystem::ErrorCode::kCouldNotRemoveFileOrDirectory, "Failed to remove folder.");
        }
        directories.pop();
    }
    return result;
}

Result<FileStatus> StandardFilesystem::Status(const Path& path) const noexcept
{
    return StatusInternal(path, true);
}

Result<FileStatus> StandardFilesystem::SymlinkStatus(const Path& path) const noexcept
{
    return StatusInternal(path, false);
}

score::Result<Path> StandardFilesystem::TempDirectoryPath() const noexcept
{
    const auto* const tmpdir_exists = score::os::Stdlib::instance().getenv("TMPDIR");
    if (tmpdir_exists != nullptr)
    {
        return tmpdir_exists;
    }

    const auto* const tmp_exists = score::os::Stdlib::instance().getenv("TMP");
    if (tmp_exists != nullptr)
    {
        return tmp_exists;
    }

    const auto* const temp_exists = score::os::Stdlib::instance().getenv("TEMP");
    if (temp_exists != nullptr)
    {
        return temp_exists;
    }

    const auto* const tempdir_exists = score::os::Stdlib::instance().getenv("TEMPDIR");
    if (tempdir_exists != nullptr)
    {
        return tempdir_exists;
    }

    return "/tmp";
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<Path> StandardFilesystem::WeaklyCanonical(const Path& path) const noexcept
{
    if (path.Empty())
    {
        return path;
    }
    const auto canonical_res = Canonical(path);
    if (canonical_res.has_value())
    {
        return canonical_res.value();
    }
    Path res{};
    bool is_canonical_ok{true};
    for (auto i = path.begin(); i != path.end(); ++i)
    {
        if (is_canonical_ok)
        {
            const Path path_for_try = res / *i;
            const auto canonical_res_try = Canonical(path_for_try);
            is_canonical_ok = canonical_res_try.has_value();
            if (is_canonical_ok)
            {
                res = canonical_res_try.value();
            }
            else
            {
                if (res.Empty() && path.IsRelative())
                {
                    const auto current_path_res = CurrentPath();
                    if (!current_path_res.has_value())
                    {
                        return MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath,
                                              "Could not get current path during WeaklyCanonical().");
                    }
                    res = current_path_res.value();
                }
                res /= *i;
            }
        }
        else
        {
            res /= *i;
        }
    }
    return res.LexicallyNormal();
}

Result<Path> StandardFilesystem::CurrentPath() const noexcept
{
    constexpr auto kBufSize =
        static_cast<std::size_t>(PATH_MAX); /* KW_SUPPRESS:MISRA.USE.EXPANSION: caused by PATH_MAX */
    std::array<Path::value_type, kBufSize> buf = {
        0,
    };
    const auto current_path = score::os::Unistd::instance().getcwd(buf.data(), kBufSize);
    if (!current_path.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath);
    }
    return Path{current_path.value()};
}

ResultBlank StandardFilesystem::CurrentPath(const Path& path) const noexcept
{
    if (!score::os::Unistd::instance().chdir(path.CStr()).has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotSetCurrentPath);
    }
    return {};
}

ResultBlank StandardFilesystem::CreateHardLink(const Path& oldpath, const Path& newpath) const noexcept
{
    if (!score::os::Unistd::instance().link(oldpath.CStr(), newpath.CStr()).has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateHardLink);
    }
    return {};
}

Result<uint64_t> StandardFilesystem::HardLinkCount(const Path& path) const noexcept
{
    os::StatBuffer buffer{};
    const auto result = os::Stat::instance().stat(path.CStr(), buffer, true);
    if (result.has_value())
    {
        return buffer.st_nlink;
    }
    return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> StandardFilesystem::IsDirectory(const Path& path) const noexcept
{
    const auto status = Status(path);
    if (!status.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    }
    return status.value().Type() == FileType::kDirectory;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> StandardFilesystem::IsRegularFile(const Path& path) const noexcept
{
    const auto status = Status(path);
    if (!status.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    }
    return status.value().Type() == FileType::kRegular;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> StandardFilesystem::IsSymlink(const Path& path) const noexcept
{
    const auto status = SymlinkStatus(path);
    if (!status.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    }
    return status.value().Type() == FileType::kSymlink;
}

Result<bool> StandardFilesystem::IsEmpty(const Path& path) const noexcept
{
    os::StatBuffer buffer{};
    const auto result = os::Stat::instance().stat(path.CStr(), buffer, true);
    if (!result.has_value())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotRetrieveStatus);
    }
    /* KW_SUPPRESS_START:MISRA.TOKEN.OCTAL.INT,MISRA.CONV.INT.SIGN,MISRA.BITS.NOT_UNSIGNED,MISRA.USE.EXPANSION: */
    /* caused by S_IS*- *macroses */
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    if (S_ISDIR(buffer.st_mode))
    {
        const DirectoryIterator iterator{path};
        if (!iterator.HasValue())
        {
            return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenDirectory);
        }
        return iterator == end(iterator);
    }
    // coverity[autosar_cpp14_m5_0_4_violation] caused by macros
    // coverity[autosar_cpp14_m5_0_21_violation] caused by macros
    // coverity[autosar_cpp14_m2_13_3_violation] caused by macros
    else if (S_ISREG(buffer.st_mode))
    {
        return buffer.st_size == 0;
    }
    else
    {
        // No action required
    }
    /* KW_SUPPRESS_END:MISRA.TOKEN.OCTAL.INT,MISRA.CONV.INT.SIGN,MISRA.BITS.NOT_UNSIGNED,MISRA.USE.EXPANSION: */
    return false;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<Path> StandardFilesystem::Absolute(const Path& path) const noexcept
{
    if (path.IsAbsolute())
    {
        return path;
    }
    if (path.Empty())
    {
        return Path{};
    }
    const auto current_path_res = CurrentPath();
    if (!current_path_res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGetCurrentPath);
    }
    return current_path_res.value() / path;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<Path> StandardFilesystem::Canonical(const Path& path) const noexcept
{
    if (path.Empty())
    {
        return MakeUnexpected(ErrorCode::kInvalidPath);
    }
    const auto absolute_res = Absolute(path);
    if (!absolute_res.has_value())
    {
        return absolute_res;
    }
    const auto absolute_path = absolute_res.value();
    std::array<Path::value_type, PATH_MAX> buf = {
        0,
    };
    const auto realpath_res = score::os::Stdlib::instance().realpath(absolute_path.CStr(), buf.data());
    if (!realpath_res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGetRealPath);
    }
    return Path{realpath_res.value()};
}

ResultBlank StandardFilesystem::CreateSymlink(const Path& target, const Path& linkpath) const noexcept
{
    if (!score::os::Unistd::instance().symlink(target.CStr(), linkpath.CStr()).has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateSymlink);
    }
    return {};
}

// coverity[autosar_cpp14_a15_5_3_violation]: `len < kBufSize` ensures `at(len)` won't throw.
Result<Path> StandardFilesystem::ReadSymlink(const Path& path) const noexcept
{
    constexpr auto kBufSize =
        static_cast<std::size_t>(PATH_MAX); /* KW_SUPPRESS:MISRA.USE.EXPANSION: caused by PATH_MAX */
    std::array<Path::value_type, kBufSize> buf = {
        0,
    };
    // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
    // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
    // NOLINTNEXTLINE(score-banned-function): See above
    const auto result = score::os::Unistd::instance().readlink(path.CStr(), buf.data(), kBufSize);
    if (!result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotReadSymlink);
    }
    const auto len = static_cast<std::size_t>(result.value());
    if (len >= kBufSize)
    {
        return MakeUnexpected(ErrorCode::kCouldNotReadSymlink, "The buffer is too small to hold all of the contents.");
    }
    buf.at(len) = '\0';
    return Path(buf.data());
}

ResultBlank StandardFilesystem::CreateDirectorySymlink(const Path& target, const Path& linkpath) const noexcept
{
    return CreateSymlink(target, linkpath);
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
ResultBlank StandardFilesystem::CopySymlink(const Path& from, const Path& dest) const noexcept
{
    const auto read_symlink_result = ReadSymlink(from);
    if (!read_symlink_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotReadSymlink, "Failed to read a symlink during copy.");
    }
    return CreateSymlink(read_symlink_result.value(), dest);
}

}  // namespace filesystem
}  // namespace score
