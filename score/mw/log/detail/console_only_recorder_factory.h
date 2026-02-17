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
#ifndef SCORE_MW_LOG_DETAIL_CONSOLE_ONLY_RECORDER_FACTORY_H
#define SCORE_MW_LOG_DETAIL_CONSOLE_ONLY_RECORDER_FACTORY_H

#include "score/mw/log/irecorder_factory.h"
#include "score/mw/log/recorder.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
class RecorderFactory : public IRecorderFactory
{
  public:
    RecorderFactory() noexcept = default;
    std::unique_ptr<Recorder> CreateStub() const noexcept override;
    std::unique_ptr<Recorder> CreateWithConsoleLoggingOnly(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;
    std::unique_ptr<Recorder> CreateFromConfiguration(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;
    ~RecorderFactory() noexcept = default;
};
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_CONSOLE_ONLY_RECORDER_FACTORY_H
