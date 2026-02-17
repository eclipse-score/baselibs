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
#ifndef SCORE_MW_LOG_DETAIL_TEXT_RECORDER_NON_BLOCKING_WRITER_H
#define SCORE_MW_LOG_DETAIL_TEXT_RECORDER_NON_BLOCKING_WRITER_H

// Be careful what you include here. Each additional header will be included in logging.h and thus exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include "score/os/unistd.h"
#include "score/mw/log/detail/error.h"

#include <score/span.hpp>
#include <climits>
#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief NonBlockingWriter Class to write logs into file.
///
/// \details this class is using write system call to write log  messages into file in non blocking manner.
class NonBlockingWriter final
{
  public:
    enum class Result : std::uint8_t
    {
        kWouldBlock = 0,
        kDone,
    };
    /// \brief Constructor that accepts fileHandle of the file to flush data into it and max_chunk size controlled by
    /// user and have limit to kMaxChunkSizeSupportedByOs.
    explicit NonBlockingWriter(const std::int32_t file_handle,
                               std::size_t max_chunk_size,
                               score::cpp::pmr::unique_ptr<score::os::Unistd> unistd) noexcept;

    /// \brief method to write buffer contents to the given file handle in non blocking manner with SSIZE_MAX.
    /// Returns Result::kDone when all the data has been written
    score::cpp::expected<Result, score::mw::log::detail::Error> FlushIntoFile() noexcept;

    /// \brief Method to Re initialize the current instance of the non blocking writer to be used to flush another span.
    void SetSpan(const score::cpp::span<const std::uint8_t>& buffer) noexcept;

    static std::size_t GetMaxChunkSize() noexcept;

  private:
    score::cpp::expected<ssize_t, score::os::Error> InternalFlush(const uint64_t size_to_flush) noexcept;

    score::cpp::pmr::unique_ptr<score::os::Unistd> unistd_;

    std::int32_t file_handle_;  // given file handle to write to it.
    uint64_t
        number_of_flushed_bytes_;  // last written byte location to be used to continue writing in other flush calls.
    score::cpp::span<const std::uint8_t> buffer_;  // the sent buffer to flush data from it to the file.
    Result buffer_flushed_;                 // Internal flag used to raise it once the whole buffer is flushed.
    std::size_t max_chunk_size_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_TEXT_RECORDER_NON_BLOCKING_WRITER_H
