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

#pragma once

#include <optional>
#include <string>

namespace score::mw::log::rust
{

/// @brief Represents severity of a log message.
enum class LogLevel
{
    Off,
    Fatal,
    Error,
    Warn,
    Info,
    Debug,
    Verbose
};

/// @brief
/// Builder for logger used by Rust libraries.
///
/// @note
/// If parameter is not set explicitly then Rust-side default is used.
/// Only global logger setup is allowed.
class StdoutLoggerBuilder final
{
  public:
    /// @brief Set context for the logger.
    /// @param context
    /// Context name.
    /// @return This builder.
    StdoutLoggerBuilder& Context(const std::string& context) noexcept;

    /// @brief Show module name in logs.
    /// @param show_module Value to set.
    /// @return This builder.
    StdoutLoggerBuilder& ShowModule(bool show_module) noexcept;

    /// @brief Show file name in logs.
    /// @param show_module Value to set.
    /// @return This builder.
    StdoutLoggerBuilder& ShowFile(bool show_file) noexcept;

    /// @brief Show line number in logs.
    /// @param show_module Value to set.
    /// @return This builder.
    StdoutLoggerBuilder& ShowLine(bool show_line) noexcept;

    /// @brief Show timestamp.
    /// UTC timestamp in the following format:
    /// `[year]/[month]/[day] [hour]:[minute]:[second].[subsecond digits:7]`
    ///
    /// Example:
    /// `2026/01/27 11:33:41.1420089`
    /// @param show_timestamp Value to set.
    /// @return This builder.
    StdoutLoggerBuilder& ShowTimestamp(bool show_timestamp) noexcept;

    /// @brief Filter logs by level.
    /// @param log_level Log level.
    /// @return This builder.
    StdoutLoggerBuilder& LogLevel(LogLevel log_level) noexcept;

    /// @brief Initialize default logger with provided parameters.
    void SetAsDefaultLogger() noexcept;

  private:
    std::optional<std::string> context_;
    std::optional<bool> show_module_;
    std::optional<bool> show_file_;
    std::optional<bool> show_line_;
    std::optional<bool> show_timestamp_;
    std::optional<score::mw::log::rust::LogLevel> log_level_;
};

}  // namespace score::mw::log::rust
