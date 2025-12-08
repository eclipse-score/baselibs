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
#include "score/mw/log/detail/recorder_factory_stub.h"
#include "score/os/fcntl_impl.h"
#include "score/os/unistd.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/file_logging/file_output_backend.h"
#include "score/mw/log/detail/file_logging/text_message_builder.h"
#include "score/mw/log/detail/file_logging/text_recorder.h"
#include "score/mw/log/log_mode.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

std::unique_ptr<Recorder> RecorderFactory::CreateStub() const noexcept
{
    return std::make_unique<EmptyRecorder>();
}

std::unique_ptr<Backend> CreateConsoleLoggingBackend(const Configuration& config,
                                                     score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    auto message_builder = std::make_unique<TextMessageBuilder>(config.GetEcuId());
    auto allocator = std::make_unique<CircularAllocator<LogRecord>>(config.GetNumberOfSlots(),
                                                                    LogRecord{config.GetSlotSizeInBytes()});
    return std::make_unique<FileOutputBackend>(std::move(message_builder),
                                               STDOUT_FILENO,
                                               std::move(allocator),
                                               score::os::FcntlImpl::Default(memory_resource),
                                               score::os::Unistd::Default(memory_resource),
                                               config.IsCircularFileLogging(),
                                               false, // overwrite log on full
                                               config.GetMaxLogFileSizeBytes(),
                                               1,     // no of log files
                                               false); // delete old log files
}

std::unique_ptr<Recorder> RecorderFactory::CreateFromConfiguration(
    [[maybe_unused]] score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    return CreateWithConsoleLoggingOnly(memory_resource);
}

std::unique_ptr<Recorder> RecorderFactory::CreateWithConsoleLoggingOnly(
    [[maybe_unused]] score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    const Configuration config;  //  using all default values
    if (nullptr == memory_resource)
    {
        memory_resource = score::cpp::pmr::get_default_resource();
    }
    auto backend = CreateConsoleLoggingBackend(config, memory_resource);
    constexpr bool check_log_level_for_console = false;
    return std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
}

std::unique_ptr<score::mw::log::IRecorderFactory> CreateRecorderFactory() noexcept
{
    return std::make_unique<RecorderFactory>();
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
