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
#include "score/mw/log/log_stream_factory.h"

#include "score/mw/log/detail/thread_local_guard.h"
#include "score/mw/log/runtime.h"

score::mw::log::LogStream score::mw::log::detail::LogStreamFactory::GetStream(
    const LogLevel log_level,
    const std::string_view context_id) noexcept
{
    if (not ThreadLocalGuard::IsWithingLogging())
    {
        ThreadLocalGuard guard{};
        // Unnamed object ok, since it will be moved out of this function
        // NOLINTNEXTLINE(score-no-unnamed-temporary-objects): See above
        return score::mw::log::LogStream{Runtime::GetRecorder(), Runtime::GetFallbackRecorder(), log_level, context_id};
    }
    else
    {
        // Unnamed object ok, since it will be moved out of this function
        // NOLINTNEXTLINE(score-no-unnamed-temporary-objects): See above
        return score::mw::log::LogStream{
            Runtime::GetFallbackRecorder(), Runtime::GetFallbackRecorder(), log_level, context_id};
    }
}
