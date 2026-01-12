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
#include "score/filesystem/file_utils/file_utils.h"

#include "score/filesystem/filestream/i_file_factory.h"
#include "score/filesystem/i_standard_filesystem.h"
#include "score/os/fcntl.h"
#include "score/os/grp.h"
#include "score/os/stdlib.h"
#include "score/os/unistd.h"

#include <array>
#include <cstdint>
#include <sstream>

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
bool IsFileContentIdentical(std::iostream& file1, std::iostream& file2) noexcept
{
    std::istreambuf_iterator<char> it1{file1.rdbuf()};
    std::istreambuf_iterator<char> it2{file2.rdbuf()};
    constexpr std::istreambuf_iterator<char> end{};

    while (it1 != end)
    {
        if (it2 == end)
        {
            break;
        }
        // LCOV_EXCL_BR_START both branches are covered by unit tests:
        // true: FileUtilsTest_FileContentsAreIdentical.ContentAreNotEqual
        // false: FileUtilsTest_FileContentsAreIdentical.FilesAreEqual
        if (!(*it1 == *it2))
        // LCOV_EXCL_BR_STOP
        {
            return false;
        }
        ++it1;
        ++it2;
    }
    return (it1 == end) && (it2 == end);
}
}  // namespace

FileUtils::FileUtils(IStandardFilesystem& standard_filesystem, IFileFactory& file_factory) noexcept
    : IFileUtils(), standard_filesystem_{standard_filesystem}, file_factory_{file_factory}
{
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
ResultBlank FileUtils::CreateDirectory(const Path& path, const score::os::Stat::Mode perms) const noexcept
{
    const Result<filesystem::FileStatus> status = standard_filesystem_.Status(path);
    if (status.has_value())
    {
        if (status.value().Type() == filesystem::FileType::kDirectory)
        {
            if (status.value().Permissions() == perms)
            {
                return {};
            }
            return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Directory exists but with wrong permissions");
        }

        if (status.value().Type() == filesystem::FileType::kNotFound)
        {
            if (!standard_filesystem_.CreateDirectory(path).has_value())
            {
                return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
            }
            if (!standard_filesystem_.Permissions(path, perms, PermOptions::kReplace).has_value())
            {
                return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Failed to set permissions");
            }
            return {};
        }
        return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Path already exists and is not a directory");
    }
    return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Failed to retrieve status");
}

// Implicit call in .value(). Call does not issue terminate because of
// previous check in algorithm.
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank FileUtils::CreatePath(const Path& final_path,
                                  const Path& cur_path,
                                  const score::os::Stat::Mode perms,
                                  const std::int64_t creation_delay_ns,
                                  const std::int64_t creation_retry_factor,
                                  std::uint32_t creation_retry_counter) const noexcept
{
    timespec creation_retry_delay{0, creation_delay_ns};
    // internal loop just to handle (accumulating across the whole path) retry attempts
    while (true)
    {
        // For parent directories of the final path we accept that they exist with whatever permissions
        if (cur_path != final_path)
        {
            const auto exists = standard_filesystem_.IsDirectory(cur_path);
            if (exists.has_value() && exists.value())
            {
                break;
            }
        }

        const auto result = CreateDirectory(cur_path, perms);
        if (!result.has_value())
        {
            if (creation_retry_counter != 0U)
            {
                --creation_retry_counter;
                std::ignore = os::Unistd::instance().nanosleep(&creation_retry_delay, nullptr);
                // coverity[autosar_cpp14_a4_7_1_violation] Values are chosen to not lead to data loss
                creation_retry_delay.tv_nsec *= creation_retry_factor;  // shall not be larger than 1 second

                // coverity[autosar_cpp14_m6_6_3_violation] This is a well-formed for loop for C++17 upwards
                continue;
            }
            return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory);
        }
        break;
    }
    return {};
}

// Implicit call in .value(). Call does not issue terminate because of
// previous check in algorithm.
// coverity[autosar_cpp14_a15_5_3_violation]
ResultBlank FileUtils::CreateDirectories(const Path& path, const score::os::Stat::Mode perms) const noexcept
{
    // For the purpose of Ticket-172058 fix, we need to save the existing APIs,
    // thus we keep these values as predefined constants. The maximum accumulated delay for path creation is 140ms
    constexpr std::int64_t kCreationRetryInitialDelayMilliseconds = 20;
    constexpr std::int64_t kCreationRetryInitialDelayMicroseconds = kCreationRetryInitialDelayMilliseconds * 1000;
    // coverity[autosar_cpp14_a0_1_1_violation] false-positive: used in the following static assert
    constexpr std::int64_t kCreationRetryInitialDelayNanoseconds = kCreationRetryInitialDelayMicroseconds * 1000;
    constexpr std::uint32_t kCreationRetryLimit = 3U;
    // coverity[autosar_cpp14_a0_1_1_violation] false-positive: used in the following static assert
    constexpr std::int64_t kCreationRetryFactor = 2;
    // coverity[autosar_cpp14_a0_1_1_violation] false-positive: used in the following static assert
    constexpr std::int64_t kNanosecondsPerSecond = 1000000000;
    static_assert(
        kCreationRetryInitialDelayNanoseconds * kCreationRetryFactor * kCreationRetryLimit < kNanosecondsPerSecond,
        "Delay may exceed one second, breaking algorithm");

    const Path native_path = path.LexicallyNormal();
    if (native_path.Empty())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCreateDirectory, "Empty path");
    }

    Path parent_path{};
    bool do_skip_root_path = native_path.IsAbsolute();

    for (const Path& part_path : native_path)
    {
        if (part_path.Empty())
        {
            // coverity[autosar_cpp14_m6_6_3_violation] This is a well-formed for loop for C++17 upwards
            continue;  // for last empty part of path (for example, 'for/bar/')
        }
        parent_path /= part_path;
        if (do_skip_root_path)
        {
            do_skip_root_path = false;
            // coverity[autosar_cpp14_m6_6_3_violation] This is a well-formed for loop for C++17 upwards
            continue;
        }

        const auto result = CreatePath(
            path, parent_path, perms, kCreationRetryInitialDelayNanoseconds, kCreationRetryFactor, kCreationRetryLimit);
        if (!result.has_value())
        {
            return result;
        }
    }
    return {};
}

ResultBlank FileUtils::ChangeGroup(const Path& path, const std::string& group_name) const noexcept
{
    std::lock_guard<std::mutex> guard{groupname_mutex_};
    // NOLINTNEXTLINE(score-banned-function): Banned because not thread safe, with mutex, caveat solved.
    const auto group_name_result = os::Grp::instance().getgrnam(group_name.c_str());
    if (!group_name_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotChangeGroup, "Unable to get group file entry");
    }
    const auto gid = group_name_result.value().gid;
    return ChangeGroup(path, gid);
}

ResultBlank FileUtils::ChangeGroup(const Path& path, const gid_t group_id) const noexcept
{
    const auto uid = os::Unistd::instance().getuid();
    const auto result = os::Unistd::instance().chown(path.CStr(), uid, group_id);
    if (!result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotChangeGroup, "Unable to change group ownership");
    }
    return {};
}

Result<bool> FileUtils::ValidateGroup(const Path& path, const std::string& group_name) const noexcept
{
    std::lock_guard<std::mutex> guard{groupname_mutex_};
    // NOLINTNEXTLINE(score-banned-function): Banned because not thread safe, with mutex, caveat solved.
    const auto groupResult = os::Grp::instance().getgrnam(group_name.c_str());
    if (!groupResult.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotValidateGroup, "Unable to get group file entry");
    }
    os::StatBuffer status{};
    const auto statResult = os::Stat::instance().stat(path.CStr(), status);
    if (!statResult.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotValidateGroup, "Unable to get file status");
    }
    return groupResult.value().gid == status.st_gid;
}

Result<std::pair<std::unique_ptr<std::iostream>, Path>> FileUtils::OpenUniqueFile(const Path& path,
                                                                                  std::ios_base::openmode mode) const
{
    const auto& template_path = path.Native();
    const std::string_view template_path_view{template_path};
    constexpr std::string_view template_path_ending_pattern{"-XXXXXX"};
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(template_path.size() >= template_path_ending_pattern.size());
    const auto template_path_ending =
        template_path_view.substr(template_path.size() - template_path_ending_pattern.size());
    if (template_path_ending != template_path_ending_pattern)
    {
        return MakeUnexpected(ErrorCode::kCouldNotGenerateTmpName, "Invalid filename template");
    }

    std::vector<char> template_store{};
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(template_path.size() < SIZE_MAX);
    template_store.reserve(template_path.size() + 1u);
    score::cpp::ignore = template_store.insert(template_store.begin(), template_path.cbegin(), template_path.cend());
    template_store.push_back('\0');

    const auto mkstemp_result = score::os::Stdlib::instance().mkstemp(template_store.data());
    if (!mkstemp_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGenerateTmpName, "Failed mkstemp");
    }

    const auto fileDescriptor = mkstemp_result.value();
    // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
    // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
    // NOLINTNEXTLINE(score-banned-function): See above
    if (!score::os::Unistd::instance().close(fileDescriptor).has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGenerateTmpName, "Failed close");
    }
    const Path tmp_file_path{{template_store.data()}};
    auto file = file_factory_.Open(tmp_file_path, mode);
    if (!file.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotGenerateTmpName, "Failed open");
    }
    return std::make_pair(std::move(file).value(), tmp_file_path);
}

ResultBlank FileUtils::SyncDirectory(const Path& dirname) const noexcept
{
    // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
    // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
    // NOLINTNEXTLINE(score-banned-function): See above
    const auto open_result = score::os::Fcntl::instance().open(dirname.CStr(), score::os::Fcntl::Open::kReadOnly);
    if (!open_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotSyncDirectory, "Failed to open directory.");
    }
    const auto directory_descriptor = open_result.value();
    const auto fsync_result = score::os::Unistd::instance().fsync(directory_descriptor);
    // The reason for banning is, because it's error-prone to use. One should use abstractions e.g. provided by
    // the C++ standard library. Since this library exactly is such abstraction, we can use the OS function.
    // NOLINTNEXTLINE(score-banned-function): See above
    score::cpp::ignore = score::os::Unistd::instance().close(directory_descriptor);
    if (!fsync_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotSyncDirectory, "Failed to fsync directory.");
    }
    return {};
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> FileUtils::FilesExist(const score::filesystem::Path& path1,
                                   const score::filesystem::Path& path2) const noexcept
{
    const auto file1_exists_result = standard_filesystem_.IsRegularFile(path1);
    if (!file1_exists_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "Failed to check file1 existence");
    }
    if (!file1_exists_result.value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "File1 does not exist or is not a regular file");
    }
    const auto file2_exists_result = standard_filesystem_.IsRegularFile(path2);
    if (!file2_exists_result.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "Failed to check file2 existence");
    }
    if (!file2_exists_result.value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "File2 does not exist or is not a regular file");
    }
    return true;
}

// Refer on top for suppression justification
// coverity[autosar_cpp14_a15_5_3_violation : FALSE]
Result<bool> FileUtils::FileContentsAreIdentical(const Path& path1, const Path& path2) const noexcept
{
    // check file existence
    const auto check_exist_result = FilesExist(path1, path2);
    if (!check_exist_result)
    {
        return check_exist_result;
    }

    // open files
    const auto flags = std::ios_base::in | std::ios_base::binary; /* KW_SUPPRESS:MISRA.ENUM.OPERAND:ios_base */
    const auto file1 = file_factory_.Open(path1, flags);
    if (((!file1.has_value()) || (file1.value() == nullptr)) || (file1.value()->fail()))
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "Failed to open file1");
    }
    const auto file2 = file_factory_.Open(path2, flags);
    if (((!file2.has_value()) || (file2.value() == nullptr)) || (file2.value()->fail()))
    {
        return MakeUnexpected(ErrorCode::kCouldNotCompareFiles, "Failed to open file2");
    }

    // compare
    return IsFileContentIdentical(**file1, **file2);
}

ResultBlank FileUtils::CheckFileSystem(const Path& partition) const noexcept
{
    const std::string cmd = std::string{"e2fsck -n "} + partition.Native();
    const auto res = os::Stdlib::instance().system_call(cmd);
    if (!res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCorruptedFileSystem);
    }
    return {};
}

ResultBlank FileUtils::RepairFileSystem(const Path& partition) const noexcept
{
    const std::string cmd = std::string{"e2fsck -p "} + partition.Native();
    const auto res = os::Stdlib::instance().system_call(cmd);
    if (!res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCorruptedFileSystem);
    }
    return {};
}

ResultBlank FileUtils::FormatPartition(const Path& partition) const noexcept
{
    // Reference: broken_link_c/issue/4757296
    const std::string mkfsCmd{"mkfs.ext2 -F -b 4096 -I 128 -O ^resize_inode,^large_file,^filetype,^dir_index "};
    const std::string cmd = mkfsCmd + partition.Native();
    const auto res = os::Stdlib::instance().system_call(cmd);
    if (!res.has_value())
    {
        return MakeUnexpected(ErrorCode::kCouldNotFormatPartition);
    }
    return {};
}

namespace
{
constexpr std::string_view FILESYSTEM_FEATURES_OPTION = "Filesystem features:";
// coverity[autosar_cpp14_a3_3_2_violation]: std::vector is required to use operator==
const std::vector<std::string> QNX_COMPATIBLE_FEATURES{"ext_attr", "sparse_super"};

std::vector<std::string> extractSortedFeatures(const std::string& featureLine)
{
    // LCOV_EXCL_BR_START caused by SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(featureLine.size() >= FILESYSTEM_FEATURES_OPTION.size());
    // LCOV_EXCL_BR_STOP
    // LCOV_EXCL_BR_START no obvious branches here, covered by FileUtilsTest_IsQnxCompatible.pass unit test
    const std::string featuresWithoutHeader{featureLine, FILESYSTEM_FEATURES_OPTION.size()};
    // LCOV_EXCL_BR_STOP
    std::istringstream featureStream{featuresWithoutHeader};
    std::vector<std::string> features{};
    std::string feature{};
    // LCOV_EXCL_BR_START true/false branches covered by FileUtilsTest_IsQnxCompatible.pass unit test
    while (!(featureStream >> feature).fail())
    // LCOV_EXCL_BR_STOP
    {
        features.push_back(feature);
    }
    std::sort(std::begin(features), std::end(features));
    return features;
}

std::string getFsInfoCommand(const Path& partition, const Path& outputFile)
{
// necessary for platform-specific commands
// can be fixed by adding a separate constexpr function
// that will reduce readability
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
    // LCOV_EXCL_START covered by FileUtilsTest_IsQnxCompatible.pass unit test
    const std::string FS_INFO_COMMAND = "dumpe2fs -h ";
    // LCOV_EXCL_STOP
// coverity[autosar_cpp14_a16_0_1_violation]
#elif defined(__linux__)
    const std::string FS_INFO_COMMAND = "tune2fs -l ";
// coverity[autosar_cpp14_a16_0_1_violation]
#else
// coverity[autosar_cpp14_a16_0_1_violation]
#error "Unsupported operating system"
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    // LCOV_EXCL_BR_START no obvious branches here, covered by FileUtilsTest_IsQnxCompatible.pass unit test
    return FS_INFO_COMMAND + partition.Native() + " > " + outputFile.Native();
    // LCOV_EXCL_BR_STOP
}
}  // namespace

bool FileUtils::IsQnxCompatible(const Path& partition) const
{
    auto tmpFile = OpenUniqueFile("/tmp/TmpFile-XXXXXX", std::ios::in);
    if (!tmpFile.has_value())
    {
        return false;
    }

    const auto fsInfoCommand = getFsInfoCommand(partition, std::get<1>(tmpFile.value()));
    const auto fsParamsRes = os::Stdlib::instance().system_call(fsInfoCommand);
    if (!fsParamsRes.has_value())
    {
        return false;
    }

    auto fsInfoOutputStream = std::move(std::get<0>(tmpFile.value()));
    if ((fsInfoOutputStream == nullptr) || (fsInfoOutputStream->fail()))
    {
        return false;
    }
    std::string fsInfoLine;
    auto is_getLine_fail = (std::getline(*fsInfoOutputStream, fsInfoLine)).fail();
    while (!is_getLine_fail)
    {
        if (fsInfoLine.rfind(FILESYSTEM_FEATURES_OPTION, 0u) == 0u)  // starts with
        {
            return extractSortedFeatures(fsInfoLine) == QNX_COMPATIBLE_FEATURES;
        }
        is_getLine_fail = (std::getline(*fsInfoOutputStream, fsInfoLine)).fail();
    }

    return false;
}

}  // namespace filesystem
}  // namespace score
