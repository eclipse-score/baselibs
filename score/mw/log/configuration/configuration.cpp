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
#include "score/mw/log/configuration/configuration.h"

#include "score/mw/log/detail/logging_identifier.h"

#include <score/callback.hpp>

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

std::string ToString(const std::string_view view)
{
    return {view.data(), view.size()};
}

}  // namespace

std::string_view Configuration::GetEcuId() const noexcept
{
    return ecu_id_.GetStringView();
}

void Configuration::SetEcuId(const std::string_view ecu_id) noexcept
{
    ecu_id_ = LoggingIdentifier{ecu_id};
}

std::string_view Configuration::GetAppId() const noexcept
{
    return app_id_.GetStringView();
}

void Configuration::SetAppId(const std::string_view app_id) noexcept
{
    app_id_ = LoggingIdentifier{app_id};
}

std::string_view Configuration::GetAppDescription() const noexcept
{
    return app_description_;
}

void Configuration::SetAppDescription(const std::string_view app_description) noexcept
{
    app_description_ = ToString(app_description);
}

const std::unordered_set<LogMode>& Configuration::GetLogMode() const noexcept
{
    return log_mode_;
}

void Configuration::SetLogMode(const std::unordered_set<LogMode>& log_mode) noexcept
{
    log_mode_ = log_mode;
}

std::string_view Configuration::GetLogFilePath() const noexcept
{
    return log_file_path_;
}

void Configuration::SetLogFilePath(const std::string_view log_file_path) noexcept
{
    log_file_path_ = ToString(log_file_path);
}

LogLevel Configuration::GetDefaultLogLevel() const noexcept
{
    return default_log_level_;
}

void Configuration::SetDefaultLogLevel(const LogLevel default_log_level) noexcept
{
    default_log_level_ = default_log_level;
}

LogLevel Configuration::GetDefaultConsoleLogLevel() const noexcept
{
    return default_console_log_level_;
}

void Configuration::SetDefaultConsoleLogLevel(const LogLevel default_console_log_level) noexcept
{
    default_console_log_level_ = default_console_log_level;
}

const ContextLogLevelMap& Configuration::GetContextLogLevel() const noexcept
{
    return context_log_level_;
}

void Configuration::SetContextLogLevel(const ContextLogLevelMap& context_log_level) noexcept
{
    context_log_level_ = context_log_level;
}

std::size_t Configuration::GetStackBufferSize() const noexcept
{
    return stack_buffer_size_;
}

void Configuration::SetStackBufferSize(const std::size_t stack_buffer_size) noexcept
{
    stack_buffer_size_ = stack_buffer_size;
}

std::size_t Configuration::GetRingBufferSize() const noexcept
{
    return ring_buffer_size_;
}

void Configuration::SetRingBufferSize(const std::size_t ring_buffer_size) noexcept
{
    ring_buffer_size_ = ring_buffer_size;
}

bool Configuration::GetRingBufferOverwriteOnFull() const noexcept
{
    return ring_buffer_overwrite_on_full_;
}

void Configuration::SetRingBufferOverwriteOnFull(const bool ring_buffer_overwrite_on_full) noexcept
{
    ring_buffer_overwrite_on_full_ = ring_buffer_overwrite_on_full;
}

std::size_t Configuration::GetNumberOfSlots() const noexcept
{
    return number_of_slots_;
}

void Configuration::SetNumberOfSlots(const std::size_t number_of_slots) noexcept
{
    number_of_slots_ = number_of_slots;
}

std::size_t Configuration::GetSlotSizeInBytes() const noexcept
{
    return slot_size_bytes_;
}

void Configuration::SetSlotSizeInBytes(const std::size_t slot_size_bytes) noexcept
{
    slot_size_bytes_ = slot_size_bytes;
}

bool Configuration::IsLogLevelEnabled(const LogLevel& log_level,
                                      const std::string_view context,
                                      const bool check_for_console) const noexcept
{
    auto max_log_level = LogLevel::kOff;

    const auto context_log_level_result = context_log_level_.find(LoggingIdentifier{context});
    const auto log_level_is_in_context = context_log_level_result != context_log_level_.cend();
    if (log_level_is_in_context)
    {
        max_log_level = context_log_level_result->second;
    }
    else if (check_for_console == false)
    {
        max_log_level = default_log_level_;
    }
    else
    {
        max_log_level = default_console_log_level_;
    }

    return log_level <= max_log_level;
}

void Configuration::SetDataRouterUid(const std::size_t uid) noexcept
{
    data_router_uid_ = uid;
}

std::size_t Configuration::GetDataRouterUid() const noexcept
{
    return data_router_uid_;
}

bool Configuration::GetDynamicDatarouterIdentifiers() const noexcept
{
    return dynamic_datarouter_identifiers_;
}

void Configuration::SetDynamicDatarouterIdentifiers(const bool enable_dynamic_identifiers) noexcept
{
    dynamic_datarouter_identifiers_ = enable_dynamic_identifiers;
}
bool Configuration::IsCircularFileLogging() const noexcept
{
    return circular_file_logging_;
}

void Configuration::SetCircularFileLogging(const bool circular_file_logging) noexcept
{
    circular_file_logging_ = circular_file_logging;
}

std::size_t Configuration::GetMaxLogFileSizeBytes() const noexcept
{
    return max_log_file_size_bytes_;
}

void Configuration::SetMaxLogFileSizeBytes(const std::size_t max_log_file_size_bytes) noexcept
{
    max_log_file_size_bytes_ = max_log_file_size_bytes;
}

bool Configuration::IsOverwriteLogOnFull() const noexcept
{
    return overwrite_log_on_full_;
}

void Configuration::SetOverwriteLogOnFull(const bool overwrite_log_on_full) noexcept
{
    // The JSON configuration key for this parameter is "overwriteLogOnFull".
    overwrite_log_on_full_ = overwrite_log_on_full;
}

std::size_t Configuration::GetNoOfLogFiles() const noexcept
{
    return no_of_log_files_;
}

void Configuration::SetNoOfLogFiles(const std::size_t no_of_log_files) noexcept
{
    no_of_log_files_ = no_of_log_files;
}

bool Configuration::IsTruncateOnRotation() const noexcept
{
    return truncate_on_rotation_;
}

void Configuration::SetTruncateOnRotation(const bool truncate_on_rotation) noexcept
{
    truncate_on_rotation_ = truncate_on_rotation;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
