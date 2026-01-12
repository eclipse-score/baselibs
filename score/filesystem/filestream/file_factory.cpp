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
#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/file_status.h"
#include "score/filesystem/filestream/file_buf.h"
#include "score/filesystem/filestream/file_stream.h"

#include "score/os/fcntl.h"
#include "score/os/stdio.h"
#include "score/os/unistd.h"

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace score::filesystem
{
namespace details
{
namespace
{
constexpr std::int32_t kTIDDigitsLength = 6;
constexpr std::uint32_t kTIDDigitsCropMask = 1'000'000U;
constexpr std::int32_t kSystemTicksDigitsLength = 8;
constexpr std::uint32_t kSystemTicksDigitsCropMask = 100'000'000U;

}  // namespace
std::string ComposeTempFilename(std::string_view original_filename,
                                std::size_t threa_id_hash,
                                std::uint64_t timestamp) noexcept
{
    std::stringstream final_filename;
    final_filename << "." << original_filename << "-" << std::setw(kTIDDigitsLength) << std::setfill('0')
                   << (threa_id_hash % kTIDDigitsCropMask) << "-" << std::setw(kSystemTicksDigitsLength)
                   << std::setfill('0') << (timestamp % kSystemTicksDigitsCropMask);
    return final_filename.str();
}
}  // namespace details
namespace
{

constexpr os::Stat::Mode kDefaultMode = os::Stat::Mode::kReadUser | os::Stat::Mode::kWriteUser |
                                        os::Stat::Mode::kReadGroup | os::Stat::Mode::kWriteGroup |
                                        os::Stat::Mode::kReadOthers | os::Stat::Mode::kWriteOthers;

using OpenFlags = os::Fcntl::Open;

OpenFlags IosOpenModeToOpenFlags(const std::ios_base::openmode mode) noexcept
{
    // The value won't be used as such, but rather will be initialized properly by the bit-wise operations below.
    // Also, the Enum does not provide a 0-valued element, that must be the starting point.
    // Therefore, initializing it to 0U is the simplest solution. We could do something like
    // OpenFlags flags{OpenFlags::kReadOnly & OpenFlags::kWriteOnly} but that would be a bit
    // overkill for this case and would depend on the current values;
    // coverity[autosar_cpp14_a7_2_1_violation]
    OpenFlags flags{0U};
    if ((mode & std::ios_base::in) != 0U)
    {
        if ((mode & std::ios_base::out) != 0U)
        {
            flags = OpenFlags::kReadWrite;
        }
        else
        {
            flags = OpenFlags::kReadOnly;
        }
    }
    else if ((mode & std::ios_base::out) != 0U)
    {
        flags = OpenFlags::kWriteOnly | OpenFlags::kCreate | OpenFlags::kTruncate;
    }

    if ((mode & std::ios_base::app) != 0U)
    {
        flags &= ~OpenFlags::kTruncate;
        flags |= OpenFlags::kAppend | OpenFlags::kCreate;
    }
    if ((mode & std::ios_base::trunc) != 0U)
    {
        flags |= OpenFlags::kTruncate | OpenFlags::kCreate;
    }
    return flags;
}

std::string ComposeTempFilename(std::string original_filename) noexcept
{
    const auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    const auto ticks = static_cast<uint64_t>(duration.count());
    return details::ComposeTempFilename(original_filename, tid, ticks);
}

[[nodiscard]] os::Stat::Mode ExtractMode(const score::Result<details::IdentityMetadata>& metadata)
{
    if (!metadata.has_value())
    {
        return kDefaultMode;
    }
    return metadata.value().mode;
}

// Supression: -1 is a value to indicate to the system that we don't intend to change the user id (Linux and QNX
// compatible). Since -1 is equal to max value for unsigned type of variable we defined as cast -1U to uid_t.
// coverity[autosar_cpp14_m5_3_2_violation]
// coverity[autosar_cpp14_m5_19_1_violation]
inline constexpr uid_t kDoNotChangeUID{static_cast<uid_t>(-1U)};

// Supression: -1 is a value to indicate to the system that we don't intend to change the group id (Linux and QNX
// compatible). Since -1 is equal to max value for unsigned type of variable we defined as cast -1U to gid_t.
// coverity[autosar_cpp14_m5_3_2_violation]
// coverity[autosar_cpp14_m5_19_1_violation]
inline constexpr uid_t kDoNotChangeGID{static_cast<gid_t>(-1U)};

[[nodiscard]] uid_t ExtractUid(const details::IdentityMetadata& metadata,
                               const AtomicUpdateOwnershipFlags ownership_flag)
{
    const uid_t uid = metadata.uid;
    if (((ownership_flag & kUseCurrentProcessUID) == kUseCurrentProcessUID) || (uid == ::getuid()))
    {
        return score::filesystem::kDoNotChangeUID;
    }
    return uid;
}

[[nodiscard]] gid_t ExtractGid(const details::IdentityMetadata& metadata,
                               const AtomicUpdateOwnershipFlags ownership_flag)
{
    const gid_t gid = metadata.gid;
    if (((ownership_flag & kUseCurrentProcessGID) == kUseCurrentProcessGID) || (gid == ::getgid()))
    {
        return score::filesystem::kDoNotChangeGID;
    }
    return gid;
}

}  // namespace

namespace details
{

Result<int> OpenFileHandle(const Path& path,
                           const std::ios_base::openmode mode,
                           const os::Stat::Mode create_mode) noexcept
{
    const auto flags = IosOpenModeToOpenFlags(mode);
    // NOLINTNEXTLINE(score-banned-function): We need to use the POSIX open call to obtain a file descriptor.
    const auto file_handle = os::Fcntl::instance().open(path.CStr(), flags, create_mode);
    if (file_handle.has_value())
    {
        return *file_handle;
    }
    else
    {
        return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
    }
}

Result<IdentityMetadata> GetIdentityMetadata(const Path& path)
{
    os::StatBuffer buffer{};
    const auto result = os::Stat::instance().stat(path.CStr(), buffer, true);
    if (result.has_value())
    {
        // NOLINTBEGIN(hicpp-signed-bitwise): macro does not affect the sign of the result.
        // Suppress "AUTOSAR C++14 M5-0-21" rule findings. This rule declares: "Bitwise operators shall only be
        // applied to operands of unsigned underlying type."
        // Rationale: Macro does not affect the sign of the result
        // Suppress "AUTOSAR C++14 M2-13-3" rule findings. This rule declares: "A “U” suffix shall be applied to all
        // octal or hexadecimal integer literals of unsigned type."
        // Suppress "AUTOSAR C++14 M5-0-4" rule findings. This rule declares: "An implicit integral conversion shall
        // not change the signedness of the underlying type"
        // Rationale: S_IFMT(integer), S_IFREG(integer) and StatBuffer.st_mode(u-integer) are defined in stat.h from
        // infrastructure delivery and can't be modified by user
        // coverity[autosar_cpp14_m5_0_21_violation]
        // coverity[autosar_cpp14_m2_13_3_violation]
        // coverity[autosar_cpp14_m5_0_4_violation]
        if ((buffer.st_mode & S_IFMT) != S_IFREG)
        // NOLINTEND(hicpp-signed-bitwise): macro does not affect the sign of the result.
        {
            return MakeUnexpected(ErrorCode::kNotImplemented);
        }

        os::Stat::Mode mode = os::IntegerToMode(buffer.st_mode);
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
        // loss."
        // Rationale: QNX defined uid_t as uint32_t, so no data loss expected
        // coverity[autosar_cpp14_a4_7_1_violation : FALSE]
        const auto uid = static_cast<uid_t>(buffer.st_uid);
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
        // loss."
        // Rationale: QNX defined gid_t as uint32_t, so no data loss expected
        // coverity[autosar_cpp14_a4_7_1_violation : FALSE]
        const auto gid = static_cast<gid_t>(buffer.st_gid);

        return IdentityMetadata{mode, uid, gid};
    }
    else
    {
        return MakeUnexpected(ErrorCode::kCouldNotRetrieveStatus);
    }
}

}  // namespace details

Result<std::unique_ptr<std::iostream>> FileFactory::Open(const Path& path, const std::ios_base::openmode mode)
{
    return details::OpenFileHandle(path, mode, kDefaultMode).and_then([mode](int file_handle) {
        return details::CreateFileStream<details::StdioFileBuf>(file_handle, mode);
    });
}

// as intended, we don't enforce users to specify ownership flags unless needed
// defaults for override and base function are the same thus static binding is safe
// NOLINTNEXTLINE(google-default-arguments) : see above
Result<std::unique_ptr<FileStream>> FileFactory::AtomicUpdate(const Path& path,
                                                              const std::ios_base::openmode mode,
                                                              const AtomicUpdateOwnershipFlags ownership_flag)
{
    if ((mode & ~(std::ios::out | std::ios::trunc | std::ios::binary)) != 0U)
    {
        return MakeUnexpected(ErrorCode::kNotImplemented);
    }

    auto filename = path.Filename();
    std::string_view filename_view{filename.Native()};
    if (filename_view.empty())
    {
        return MakeUnexpected(filesystem::ErrorCode::kCouldNotOpenFileStream);
    }

    const auto rand_filename = ComposeTempFilename(filename.Native());
    auto temp_path = path.ParentPath();
    temp_path /= rand_filename;

    const auto metadata = details::GetIdentityMetadata(path);
    const auto create_mode = ExtractMode(metadata);

    if (auto file_handle = details::OpenFileHandle(temp_path, mode, create_mode); file_handle.has_value())
    {
        if (metadata.has_value())
        {
            const uid_t uid = ExtractUid(metadata.value(), ownership_flag);
            const gid_t gid = ExtractGid(metadata.value(), ownership_flag);

            if ((uid != kDoNotChangeUID) || (gid != kDoNotChangeGID))
            {
                auto ownership_adjustment = os::Unistd::instance().chown(temp_path.CStr(), uid, gid);
                if (!ownership_adjustment.has_value())
                {
                    // If close or unlink fails, there is not much we can do. Returning a different error would hide the
                    // original issue that permissions could not be set.
                    // We could log an error but this library is currently designed without logging. Introduce logging
                    // only for that does not seem worth it.
                    score::cpp::ignore = os::Unistd::instance().close(*file_handle);
                    score::cpp::ignore = os::Unistd::instance().unlink(temp_path.CStr());
                    return MakeUnexpected(ErrorCode::kCouldNotSetPermissions);
                }
            }
        }

        return details::CreateFileStream<details::AtomicFileBuf>(*file_handle, mode, std::move(temp_path), path);
    }
    else
    {
        return MakeUnexpected<std::unique_ptr<FileStream>>(file_handle.error());
    }
}

}  // namespace score::filesystem
