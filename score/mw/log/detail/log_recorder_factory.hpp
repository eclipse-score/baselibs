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
#ifndef SCORE_MW_LOG_DETAIL_LOG_RECORDER_FACTORY_HPP
#define SCORE_MW_LOG_DETAIL_LOG_RECORDER_FACTORY_HPP

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/recorder.h"
#include "score/os/fcntl.h"
#include <score/memory.hpp>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
template <typename DerivedRecorder>
class LogRecorderFactory
{
  public:
    std::unique_ptr<Recorder> CreateLogRecorder(const Configuration& config,
                                                score::cpp::pmr::memory_resource* memory_resource)
    {
        return static_cast<DerivedRecorder*>(this)->CreateConcreteLogRecorder(config, memory_resource);
    }

  private:
    // prevent wrong inheritance
    LogRecorderFactory() = default;
    ~LogRecorderFactory() = default;
    LogRecorderFactory(LogRecorderFactory&) = delete;
    LogRecorderFactory(LogRecorderFactory&&) = delete;
    LogRecorderFactory& operator=(LogRecorderFactory&) = delete;
    LogRecorderFactory& operator=(LogRecorderFactory&&) = delete;

    /*
    Deviation from rule: A11-3-1
    - "Friend declarations shall not be used."
    Justification:
    - friend class and private constructor used here to prevent wrong inheritance
    */
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend DerivedRecorder;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_LOG_RECORDER_FACTORY_HPP
