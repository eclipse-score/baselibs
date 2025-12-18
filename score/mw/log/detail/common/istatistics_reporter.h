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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_ISTATISTICS_REPORTER_H
#define SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_ISTATISTICS_REPORTER_H

#include "score/mw/log/recorder.h"

#include <chrono>
namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief This class shall provide logging-related statistics.
/// \details For instance it shall report the reason and number of dropped messages.
/// The public interface shall be implemented in a thread-safe way, except for the SetRecorder method that should only
/// be called in the initialization phase.
class IStatisticsReporter
{
  public:
    IStatisticsReporter() noexcept = default;
    IStatisticsReporter(IStatisticsReporter&&) noexcept = delete;
    IStatisticsReporter(const IStatisticsReporter&) noexcept = delete;
    IStatisticsReporter& operator=(IStatisticsReporter&&) noexcept = delete;
    IStatisticsReporter& operator=(const IStatisticsReporter&) noexcept = delete;

    virtual ~IStatisticsReporter() noexcept;

    /// \brief Increment the counter that shows the number of dropped messages due to no free slot available.
    /// \details This must be implemented in a thread safe way.
    virtual void IncrementNoSlotAvailable() noexcept = 0;

    /// \brief Increment the counter that shows the number of dropped messages due to no free slot available.
    /// \details This must be implemented in a thread safe way.
    virtual void IncrementMessageTooLong() noexcept = 0;

    /// \brief Send a statistics report if needed.
    /// \details This method shall be called periodically so that the implementation is able to send a report.
    /// Not every call to Update() will trigger a report, but only few reports are send in a specific interval.
    virtual void Update(const std::chrono::steady_clock::time_point& now) noexcept = 0;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_COMMON_DATA_ROUTER_ISTATISTICS_REPORTER_H
