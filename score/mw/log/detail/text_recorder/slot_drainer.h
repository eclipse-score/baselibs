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
#ifndef MW_LOG_DETAILS_SLOT_DRAINER_H_
#define MW_LOG_DETAILS_SLOT_DRAINER_H_

#include "score/mw/log/detail/circular_allocator.h"
#include "score/mw/log/detail/log_record.h"
#include "score/mw/log/detail/text_recorder/imessage_builder.h"
#include "score/mw/log/detail/text_recorder/non_blocking_writer.h"
#include "score/mw/log/slot_handle.h"

#include <score/circular_buffer.hpp>
#include <score/span.hpp>

#include <memory>
#include <mutex>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class SlotDrainer
{
  public:
    enum class FlushResult : std::uint8_t
    {
        kAllDataProcessed = 0,
        kPartiallyProcessed,
        kNumberOfProcessedSlotsExceeded,
    };
    SlotDrainer(std::unique_ptr<IMessageBuilder> message_builder,
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
                const std::size_t limit_slots_in_one_cycle = 32UL);

    SlotDrainer(SlotDrainer&&) noexcept = delete;
    SlotDrainer(const SlotDrainer&) noexcept = delete;
    SlotDrainer& operator=(SlotDrainer&&) noexcept = delete;
    SlotDrainer& operator=(const SlotDrainer&) noexcept = delete;

    void PushBack(const SlotHandle& slot) noexcept;
    void Flush() noexcept;

    ~SlotDrainer();

  private:
    score::cpp::expected<FlushResult, score::mw::log::detail::Error> TryFlushSlots() noexcept;
    score::cpp::expected<FlushResult, score::mw::log::detail::Error> TryFlushSpans() noexcept;
    bool MoreSlotsAvailableAndLoaded() noexcept;
    bool MoreSpansAvailableAndLoaded() noexcept;

    std::shared_ptr<CircularAllocator<LogRecord>> allocator_;
    std::unique_ptr<IMessageBuilder> message_builder_;
    std::mutex context_mutex_;
    // used in circular_buffer_ declaration
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    static constexpr std::size_t kMaxCircularBufferSize = 1024UL;
    //  TODO: assert size of circular_buffer:
    score::cpp::circular_buffer<SlotHandle, kMaxCircularBufferSize> circular_buffer_;
    //  To manually release resource and to set and reset access:
    score::cpp::optional<std::reference_wrapper<const SlotHandle>> current_slot_;
    NonBlockingWriter non_blocking_writer_;
    const std::size_t limit_slots_in_one_cycle_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  MW_LOG_DETAILS_SLOT_DRAINER_H_
