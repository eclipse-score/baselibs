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
#include "score/mw/log/logging.h"

#include "score/mw/log/log_level.h"
#include "score/mw/log/log_stream_factory.h"
#include "score/mw/log/recorder.h"
#include "score/mw/log/runtime.h"

score::mw::log::LogStream score::mw::log::LogFatal() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal);
}

score::mw::log::LogStream score::mw::log::LogError() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError);
}

score::mw::log::LogStream score::mw::log::LogWarn() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn);
}

score::mw::log::LogStream score::mw::log::LogInfo() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo);
}

score::mw::log::LogStream score::mw::log::LogDebug() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug);
}

score::mw::log::LogStream score::mw::log::LogVerbose() noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose);
}

score::mw::log::LogStream score::mw::log::LogFatal(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kFatal, context_id);
}

score::mw::log::LogStream score::mw::log::LogError(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kError, context_id);
}

score::mw::log::LogStream score::mw::log::LogWarn(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kWarn, context_id);
}

score::mw::log::LogStream score::mw::log::LogInfo(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kInfo, context_id);
}

score::mw::log::LogStream score::mw::log::LogDebug(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kDebug, context_id);
}

score::mw::log::LogStream score::mw::log::LogVerbose(const std::string_view context_id) noexcept
{
    return score::mw::log::detail::LogStreamFactory::GetStream(LogLevel::kVerbose, context_id);
}

score::mw::log::Recorder& score::mw::log::GetDefaultLogRecorder() noexcept
{
    return score::mw::log::detail::Runtime::GetRecorder();
}

void score::mw::log::SetLogRecorder(score::mw::log::Recorder* const recorder) noexcept
{
    score::mw::log::detail::Runtime::SetRecorder(recorder);
}

score::mw::log::Recorder& score::mw::log::GetFallbackLogRecorder() noexcept
{
    return score::mw::log::detail::Runtime::GetFallbackRecorder();
}
