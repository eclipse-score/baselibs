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
#include "score/mw/log/detail/file_recorder/file_recorder.h"
#include "score/mw/log/detail/common/dlt_format.h"
#include "score/mw/log/detail/dlt_argument_counter.h"

#include "score/os/fcntl_impl.h"
#include "score/os/stat.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

template <typename T>
// LogData is not static storage. function is used to log different types
// coverity[autosar_cpp14_a2_10_4_violation : FALSE]
// coverity[autosar_cpp14_a2_10_1_violation]
inline void LogData(const SlotHandle& slot, detail::Backend& backend, const T data) noexcept
{
    auto& log_record = backend.GetLogRecord(slot);
    DltArgumentCounter counter{log_record.getLogEntry().num_of_args};
    auto& payload = log_record.getVerbosePayload();
    std::ignore = counter.TryAddArgument([data, &payload]() {
        const auto result = DLTFormat::Log(payload, data);
        return result;
    });
}

}  //  anonymous namespace

FileRecorder::FileRecorder(const detail::Configuration& config, std::unique_ptr<detail::Backend> backend)
    : Recorder(),
      backend_(std::move(backend)),
      config_(config)  // LCOV_EXCL_BR_LINE: there are no branches to be covered, it is just pre-construction.
{
}

score::cpp::optional<SlotHandle> FileRecorder::StartRecord(const std::string_view context_id,
                                                    const LogLevel log_level) noexcept
{
    if (IsLogEnabled(log_level, context_id) == false)
    {
        return {};
    }

    auto slot_handle = backend_->ReserveSlot();
    if (slot_handle.has_value())
    {
        auto& log_record = backend_->GetLogRecord(slot_handle.value());
        auto& log_entry = log_record.getLogEntry();

        const auto app_id = config_.GetAppId();
        log_entry.app_id = detail::LoggingIdentifier{app_id};
        log_entry.ctx_id = detail::LoggingIdentifier{context_id};
        log_entry.num_of_args = 0U;
        log_entry.log_level = log_level;
        log_record.getVerbosePayload().Reset();
    }
    return slot_handle;
}

void FileRecorder::StopRecord(const SlotHandle& slot) noexcept
{
    backend_->FlushSlot(slot);
}

void FileRecorder::Log(const SlotHandle& slot, const bool data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::uint8_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::int8_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::uint16_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::int16_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::uint32_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::int32_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::uint64_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::int64_t data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const float data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const double data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const std::string_view data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogHex8 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogHex16 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogHex32 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogHex64 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogBin8 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogBin16 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogBin32 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogBin64 data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogRawBuffer data) noexcept
{
    LogData(slot, *backend_, data);
}

void FileRecorder::Log(const SlotHandle& slot, const LogSlog2Message data) noexcept
{
    LogData(slot, *backend_, data.GetMessage());
}

bool FileRecorder::IsLogEnabled(const LogLevel& log_level, const std::string_view context) const noexcept
{
    return config_.IsLogLevelEnabled(log_level, context);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
