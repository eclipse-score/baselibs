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
#include "score/mw/log/detail/file_logging/slot_drainer.h"

#include <algorithm>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

SlotDrainer::SlotDrainer(std::unique_ptr<IMessageBuilder> message_builder,
                         std::shared_ptr<CircularAllocator<LogRecord>> allocator,
                         const std::int32_t file_descriptor,
                         const std::string& file_path,
                         score::cpp::pmr::unique_ptr<score::os::Unistd> unistd,
                         score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl,
                         const bool circular_file_logging,
                         const bool overwrite_log_on_full,
                         const std::size_t max_log_file_size_bytes,
                         const std::size_t no_of_log_files,
                         const bool delete_old_log_files,
                         const std::size_t limit_slots_in_one_cycle)
    : allocator_(allocator),
      message_builder_(std::move(message_builder)),
      non_blocking_writer_(file_path,
                           file_descriptor,
                           NonBlockingWriter::GetMaxChunkSize(),
                           std::move(unistd),
                           std::move(fcntl),
                           circular_file_logging,
                           overwrite_log_on_full,
                           max_log_file_size_bytes,
                           no_of_log_files,
                           delete_old_log_files),
      limit_slots_in_one_cycle_(limit_slots_in_one_cycle)
{
}

bool SlotDrainer::MoreSpansAvailableAndLoaded() noexcept
{
    //  remaining span flushed, try next span:
    const auto span = message_builder_->GetNextSpan();
    if (span.has_value())
    {
        non_blocking_writer_.SetSpan(span.value());
        return true;
    }
    return false;
}

score::cpp::expected<SlotDrainer::FlushResult, score::mw::log::detail::Error> SlotDrainer::TryFlushSpans() noexcept
{
    do
    {
        //  First try to flush remaining data from previous cycle:
        const auto status = non_blocking_writer_.FlushIntoFile();
        if (status.has_value())
        {
            if (status.value() != NonBlockingWriter::Result::kDone)
            {
                return FlushResult::kPartiallyProcessed;
            }
        }
        else
        {
            return score::cpp::make_unexpected(status.error());
        }
    } while (MoreSpansAvailableAndLoaded());

    return FlushResult::kAllDataProcessed;  //  no more entries
}

bool SlotDrainer::MoreSlotsAvailableAndLoaded() noexcept
{
    if (circular_buffer_.empty())
    {
        return false;
    }
    auto& reference_to_slot = circular_buffer_.front();
    circular_buffer_.pop_front();
    current_slot_ = reference_to_slot;

    //  Casting to more capable integer type:
    auto& underlying_data =
        allocator_->GetUnderlyingBufferFor(static_cast<std::size_t>(reference_to_slot.GetSlotOfSelectedRecorder()));
    message_builder_->SetNextMessage(underlying_data);
    return true;
}

score::cpp::expected<SlotDrainer::FlushResult, score::mw::log::detail::Error> SlotDrainer::TryFlushSlots() noexcept
{
    std::size_t number_of_processed_slots = 0U;
    do
    {
        const auto status = TryFlushSpans();
        if ((!status.has_value()) || (FlushResult::kPartiallyProcessed == status.value()))
        {
            //  error or "would block" -> return error or 'false'.
            return status;
        }
        //  slot is flushed, try next one:
        if (current_slot_.has_value())  // manually release slot
        {
            //  Casting to more capable integer type:
            allocator_->ReleaseSlot(static_cast<std::size_t>(current_slot_.value().get().GetSlotOfSelectedRecorder()));
            current_slot_.reset();
        }

        if (number_of_processed_slots > limit_slots_in_one_cycle_)
        {
            return FlushResult::kNumberOfProcessedSlotsExceeded;
        }
        ++number_of_processed_slots;
    } while (MoreSlotsAvailableAndLoaded());

    return FlushResult::kAllDataProcessed;
}

void SlotDrainer::PushBack(const SlotHandle& slot) noexcept
{
    const std::lock_guard<std::mutex> lock(context_mutex_);
    circular_buffer_.push_back(slot);
}

void SlotDrainer::Flush() noexcept
{
    const std::lock_guard<std::mutex> lock(context_mutex_);
    std::ignore = TryFlushSlots();
}

SlotDrainer::~SlotDrainer()
{
    //  Try to flush residual data:
    Flush();
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
