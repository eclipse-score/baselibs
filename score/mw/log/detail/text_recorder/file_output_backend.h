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
#ifndef MW_LOG_DETAIL_FILE_OUTPUT_BACKEND_H_
#define MW_LOG_DETAIL_FILE_OUTPUT_BACKEND_H_

#include "score/mw/log/detail/backend.h"
#include "score/mw/log/detail/circular_allocator.h"
#include "score/mw/log/detail/text_recorder/slot_drainer.h"

#include "score/os/fcntl_impl.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class FileOutputBackend final : public Backend
{
  public:
    FileOutputBackend(std::unique_ptr<IMessageBuilder> message_builder,
                      const std::int32_t file_descriptor,
                      const std::string& file_path,
                      std::unique_ptr<CircularAllocator<LogRecord>> allocator,
                      score::cpp::pmr::unique_ptr<score::os::Fcntl> fcntl,
                      score::cpp::pmr::unique_ptr<score::os::Unistd> unistd,
                      const bool circular_file_logging,
                      const bool overwrite_log_on_full,
                      const std::size_t max_log_file_size_bytes,
                      const std::size_t no_of_log_files,
                      const bool truncate_on_rotation) noexcept;
    /// \brief Before a producer can store data in our buffer, he has to reserve a slot.
    ///
    /// \return SlotHandle if a slot was able to be reserved, empty otherwise.
    ///
    /// \post This ensures that no other thread will write to the reserved slot until FlushSlot() is invoked.
    score::cpp::optional<SlotHandle> ReserveSlot() noexcept override;

    /// \brief After a producer finished writing into a slot Flush() needs to be called.
    ///
    /// \param slot The slot that shall be flushed
    ///
    /// \pre ReserveSlot() was invoked to get a SlotHandle that shall be flushed
    /// \post This ensures that afterwards the respective slot can be either read or overwritten
    void FlushSlot(const SlotHandle& slot) noexcept override;

    /// \brief In order to stream data into a slot, the underlying slot buffer needs to be exposed.
    ///
    /// \param slot The slot for which the associated buffer shall be returned
    /// \return The payload associated with slot. (Where a producer can add its data)
    ///
    /// \pre ReserveSlot() was invoked to receive a SlotHandle
    LogRecord& GetLogRecord(const SlotHandle& slot) noexcept override;

  private:
    // shared with SlotDrainer
    std::shared_ptr<CircularAllocator<LogRecord>> buffer_allocator_;
    SlotDrainer slot_drainer_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  MW_LOG_DETAIL_FILE_OUTPUT_BACKEND_H_
