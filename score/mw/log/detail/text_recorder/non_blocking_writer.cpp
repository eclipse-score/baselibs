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
#include "score/mw/log/detail/text_recorder/non_blocking_writer.h"

#if defined(__QNXNTO__)
#include <sys/iomsg.h>
#endif  //  __QNXNTO__

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

std::size_t NonBlockingWriter::GetMaxChunkSize() noexcept
{
/// \brief Maximum number of bytes to be flushed in one call.
/// For QNX The Max size of bytes to be written shall be less than SSIZE_MAX - sizeof(io_write_t)
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined __QNX__
    constexpr std::size_t kMaxChunkSizeSupportedByOs = static_cast<std::size_t>(SSIZE_MAX) - sizeof(io_write_t);
// For QNX The Max size of bytes to be written shall be less than SSIZE_MAX - sizeof(io_write_t)
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    constexpr std::size_t kMaxChunkSizeSupportedByOs = static_cast<std::size_t>(SSIZE_MAX);
// For QNX The Max size of bytes to be written shall be less than SSIZE_MAX - sizeof(io_write_t)
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    return kMaxChunkSizeSupportedByOs;
}

NonBlockingWriter::NonBlockingWriter(const int32_t file_handle,
                                     std::size_t max_chunk_size,
                                     score::cpp::pmr::unique_ptr<score::os::Unistd> unistd) noexcept
    : unistd_{std::move(unistd)},
      file_handle_(file_handle),
      number_of_flushed_bytes_{0U},
      buffer_{nullptr, 0UL},
      buffer_flushed_{Result::kWouldBlock},
      max_chunk_size_(std::min(max_chunk_size, GetMaxChunkSize()))

{
}

void NonBlockingWriter::SetSpan(const score::cpp::span<const std::uint8_t>& buffer) noexcept
{
    buffer_flushed_ = Result::kWouldBlock;
    number_of_flushed_bytes_ = 0U;
    buffer_ = buffer;
}

score::cpp::expected<NonBlockingWriter::Result, score::mw::log::detail::Error> NonBlockingWriter::FlushIntoFile() noexcept
{
    const auto buffer_size = static_cast<uint64_t>(buffer_.size());

    const uint64_t left_over = buffer_size - number_of_flushed_bytes_;

    const uint64_t bytes_to_write = max_chunk_size_ <= left_over ? max_chunk_size_ : left_over;

    const auto flush_output = InternalFlush(bytes_to_write);

    if (!(flush_output.has_value()))
    {
        return score::cpp::make_unexpected(score::mw::log::detail::Error::kUnknownError);
    }

    if (number_of_flushed_bytes_ == buffer_size)
    {
        buffer_flushed_ = Result::kDone;
    }

    return buffer_flushed_;
}

score::cpp::expected<ssize_t, score::os::Error> NonBlockingWriter::InternalFlush(const uint64_t size_to_flush) noexcept
{
    const auto buffer_size = static_cast<uint64_t>(buffer_.size());
    if (number_of_flushed_bytes_ < buffer_size)
    {
        score::cpp::expected<ssize_t, score::os::Error> num_of_bytes_written{};
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
        // NOLINTBEGIN(score-banned-function) it is among safety headers.
        // Needs ptr to access score::cpp::span elements
        // coverity[autosar_cpp14_m5_0_15_violation]
        num_of_bytes_written = unistd_->write(file_handle_, &(buffer_[number_of_flushed_bytes_]), size_to_flush);
        // NOLINTEND(score-banned-function) it is among safety headers.
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
        if (!num_of_bytes_written.has_value())
        {
            return score::cpp::make_unexpected(num_of_bytes_written.error());
        }
        else
        {
            number_of_flushed_bytes_ += static_cast<uint64_t>(num_of_bytes_written.value());
        }
    }
    return number_of_flushed_bytes_;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
