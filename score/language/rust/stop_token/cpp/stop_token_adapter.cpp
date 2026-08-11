/*******************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "score/language/rust/stop_token/cpp/stop_token_adapter.h"

namespace score::language::rust::stop_token
{

bool StopTokenStopRequested(const score::cpp::stop_token& token) noexcept
{
    return token.stop_requested();
}

bool StopTokenStopPossible(const score::cpp::stop_token& token) noexcept
{
    return token.stop_possible();
}

bool StopTokenEqual(const score::cpp::stop_token& lhs, const score::cpp::stop_token& rhs) noexcept
{
    return lhs == rhs;
}

bool StopSourceStopRequested(const score::cpp::stop_source& source) noexcept
{
    return source.stop_requested();
}

bool StopSourceStopPossible(const score::cpp::stop_source& source) noexcept
{
    return source.stop_possible();
}

bool StopSourceRequestStop(const score::cpp::stop_source& source) noexcept
{
    // request_stop() only updates the shared stop state atomically.
    return const_cast<score::cpp::stop_source&>(source).request_stop();
}

std::shared_ptr<score::cpp::stop_token> StopSourceGetToken(const score::cpp::stop_source& source) noexcept
{
    return std::make_shared<score::cpp::stop_token>(source.get_token());
}

bool StopSourceEqual(const score::cpp::stop_source& lhs, const score::cpp::stop_source& rhs) noexcept
{
    return lhs == rhs;
}

std::shared_ptr<score::cpp::stop_token> MakeDefaultStopToken() noexcept
{
    return std::make_shared<score::cpp::stop_token>();
}

std::shared_ptr<score::cpp::stop_source> MakeStopSource() noexcept
{
    return std::make_shared<score::cpp::stop_source>();
}

}  // namespace score::language::rust::stop_token
