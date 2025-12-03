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
#include "score/mw/log/detail/slog/slog_recorder_factory.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
std::unique_ptr<Recorder> SlogRecorderFactory::CreateConcreteLogRecorder(const Configuration& config,
                                                                         score::cpp::pmr::memory_resource* memory_resource)
{
    auto backend = CreateSystemBackend(config, memory_resource);  // LCOV_EXCL_LINE : no branches to test
    constexpr bool check_log_level_for_console = false;
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
}

std::unique_ptr<Backend> SlogRecorderFactory::CreateSystemBackend(const Configuration& config,
                                                                  score::cpp::pmr::memory_resource* memory_resource)
{
    return std::make_unique<SlogBackend>(config.GetNumberOfSlots(),
                                         LogRecord{config.GetSlotSizeInBytes()},
                                         config.GetAppId(),
                                         score::os::qnx::Slog2Impl::Default(memory_resource));
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
