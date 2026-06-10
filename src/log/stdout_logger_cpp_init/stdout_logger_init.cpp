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

#include "stdout_logger_init.h"

extern "C" void set_default_logger(const char* context_ptr,
                                   size_t context_size,
                                   const bool* show_module,
                                   const bool* show_file,
                                   const bool* show_line,
                                   const bool* show_timestamp,
                                   const score::mw::log::rust::LogLevel* log_level);

namespace score::mw::log::rust
{

StdoutLoggerBuilder& StdoutLoggerBuilder::Context(const std::string& context) noexcept
{
    context_ = context;
    return *this;
}

StdoutLoggerBuilder& StdoutLoggerBuilder::ShowModule(bool show_module) noexcept
{
    show_module_ = show_module;
    return *this;
}

StdoutLoggerBuilder& StdoutLoggerBuilder::ShowFile(bool show_file) noexcept
{
    show_file_ = show_file;
    return *this;
}

StdoutLoggerBuilder& StdoutLoggerBuilder::ShowLine(bool show_line) noexcept
{
    show_line_ = show_line;
    return *this;
}

StdoutLoggerBuilder& StdoutLoggerBuilder::ShowTimestamp(bool show_timestamp) noexcept
{
    show_timestamp_ = show_timestamp;
    return *this;
}

StdoutLoggerBuilder& StdoutLoggerBuilder::LogLevel(score::mw::log::rust::LogLevel log_level) noexcept
{
    log_level_ = log_level;
    return *this;
}

void StdoutLoggerBuilder::SetAsDefaultLogger() noexcept
{
    const char* context_ptr{nullptr};
    size_t context_size{0};
    if (context_)
    {
        auto value{context_.value()};
        context_ptr = value.c_str();
        context_size = value.size();
    }

    const bool* show_module{show_module_ ? &show_module_.value() : nullptr};
    const bool* show_file{show_file_ ? &show_file_.value() : nullptr};
    const bool* show_line{show_line_ ? &show_line_.value() : nullptr};
    const bool* show_timestamp{show_timestamp_ ? &show_timestamp_.value() : nullptr};
    const score::mw::log::rust::LogLevel* log_level{log_level_ ? &log_level_.value() : nullptr};

    set_default_logger(context_ptr, context_size, show_module, show_file, show_line, show_timestamp, log_level);
}

}  // namespace score::mw::log::rust
