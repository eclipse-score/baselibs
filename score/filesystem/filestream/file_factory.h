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
#ifndef SCORE_LIB_FILESYSTEM_FILE_FACTORY_H
#define SCORE_LIB_FILESYSTEM_FILE_FACTORY_H

#include "score/filesystem/filestream/i_file_factory.h"

#include "score/os/stat.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace score::filesystem
{

namespace details
{
std::string ComposeTempFilename(std::string_view original_filename,
                                std::size_t threa_id_hash,
                                std::uint64_t timestamp) noexcept;
}

/// @brief Production implementation of IFileFactory. Will create actual file streams.
class FileFactory final : public IFileFactory
{
  public:
    FileFactory() noexcept = default;
    virtual ~FileFactory() noexcept = default;
    FileFactory(const FileFactory&) = default;
    FileFactory& operator=(const FileFactory&) = default;
    FileFactory(FileFactory&&) noexcept = default;
    FileFactory& operator=(FileFactory&&) noexcept = default;

    Result<std::unique_ptr<std::iostream>> Open(const Path&, const std::ios_base::openmode mode) override;

    // as intended, we don't enforce users to specify ownership flags unless needed
    // defaults for override and base function are the same thus static binding is safe
    // NOLINTNEXTLINE(google-default-arguments) : see above
    Result<std::unique_ptr<FileStream>> AtomicUpdate(
        const Path& path,
        const std::ios_base::openmode mode,
        const AtomicUpdateOwnershipFlags ownership_flag = kUseTargetFileUID | kUseTargetFileGID) override;
};

namespace details
{

template <typename Buf, typename... Args>
Result<std::unique_ptr<FileStream>> CreateFileStream(Args&&... args)
{
    Buf filebuf{std::forward<Args>(args)...};
    // LCOV_EXCL_BR_START Tooling issue, both branches show coverage, so decisions are also both covered (Ticket-202112)
    if (filebuf.is_open() != true)
    // LCOV_EXCL_BR_STOP
    {
        return MakeUnexpected(ErrorCode::kCouldNotOpenFileStream);
    }
    else
    {
        return std::make_unique<details::FileStreamImpl<Buf>>(std::move(filebuf));
    }
}

Result<int> OpenFileHandle(const Path& path,
                           const std::ios_base::openmode mode,
                           const os::Stat::Mode create_mode) noexcept;

struct IdentityMetadata
{
    // Suppress "AUTOSAR C++14 M11-0-1" rule findings. This rule states: "Member data in non-POD class types shall
    // be private.". We need these data elements to be organized into a coherent organized data structure.
    // coverity[autosar_cpp14_m11_0_1_violation]
    os::Stat::Mode mode{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    uid_t uid{};
    // coverity[autosar_cpp14_m11_0_1_violation]
    gid_t gid{};
};

Result<IdentityMetadata> GetIdentityMetadata(const Path& path);

}  // namespace details

}  // namespace score::filesystem

#endif  // SCORE_LIB_FILESYSTEM_FILE_FACTORY_H
