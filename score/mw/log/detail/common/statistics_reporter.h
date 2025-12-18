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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_STATISTICS_REPORTER_H
#define SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_STATISTICS_REPORTER_H

#include "score/mw/log/recorder.h"

#include "score/mw/log/detail/common/istatistics_reporter.h"

#include <score/optional.hpp>

#include <atomic>
#include <chrono>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class StatisticsReporter final : public IStatisticsReporter
{
  public:
    explicit StatisticsReporter(Recorder&,
                                const std::chrono::seconds report_interval,
                                const std::size_t number_of_slots,
                                const std::size_t slot_size_bytes) noexcept;
    void IncrementNoSlotAvailable() noexcept override;
    void IncrementMessageTooLong() noexcept override;
    void Update(const std::chrono::steady_clock::time_point& now) noexcept override;

  private:
    Recorder& recorder_;
    std::chrono::seconds report_interval_;
    std::size_t number_of_slots_;
    std::size_t slot_size_bytes_;
    std::atomic<std::size_t> no_slot_available_counter_;
    std::atomic<std::size_t> message_too_long_counter_;
    std::atomic<std::int64_t> last_report_time_point_nanoseconds_;
    std::atomic_bool currently_reporting_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_STATISTICS_REPORTER_H
