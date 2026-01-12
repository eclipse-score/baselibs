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
#include "score/mw/log/detail/text_recorder/text_recorder.h"

#include "score/mw/log/detail/dlt_argument_counter.h"
#include "score/mw/log/detail/log_record.h"
#include "score/mw/log/detail/text_recorder/text_format.h"

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
inline void GenericLog(const SlotHandle& slot_handle, detail::Backend& backend, const T data) noexcept
{
    auto& log_record = backend.GetLogRecord(slot_handle);

    detail::DltArgumentCounter counter{log_record.getLogEntry().num_of_args};
    std::ignore = counter.TryAddArgument([data, &log_record]() noexcept {
        if (log_record.getVerbosePayload()
                .RemainingCapacity() >  // LCOV_EXCL_BR_LINE: lcov complains about lots of uncovered branches, it is
                                        // not convenient/related to this condition.
            0U)
        {
            detail::TextFormat::Log(log_record.getVerbosePayload(), data);
            return detail::AddArgumentResult::Added;
        }
        else
        {
            return detail::AddArgumentResult::NotAdded;
        }
    });
}

inline void SlogGenericLog(const SlotHandle& slot_handle, detail::Backend& backend, const LogSlog2Message data) noexcept
{
// QNX-specific recorder
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined __QNX__
    auto& log_record = backend.GetLogRecord(slot_handle);
    auto& log_entry = log_record.getLogEntry();
    log_entry.slog2_code = data.GetCode();
// QNX-specific recorder
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

    GenericLog(slot_handle, backend, data.GetMessage());
}

}  //  anonymous namespace

TextRecorder::TextRecorder(const detail::Configuration& config,
                           std::unique_ptr<detail::Backend> backend,
                           const bool check_log_level_for_console) noexcept
    : Recorder(),
      backend_(std::move(backend)),
      config_(config),
      check_log_level_for_console_{check_log_level_for_console}
{
}

score::cpp::optional<SlotHandle> TextRecorder::StartRecord(const std::string_view context_id,
                                                    const LogLevel log_level) noexcept
{
    if (IsLogEnabled(log_level, context_id) == false)
    {
        return {};
    }

    auto slot_handle = backend_->ReserveSlot();
    if (slot_handle.has_value())
    {
        auto& payload = backend_->GetLogRecord(slot_handle.value());
        auto& log_entry = payload.getLogEntry();

        const auto app_id = config_.GetAppId();
        log_entry.app_id = detail::LoggingIdentifier{app_id};
        log_entry.ctx_id = detail::LoggingIdentifier{context_id};
        log_entry.num_of_args = 0U;
        log_entry.log_level = log_level;
        payload.getVerbosePayload().Reset();
    }

    return slot_handle;
}

void TextRecorder::StopRecord(const SlotHandle& slot) noexcept
{
    backend_->FlushSlot(slot);
}

bool TextRecorder::IsLogEnabled(const LogLevel& log_level, const std::string_view context) const noexcept
{
    return config_.IsLogLevelEnabled(log_level, context, check_log_level_for_console_);
}

void TextRecorder::Log(const SlotHandle& slot, const bool data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::uint8_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::int8_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::uint16_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::int16_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::uint32_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::int32_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::uint64_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::int64_t data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const float data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const double data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogRawBuffer data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const std::string_view data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogHex8 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogHex16 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogHex32 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogHex64 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogBin8 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogBin16 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogBin32 data) noexcept
{
    GenericLog(slot, *backend_, data);
}
void TextRecorder::Log(const SlotHandle& slot, const LogBin64 data) noexcept
{
    GenericLog(slot, *backend_, data);
}

void TextRecorder::Log(const SlotHandle& slot, const LogSlog2Message data) noexcept
{
    SlogGenericLog(slot, *backend_, data);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
