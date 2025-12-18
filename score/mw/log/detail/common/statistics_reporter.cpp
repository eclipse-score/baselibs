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

#include "score/mw/log/detail/common/statistics_reporter.h"

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

bool IsReportOverdue(const std::chrono::steady_clock::time_point& now,
                     const std::int64_t& last_report_time_nsec,
                     const std::chrono::seconds& report_interval) noexcept
{
    if (std::chrono::duration_cast<std::chrono::seconds>(
            now - std::chrono::steady_clock::time_point{std::chrono::nanoseconds{last_report_time_nsec}}) >=
        report_interval)
    {
        return true;
    }
    return false;
}

void ReportStatisticsViaRecorder(Recorder& recorder,
                                 const std::size_t& no_slot_available_counter,
                                 const std::size_t& message_too_long_counter,
                                 const std::size_t& number_of_slots,
                                 const std::size_t& slot_size_bytes) noexcept
{
    LogLevel log_level = LogLevel::kInfo;
    if ((no_slot_available_counter > 0UL) || (message_too_long_counter > 0UL))
    {
        log_level = LogLevel::kWarn;
    }

    const auto slot = recorder.StartRecord("STAT", log_level);

    if (slot.has_value() == false)
    {
        return;
    }
    recorder.Log(slot.value(), std::string_view{"mw::log statistics: number_of_slots="});
    recorder.Log(slot.value(), number_of_slots);
    recorder.Log(slot.value(), std::string_view{", slot_size_bytes="});
    recorder.Log(slot.value(), slot_size_bytes);
    recorder.Log(slot.value(), std::string_view{", no_slot_available_counter="});
    recorder.Log(slot.value(), no_slot_available_counter);
    recorder.Log(slot.value(), std::string_view{", message_too_long_counter="});
    recorder.Log(slot.value(), message_too_long_counter);
    recorder.StopRecord(slot.value());
}

}  // namespace

StatisticsReporter::StatisticsReporter(Recorder& recorder,
                                       const std::chrono::seconds report_interval,
                                       const std::size_t number_of_slots,
                                       const std::size_t slot_size_bytes) noexcept
    : IStatisticsReporter{},
      recorder_{recorder},
      report_interval_{report_interval},
      number_of_slots_{number_of_slots},
      slot_size_bytes_{slot_size_bytes},
      no_slot_available_counter_{},
      message_too_long_counter_{},
      last_report_time_point_nanoseconds_{},
      currently_reporting_{}
{
}

void StatisticsReporter::IncrementNoSlotAvailable() noexcept
{
    no_slot_available_counter_++;
}

void StatisticsReporter::IncrementMessageTooLong() noexcept
{
    message_too_long_counter_++;
}

void StatisticsReporter::Update(const std::chrono::steady_clock::time_point& now) noexcept
{
    if (IsReportOverdue(now, last_report_time_point_nanoseconds_.load(), report_interval_) == false)
    {
        return;
    }

    // Try to acquire the reporting state
    bool current_reporting_expected_false = false;
    if (currently_reporting_.compare_exchange_weak(current_reporting_expected_false, true) == false)
    {
        // Give up because another thread is already reporting.
        return;
    }

    ReportStatisticsViaRecorder(recorder_,
                                no_slot_available_counter_.load(),
                                message_too_long_counter_.load(),
                                number_of_slots_,
                                slot_size_bytes_);
    last_report_time_point_nanoseconds_ = std::chrono::nanoseconds{now.time_since_epoch()}.count();

    // Release reporting state.
    currently_reporting_ = false;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
