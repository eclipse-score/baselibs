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
#include "score/mw/log/detail/file_logging/console_recorder_factory.h"
#include "score/mw/log/detail/file_logging/file_output_backend.h"
#include "score/mw/log/detail/file_logging/text_message_builder.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
std::unique_ptr<Recorder> ConsoleRecorderFactory::CreateConcreteLogRecorder(const Configuration& config,
                                                                            score::cpp::pmr::memory_resource* memory_resource)
{
    auto backend = CreateConsoleLoggingBackend(config, memory_resource);
    constexpr bool check_log_level_for_console = true;
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
}

std::unique_ptr<Backend> ConsoleRecorderFactory::CreateConsoleLoggingBackend(
    const Configuration& config,
    score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    auto message_builder = std::make_unique<TextMessageBuilder>(config.GetEcuId());
    auto allocator = std::make_unique<CircularAllocator<LogRecord>>(config.GetNumberOfSlots(),
                                                                    LogRecord{config.GetSlotSizeInBytes()});

    return std::make_unique<FileOutputBackend>(std::move(message_builder),
                                               STDOUT_FILENO,
                                               "",
                                               std::move(allocator),
                                               score::os::FcntlImpl::Default(memory_resource),
                                               score::os::Unistd::Default(memory_resource),
                                               false,
                                               false,
                                               0,
                                               1,
                                               false);
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
