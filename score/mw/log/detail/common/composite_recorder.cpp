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
#include "score/mw/log/detail/common/composite_recorder.h"

#include "score/result/result.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/initialization_reporter.h"

#include "score/callback.hpp"

#include <cstring>

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

// Deviation from Rule A0-1-1:
// - A project shall not contain instances of non-volatile variables being given values that are not subsequently used.
// Justification:
// - It's false positive, as kRecorderWithRecorderIdCallbackCapacity is already used.
// coverity[autosar_cpp14_a0_1_1_violation : FALSE]
constexpr std::size_t kRecorderWithRecorderIdCallbackCapacity = 64UL;
using RecorderWithRecorderIdCallback =
    score::cpp::callback<void(Recorder&, SlotHandle::RecorderIdentifier), kRecorderWithRecorderIdCallbackCapacity>;

/// \brief Iterates over all recorders and invokes a callback.
void ForEachRecorder(const std::vector<std::unique_ptr<Recorder>>& recorders,
                     const RecorderWithRecorderIdCallback callback) noexcept
{
    const auto begin = recorders.begin();
    for (auto it = recorders.begin(); it < recorders.end(); it++)
    {
        const auto index = static_cast<std::size_t>(std::distance(begin, it));
        callback(*it->get(), SlotHandle::RecorderIdentifier{index});
    }
}

using RecorderWithSlotCallback = score::cpp::callback<void(Recorder&, const SlotHandle&)>;

/// \brief Iterates over all recorders with an active slot.
/// \details For each recorder, we check if the composite_slot contains a corresponding slot handle. If the slot handle
/// is available we invoke the callback with the pair of concrete recorder and corresponding slot.
void ForEachActiveSlot(const std::vector<std::unique_ptr<Recorder>>& recorders,
                       const SlotHandle composite_slot,
                       const RecorderWithSlotCallback callback) noexcept
{
    SlotHandle slot_for_recorder{};
    ForEachRecorder(recorders,
                    [&slot_for_recorder, &composite_slot, &callback](Recorder& recorder,
                                                                     const SlotHandle::RecorderIdentifier recorder_id) {
                        if (composite_slot.IsRecorderActive(recorder_id))
                        {
                            slot_for_recorder.SetSlot(composite_slot.GetSlot(recorder_id));
                            callback(recorder, slot_for_recorder);
                        }
                    });
}

template <typename T>
void LogForEachActiveSlot(const std::vector<std::unique_ptr<Recorder>>& recorders,
                          const SlotHandle& composite_slot,
                          const T& arg)
{
    // LCOV_EXCL_START : no branches to test
    ForEachActiveSlot(recorders, composite_slot, [arg](Recorder& recorder, const SlotHandle& slot) noexcept {
        recorder.Log(slot, arg);
    });
    // LCOV_EXCL_STOP
}
}  // namespace

CompositeRecorder::CompositeRecorder(std::vector<std::unique_ptr<Recorder>> recorders) noexcept
    : Recorder{}, recorders_{std::move(recorders)}
{
    if (recorders_.size() > SlotHandle::kMaxRecorders)
    {
        ReportInitializationError(Error::kMaximumNumberOfRecordersExceeded);
        recorders_.resize(SlotHandle::kMaxRecorders);
    }
}

score::cpp::optional<SlotHandle> CompositeRecorder::StartRecord(const std::string_view context_id,
                                                         const LogLevel log_level) noexcept
{
    SlotHandle composite_slot{};

    ForEachRecorder(recorders_,
                    [&context_id, &log_level, &composite_slot](Recorder& recorder,
                                                               const SlotHandle::RecorderIdentifier recorder_id) {
                        const auto result = recorder.StartRecord(context_id, log_level);
                        if (result.has_value())
                        {
                            // LCOV_EXCL_START : no branches to test
                            composite_slot.SetSlot(result->GetSlotOfSelectedRecorder(), recorder_id);
                            // LCOV_EXCL_STOP
                        }
                    });

    return composite_slot;
}

void CompositeRecorder::StopRecord(const SlotHandle& slot) noexcept
{
    ForEachActiveSlot(recorders_, slot, [](Recorder& recorder, const SlotHandle& composite_slot) noexcept {
        recorder.StopRecord(composite_slot);
    });
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const bool data) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, data);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::uint8_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::int8_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}
void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::uint16_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::int16_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::uint32_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::int32_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::uint64_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::int64_t arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const float arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const double arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const std::string_view arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogHex8 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogHex16 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogHex32 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogHex64 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogBin8 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogBin16 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogBin32 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogBin64 arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.value);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogRawBuffer arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg);
}

void CompositeRecorder::Log(const SlotHandle& composite_slot, const LogSlog2Message arg) noexcept
{
    LogForEachActiveSlot(recorders_, composite_slot, arg.GetMessage());
}

const std::vector<std::unique_ptr<Recorder>>& CompositeRecorder::GetRecorders() const noexcept
{
    return recorders_;
}

bool CompositeRecorder::IsLogEnabled(const LogLevel& log_level, const std::string_view context) const noexcept
{
    // Return true if at least one recorder is enabled.

    bool is_log_enabled = false;

    ForEachRecorder(
        recorders_,
        [&is_log_enabled, log_level, context](const auto& recorder, const SlotHandle::RecorderIdentifier) noexcept {
            is_log_enabled = is_log_enabled || recorder.IsLogEnabled(log_level, context);
        });

    return is_log_enabled;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
