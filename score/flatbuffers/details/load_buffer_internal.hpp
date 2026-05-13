/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#ifndef SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_INTERNAL_HPP
#define SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_INTERNAL_HPP

#include "score/filesystem/path.h"
#include "score/os/errno.h"

#include "score/os/fcntl_impl.h"
#include "score/os/stat_impl.h"
#include "score/os/unistd.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <new>

namespace score
{

namespace flatbuffers
{

namespace detail
{

struct OS
{
    score::os::FcntlImpl fcntl{};
    score::os::StatImpl stat{};
    score::os::internal::UnistdImpl unistd{};
};

// OST must expose three members: fcntl (open), stat (fstat), unistd (close/read).
// Container must provide resize() and data().
template <class OST = OS, class Container>
score::os::Result<score::cpp::blank> LoadBufferImpl(const OST& os,
                                                    const score::filesystem::Path& path,
                                                    Container& data) noexcept
{
    const auto fd_result = os.fcntl.open(path.CStr(), score::os::Fcntl::Open::kReadOnly);
    if (!fd_result.has_value())
    {
        return score::cpp::make_unexpected(fd_result.error());
    }
    const std::int32_t file_desc = fd_result.value();

    // Helper to ensure the file descriptor in an error case is always closed.
    // Any close error is only reported when no prior error occurred.
    auto close_fd = [&os,
                     file_desc](const score::os::Error* prior_error) noexcept -> score::os::Result<score::cpp::blank> {
        const auto close_result = os.unistd.close(file_desc);
        if (prior_error != nullptr)
        {
            return score::cpp::make_unexpected(*prior_error);
        }
        return close_result;
    };

    // Obtain the file size via fstat
    score::os::StatBuffer stat_buf{};
    const auto stat_result = os.stat.fstat(file_desc, stat_buf);
    if (!stat_result.has_value())
    {  // defensive error handling
        // fstat failure is impossible on a just-opened valid fd. Mocks are used to
        // cover this line in unit tests.
        return close_fd(&stat_result.error());
    }

    if (stat_buf.st_size < 0)
    {  // defensive error handling
        // No real Linux filesystem reports negative sizes. Mocks are used to cover
        // this line in unit tests.
        const auto err = score::os::Error::createFromErrno(EINVAL);
        return close_fd(&err);
    }

    const auto file_size = static_cast<std::size_t>(stat_buf.st_size);

    try
    {
        data.resize(file_size);
    }
    catch (const std::bad_alloc&)
    {
        const auto err = score::os::Error::createFromErrno(ENOMEM);
        return close_fd(&err);
    }
    catch (...)
    {  // potential custom exception
        const auto err = score::os::Error::createUnspecifiedError();
        return close_fd(&err);
    }

    // Read the entire file, handling partial reads
    std::size_t total_bytes_read = 0U;
    while (total_bytes_read < file_size)
    {
        const auto read_result = os.unistd.read(file_desc,
                                                std::next(data.data(), static_cast<std::ptrdiff_t>(total_bytes_read)),
                                                file_size - total_bytes_read);
        if (!read_result.has_value())
        {
            const auto& read_error = read_result.error();
            // Retry on EINTR (interrupted system call)
            if (read_error == score::os::Error::Code::kOperationWasInterruptedBySignal)
            {  // The EINTR retry requires a signal to arrive during a regular-file
                // read syscall, which is non-deterministic. Mocks are used to cover
                // this line in unit tests.
                continue;
            }
            return close_fd(&read_error);
        }

        const auto bytes_read = read_result.value();
        if (bytes_read == 0)
        {
            // Unexpected EOF before reading the full file
            const auto err = score::os::Error::createFromErrno(EIO);
            return close_fd(&err);
        }

        total_bytes_read += static_cast<std::size_t>(bytes_read);
    }

    return close_fd(nullptr);
}

}  // namespace detail
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_LOAD_BUFFER_INTERNAL_HPP
