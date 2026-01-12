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
#include "score/filesystem/filestream/file_buf.h"

#include "score/filesystem/error.h"
#include "score/os/stdio.h"
#include "score/os/unistd.h"

namespace score::filesystem::details
{

ResultBlank StdioFileBuf::Close()
{
    if (is_open())
    {
        if (close() == nullptr)
        {
            return MakeUnexpected(ErrorCode::kCloseFailed, "Unable to close file descriptor file buffer");
        }
    }
    return {};
}

AtomicFileBuf::AtomicFileBuf(int fd, std::ios::openmode mode, Path from_path, Path to_path)
    : StdioFileBuf{fd, mode}, from_path_{std::move(from_path)}, to_path_{std::move(to_path)}
{
}

ResultBlank AtomicFileBuf::Close()
{
    ResultBlank result{};

    if (is_open())
    {
        if (sync() != 0)
        {
            return MakeUnexpected(ErrorCode::kFsyncFailed);
        }

        if (!os::Unistd::instance().fsync(fd()).has_value())
        {
            return MakeUnexpected(ErrorCode::kFsyncFailed);
        }

        if (close() == nullptr)
        {
            // If closing fails, do not try to rename since we might replace a working
            // file with a corrupted one.
            constexpr std::string_view user_message{"Unable to close file descriptor file buffer during atomic update"};
            return MakeUnexpected(ErrorCode::kCloseFailed, user_message);
        }

        if (auto rename_result = os::Stdio::instance().rename(from_path_.CStr(), to_path_.CStr());
            !rename_result.has_value())
        {
            return MakeUnexpected(ErrorCode::kCouldNotRenameFile);
        }
    }

    // False positive, result is initialized at the beginning of the method.
    // coverity[autosar_cpp14_a8_5_0_violation]
    return result;
}

}  // namespace score::filesystem::details
