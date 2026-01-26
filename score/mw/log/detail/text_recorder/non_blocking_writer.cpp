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
#include "score/filesystem/path.h"
#include "score/os/fcntl.h"
#include "score/os/stat.h"
#include <string>

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

NonBlockingWriter::NonBlockingWriter(const std::string& file_path,
                                     std::size_t max_chunk_size,
                                     score::cpp::pmr::unique_ptr<score::os::Unistd> unistd,
                                     score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl,
                                     const bool circular_file_logging,
                                     const bool overwrite_log_on_full,
                                     const std::size_t max_log_file_size_bytes,
                                     const std::size_t no_of_log_files,
                                     const bool delete_old_log_files) noexcept
    : unistd_{std::move(unistd)},
      fcntl_{std::move(fcntl)},
      number_of_flushed_bytes_{0U},
      buffer_{nullptr, 0UL},
      buffer_flushed_{Result::kWouldBlock},
      max_chunk_size_(std::min(max_chunk_size, GetMaxChunkSize())),
      circular_file_logging_(circular_file_logging),
      overwrite_log_on_full_(overwrite_log_on_full),
      max_log_file_size_bytes_(max_log_file_size_bytes),
      current_file_position_{0U},
      no_of_log_files_(no_of_log_files),
      delete_old_log_files_(delete_old_log_files),
      current_log_file_index_{1}
{
    const score::filesystem::Path path(file_path);
    file_path_ = path.ParentPath().Native();
    file_extension_ = path.Extension().Native();
    file_name_ = path.Stem().Native();

    std::string initial_file_path = file_path;
    if (no_of_log_files_ > 1)
    {
        initial_file_path = file_path_ + "/" + file_name_ + "_" + std::to_string(current_log_file_index_) + file_extension_;
    }

    auto open_flags =
    score::os::Fcntl::Open::kReadWrite | score::os::Fcntl::Open::kCreate | score::os::Fcntl::Open::kCloseOnExec;

    // NOLINTBEGIN(score-banned-function): FileLoggingBackend is disabled in production. Argumentation: Ticket-75726
    const auto descriptor_result = fcntl_->open(
        initial_file_path.data(),
        open_flags,
        score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser | score::os::Stat::Mode::kReadGroup |
            score::os::Stat::Mode::kReadOthers);

    if (descriptor_result.has_value())
    {
        file_handle_ = descriptor_result.value();
        const auto flags = fcntl_->fcntl(file_handle_, score::os::Fcntl::Command::kFileGetStatusFlags);
        if (flags.has_value())
        {
            std::ignore = fcntl_->fcntl(
                file_handle_,
                score::os::Fcntl::Command::kFileSetStatusFlags,
                flags.value() | score::os::Fcntl::Open::kNonBlocking | score::os::Fcntl::Open::kCloseOnExec);
        }
    }

    if (circular_file_logging_ && max_log_file_size_bytes_ > 0 && file_handle_ != -1)
    {
        const auto file_size_or_error = unistd_->lseek(file_handle_, 0, SEEK_END);
        if (file_size_or_error.has_value())
        {
            current_file_position_ = static_cast<uint64_t>(file_size_or_error.value());
        }
        // If lseek fails, current_file_position_ remains 0, which is a safe fallback.
    }
}

NonBlockingWriter::NonBlockingWriter(const std::string& file_path,
                                     std::int32_t file_descriptor,
                                     std::size_t max_chunk_size,
                                     score::cpp::pmr::unique_ptr<score::os::Unistd> unistd,
                                     score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl,
                                     const bool circular_file_logging,
                                     const bool overwrite_log_on_full,
                                     const std::size_t max_log_file_size_bytes,
                                     const std::size_t no_of_log_files,
                                     const bool delete_old_log_files) noexcept
    : unistd_{std::move(unistd)},
      fcntl_{std::move(fcntl)},
      file_handle_{file_descriptor},
      number_of_flushed_bytes_{0U},
      buffer_{nullptr, 0UL},
      buffer_flushed_{Result::kWouldBlock},
      max_chunk_size_(std::min(max_chunk_size, GetMaxChunkSize())),
      circular_file_logging_(circular_file_logging),
      overwrite_log_on_full_(overwrite_log_on_full),
      max_log_file_size_bytes_(max_log_file_size_bytes),
      current_file_position_{0U},
      no_of_log_files_(no_of_log_files),
      delete_old_log_files_(delete_old_log_files),
      current_log_file_index_{1}
{
    const score::filesystem::Path path(file_path);
    file_path_ = path.ParentPath().Native();
    file_extension_ = path.Extension().Native();
    file_name_ = path.Stem().Native();
    if (no_of_log_files_ > 1 && file_name_.size() > 2 && file_name_.substr(file_name_.size() - 2) == "_1")
    {
        file_name_ = file_name_.substr(0, file_name_.size() - 2);
    }
    const auto flags = fcntl_->fcntl(file_handle_, score::os::Fcntl::Command::kFileGetStatusFlags);
    if (flags.has_value())
    {
        std::ignore = fcntl_->fcntl(
            file_handle_,
            score::os::Fcntl::Command::kFileSetStatusFlags,
            flags.value() | score::os::Fcntl::Open::kNonBlocking | score::os::Fcntl::Open::kCloseOnExec);
    }

    if (circular_file_logging_ && max_log_file_size_bytes_ > 0 && file_handle_ != -1)
    {
        const auto file_size_or_error = unistd_->lseek(file_handle_, 0, SEEK_END);
        if (file_size_or_error.has_value())
        {
            current_file_position_ = static_cast<uint64_t>(file_size_or_error.value());
        }
        // If lseek fails, current_file_position_ remains 0, which is a safe fallback.
    }
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
    if (file_handle_ == -1)
    {
        if (rotation_failed_)
        {
            return 0;  // A rotation attempt already failed, do nothing.
        }
        RotateLogFile();
        if (file_handle_ == -1)
        {
            return 0; // Rotation failed, do nothing.
        }
    }

    const auto buffer_size = static_cast<uint64_t>(buffer_.size());
    if (number_of_flushed_bytes_ >= buffer_size)
    {
        return 0;  // Nothing left to flush
    }

    // --- Multi-file rotation logic ---
    if (circular_file_logging_ && no_of_log_files_ > 1)
    {
        // If the file size is limited and the next write would exceed it, rotate the file.
        if (max_log_file_size_bytes_ > 0 && (current_file_position_ + size_to_flush > max_log_file_size_bytes_))
        {
            RotateLogFile();
            if (rotation_failed_)
            {
                return 0; // A rotation attempt just failed, do nothing.
            }
        }

        // After potential rotation, we write to the current file.
        // We must still guard against a single write being larger than the max file size.
        const uint64_t remaining_space = (max_log_file_size_bytes_ > 0)
                                           ? (max_log_file_size_bytes_ - current_file_position_)
                                           : size_to_flush;
        const uint64_t write_size = std::min(size_to_flush, remaining_space);

        if (write_size == 0 && max_log_file_size_bytes_ > 0)
        {
            // This can happen if the file is exactly full. The next call will rotate.
            return 0;
        }

        auto num_of_bytes_written =
            unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), write_size);

        if (!num_of_bytes_written.has_value())
        {
            return score::cpp::make_unexpected(num_of_bytes_written.error());
        }

        const auto written_count = static_cast<uint64_t>(num_of_bytes_written.value());
        number_of_flushed_bytes_ += written_count;
        current_file_position_ += written_count;
        return num_of_bytes_written.value();
    }

    // --- Fallback to simple write or single-file circular logging ---

    // If circular logging is not enabled, perform a simple write.
    if (!circular_file_logging_ || max_log_file_size_bytes_ == 0) {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto num_of_bytes_written =
            unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), size_to_flush);
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (!num_of_bytes_written.has_value())
        {
            return score::cpp::make_unexpected(num_of_bytes_written.error());
        }
        const auto written_count = static_cast<uint64_t>(num_of_bytes_written.value());
        number_of_flushed_bytes_ += written_count;
        return num_of_bytes_written.value();
    }

    // --- Circular logging is enabled from this point on (and it's single-file mode) ---

    if (!overwrite_log_on_full_)
    {
        // CASE: Circular logging, NO overwrite.
        if (current_file_position_ >= max_log_file_size_bytes_)
        {
            const std::string console_warning = "WARNING: Log file size limit reached. Logging has stopped.\n";
            unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
            return 0; // File is full, stop writing.
        }

        const uint64_t remaining_space = max_log_file_size_bytes_ - current_file_position_;
        const uint64_t write_size = std::min(size_to_flush, remaining_space);

        if (write_size == 0) {
            return 0;
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        auto num_of_bytes_written =
            unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), write_size);
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (!num_of_bytes_written.has_value())
        {
            return score::cpp::make_unexpected(num_of_bytes_written.error());
        }

        const auto written_count = static_cast<uint64_t>(num_of_bytes_written.value());
        number_of_flushed_bytes_ += written_count;
        current_file_position_ += written_count;
        return num_of_bytes_written.value();
    }
    else
    {
        // CASE: Circular logging, WITH overwrite.
        if (current_file_position_ + size_to_flush <= max_log_file_size_bytes_)
        {
            // The write fits without wrapping.
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            auto num_of_bytes_written =
                unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), size_to_flush);
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            if (!num_of_bytes_written.has_value())
            {
                return score::cpp::make_unexpected(num_of_bytes_written.error());
            }
            const auto written_count = static_cast<uint64_t>(num_of_bytes_written.value());
            number_of_flushed_bytes_ += written_count;
            current_file_position_ += written_count;
            return num_of_bytes_written.value();
        }
        else
        {
            // The write crosses the boundary and needs to wrap.
            const std::string console_warning = "WARNING: Log file size limit reached. Wrapping around.\n";
            unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());

            // Seek to the beginning of the file to overwrite.
            const auto seek_result = unistd_->lseek(file_handle_, 0, SEEK_SET);
            if (!seek_result.has_value())
            {
                // If seek fails, we cannot proceed with the circular write.
                rotation_failed_ = true;
                return score::cpp::make_unexpected(seek_result.error());
            }

            const uint64_t write_size = std::min(size_to_flush, max_log_file_size_bytes_);

            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            auto num_of_bytes_written =
                unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), write_size);
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            if (!num_of_bytes_written.has_value())
            {
                return score::cpp::make_unexpected(num_of_bytes_written.error());
            }

            const auto written_count = static_cast<uint64_t>(num_of_bytes_written.value());
            number_of_flushed_bytes_ += written_count;
            current_file_position_ = written_count;
            rotation_failed_ = false; // Success
            return num_of_bytes_written.value();
        }
    }
}

void NonBlockingWriter::RotateLogFile() noexcept
{
    if (no_of_log_files_ <= 1)
    {
        return;
    }
    if (file_handle_ != -1)
    {
        unistd_->close(file_handle_);
        file_handle_ = -1;  // Invalidate handle immediately after closing.
    }

    current_log_file_index_++;
    if (current_log_file_index_ > no_of_log_files_)
    {
        current_log_file_index_ = 1;
    }

    std::string next_file_path = file_path_ + "/" + file_name_ + "_" + std::to_string(current_log_file_index_) + file_extension_;

    // Check for file existence using the Unistd wrapper.
    const auto access_result = unistd_->access(next_file_path.c_str(), score::os::Unistd::AccessMode::kExists);
    if (!access_result.has_value() && access_result.error() != score::os::Error::Code::kNoSuchFileOrDirectory)
    {
        rotation_failed_ = true;
        const std::string console_warning = "ERROR: Could not access next log file: " + next_file_path + "\n";
        unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
        return;
    }
    const bool file_exists = access_result.has_value();

    if (file_exists && !overwrite_log_on_full_)
    {
        const std::string console_warning = "WARNING: Log file rotation stopped. File exists and overwrite is disabled.\n";
        unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
        current_file_position_ = 0;
        rotation_failed_ = true; // Set failure flag
        return;  // Stop, leaving file_handle_ as -1.
    }

    auto open_flags =
        score::os::Fcntl::Open::kWriteOnly | score::os::Fcntl::Open::kCreate | score::os::Fcntl::Open::kCloseOnExec;

    // Only truncate if both overwrite and delete are enabled, matching user's expectation.
    if (overwrite_log_on_full_ && delete_old_log_files_)
    {
        open_flags |= score::os::Fcntl::Open::kTruncate;
    }

    const auto descriptor_result = fcntl_->open(
        next_file_path.data(),
        open_flags,
        score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser | score::os::Stat::Mode::kReadGroup |
            score::os::Stat::Mode::kReadOthers);

    if (descriptor_result.has_value())
    {
        file_handle_ = descriptor_result.value();

        const auto flags = fcntl_->fcntl(file_handle_, score::os::Fcntl::Command::kFileGetStatusFlags);
        if (!flags.has_value())
        {
            rotation_failed_ = true;
            const std::string console_warning = "ERROR: Could not get flags for new log file: " + next_file_path + "\n";
            unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
            unistd_->close(file_handle_);
            file_handle_ = -1;
            return;
        }

        auto fcntl_result = fcntl_->fcntl(
            file_handle_,
            score::os::Fcntl::Command::kFileSetStatusFlags,
            flags.value() | score::os::Fcntl::Open::kNonBlocking | score::os::Fcntl::Open::kCloseOnExec);

        if (!fcntl_result.has_value())
        {
            rotation_failed_ = true;
            const std::string console_warning = "ERROR: Could not set flags for new log file: " + next_file_path + "\n";
            unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
            unistd_->close(file_handle_);
            file_handle_ = -1;
            return;
        }


        if (file_exists && overwrite_log_on_full_ && !delete_old_log_files_)
        {
            const auto seek_result = unistd_->lseek(file_handle_, 0, SEEK_SET);
            if (!seek_result.has_value())
            {
                rotation_failed_ = true;
                const std::string console_warning = "ERROR: Could not seek to beginning of log file: " + next_file_path + "\n";
                unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
                unistd_->close(file_handle_);
                file_handle_ = -1;
                return;
            }
        }
        rotation_failed_ = false; // Rotation fully succeeded
    }
    else
    {
        // Handle remains -1 from earlier.
        const std::string console_warning = "ERROR: Could not open next log file: " + next_file_path + "\n";
        unistd_->write(STDERR_FILENO, console_warning.c_str(), console_warning.length());
        rotation_failed_ = true; // Set failure flag
    }

    current_file_position_ = 0;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
