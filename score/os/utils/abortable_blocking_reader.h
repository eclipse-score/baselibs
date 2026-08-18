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
#ifndef SCORE_LIB_OS_UTILS_ABORTABLE_BLOCKING_READER_H
#define SCORE_LIB_OS_UTILS_ABORTABLE_BLOCKING_READER_H

#include "score/os/errno.h"
#include "score/os/fcntl.h"
#include "score/os/sys_poll.h"
#include "score/os/unistd.h"

#include <score/expected.hpp>
#include <score/span.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <shared_mutex>

namespace score
{
namespace os
{

/// \brief Makes a file descriptor non-blocking and closes it on destruction
class NonBlockingFileDescriptor
{
  public:
    static score::cpp::expected<NonBlockingFileDescriptor, Error> Make(const std::int32_t file_descriptor) noexcept;
    static score::cpp::expected<NonBlockingFileDescriptor, Error> Make(const std::int32_t file_descriptor,
                                                                       Fcntl& fcntl,
                                                                       std::shared_ptr<Unistd> unistd) noexcept;

    NonBlockingFileDescriptor() = default;
    ~NonBlockingFileDescriptor();

    NonBlockingFileDescriptor(const NonBlockingFileDescriptor&) = delete;
    NonBlockingFileDescriptor& operator=(const NonBlockingFileDescriptor&) & = delete;
    NonBlockingFileDescriptor(NonBlockingFileDescriptor&& other) noexcept;
    NonBlockingFileDescriptor& operator=(NonBlockingFileDescriptor&& other) & noexcept;

    std::int32_t GetUnderlying() const noexcept;

    // NOLINTNEXTLINE(*-explicit-constructor) Intentional implicit conversion to use this class naturally in POSIX API
    operator std::int32_t() const noexcept;

  private:
    std::shared_ptr<Unistd> unistd_{nullptr};
    std::int32_t file_descriptor_{-1};

    explicit NonBlockingFileDescriptor(const std::int32_t file_descriptor, std::shared_ptr<Unistd> unistd) noexcept;

    void CloseFileDescriptor() noexcept;
};

/// \brief A helper class to use blocking reads on file descriptors.
class AbortableBlockingReader
{
  public:
    AbortableBlockingReader() noexcept;
    AbortableBlockingReader(std::shared_ptr<Fcntl> fcntl,
                            std::shared_ptr<SysPoll> syspoll,
                            std::shared_ptr<Unistd> unistd) noexcept;
    ~AbortableBlockingReader();

    AbortableBlockingReader(const AbortableBlockingReader&) = delete;
    AbortableBlockingReader& operator=(const AbortableBlockingReader&) & = delete;
    AbortableBlockingReader(AbortableBlockingReader&& other) noexcept = delete;
    AbortableBlockingReader& operator=(AbortableBlockingReader&& other) & noexcept = delete;

    /// \brief Returns the success of the internal setup at construction
    score::cpp::expected_blank<Error> IsValid() const noexcept;

    /// \brief Stops the reader and unblocks all pending read operations.
    /// Once stopped, the reader can no longer be used for new read operations.
    void Stop() noexcept;

    /// \brief Performs a blocking read on the provided file_descriptor
    ///
    /// Internally calls poll followed by a read if the file descriptor becomes ready before the class is destructed.
    /// May be used concurrently on multiple file_descriptors.
    ///
    /// \param file_descriptor the file descriptor to read from (Only blocks if does not point to a regular file)
    /// \param buffer the buffer to read into
    /// \return error or the span of read data
    ///         Will return Error::Code::kOperationWasInterruptedBySignal if AbortableBlockingReader is destructed while
    ///         waiting for data.
    score::cpp::expected<score::cpp::span<std::uint8_t>, Error> Read(
        const NonBlockingFileDescriptor& file_descriptor,
        const score::cpp::span<std::uint8_t> buffer) noexcept;

  private:
    std::shared_ptr<Fcntl> fcntl_;
    std::shared_ptr<SysPoll> syspoll_;
    std::shared_ptr<Unistd> unistd_;

    std::shared_timed_mutex mutex_;
    score::cpp::expected_blank<Error> construction_error_;
    NonBlockingFileDescriptor stop_read_file_descriptor_;
    NonBlockingFileDescriptor stop_write_file_descriptor_;

    void SignalStop() noexcept;

    score::cpp::expected_blank<Error> WaitForData(const NonBlockingFileDescriptor& file_descriptor) noexcept;

    static score::cpp::expected<std::pair<NonBlockingFileDescriptor, NonBlockingFileDescriptor>, Error>
    MakeNonBlockingPipe(Fcntl& fcntl, const std::shared_ptr<Unistd>& unistd) noexcept;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_ABORTABLE_BLOCKING_READER_H
