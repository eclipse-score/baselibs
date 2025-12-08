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
#include "score/mw/log/detail/file_logging/file_recorder_factory.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/file_logging/dlt_message_builder.h"
#include "score/mw/log/detail/file_logging/file_output_backend.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
std::unique_ptr<Recorder> FileRecorderFactory::CreateConcreteLogRecorder(const Configuration& config,
                                                                         score::cpp::pmr::memory_resource* memory_resource)
{
    auto backend = CreateFileLoggingBackend(config, memory_resource);
    if (backend == nullptr)
    {
        return std::make_unique<EmptyRecorder>();
    }
    return std::make_unique<FileRecorder>(config, std::move(backend));
}

std::unique_ptr<Backend> FileRecorderFactory::CreateFileLoggingBackend(
    const Configuration& config,
    score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    const std::string file_path_base{std::string(config.GetLogFilePath().data(), config.GetLogFilePath().size())};
    const std::string app_id{std::string{config.GetAppId().data(), config.GetAppId().size()}};
    const std::string file_extension{".dlt"};
    std::string file_name{file_path_base + "/" + app_id + file_extension};
    if (config.GetNoOfLogFiles() > 1) {
        file_name = file_path_base + "/" + app_id + "_1" + file_extension;
    }

    auto open_flags =
        score::os::Fcntl::Open::kReadWrite | score::os::Fcntl::Open::kCreate | score::os::Fcntl::Open::kCloseOnExec;

    // NOLINTBEGIN(score-banned-function): FileLoggingBackend is disabled in production. Argumentation: Ticket-75726
    const auto descriptor_result = fcntl_->open(
        file_name.data(),
        open_flags,
        score::os::Stat::Mode::kReadUser | score::os::Stat::Mode::kWriteUser | score::os::Stat::Mode::kReadGroup |
            score::os::Stat::Mode::kReadOthers);
    // NOLINTEND(score-banned-function): see above for detailed explanation

    // 'descriptor' is intentionally initialized to 0 to ensure it has a defined value.
    // Even though this initial value is overwritten when 'descriptor_result' holds a valid value,
    // the initialization is required to avoid potential usage of an uninitialized variable and to satisfy our coding
    // style.
    // coverity[autosar_cpp14_a0_1_1_violation]
    std::int32_t descriptor{};

    if (descriptor_result.has_value())
    {
        descriptor = descriptor_result.value();
    }
    else
    {
        ReportInitializationError(Error::kLogFileCreationFailed, descriptor_result.error().ToString());
        return nullptr;
    }

    auto message_builder = std::make_unique<DltMessageBuilder>(config.GetEcuId());
    auto allocator = std::make_unique<CircularAllocator<LogRecord>>(config.GetNumberOfSlots(),
                                                                    LogRecord{config.GetSlotSizeInBytes()});

    return std::make_unique<FileOutputBackend>(std::move(message_builder),
                                               descriptor,
                                               file_name,
                                               std::move(allocator),
                                               score::os::Fcntl::Default(memory_resource),
                                               score::os::Unistd::Default(memory_resource),
                                               config.IsCircularFileLogging(),
                                               config.IsOverwriteLogOnFull(),
                                               config.GetMaxLogFileSizeBytes(),
                                               config.GetNoOfLogFiles(),
                                               config.IsDeleteOldLogFiles());
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
