/********************************************************************************
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
********************************************************************************/
#include "score/os/mocklib/qnx/mock_slog.h"
 
#include <cstdarg>
#include <cstdio>
#include <string>
 
namespace score
{
namespace os
{
namespace qnx
{
 
score::cpp::expected<std::int32_t, score::os::Error> MockSlog::slogf(const SlogCode code,
                                                                     const SlogSeverity severity,
                                                                     const char* const format,
                                                                     ...) const noexcept
{
    va_list args;
    va_start(args, format);
 
    // Measure the formatted length, then rewind the va_list to format for real.
    const auto message_length = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);
 
    // A negative return means an encoding error.
    if (message_length < 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
 
    va_start(args, format);
 
    std::string message{};
    message.resize(static_cast<std::string::size_type>(message_length));
 
    const auto length_including_terminator = message_length + 1;
    std::vsnprintf(&message[0], length_including_terminator, format, args);
 
    va_end(args);
 
    return MockedSlogf(code, severity, message);
}
 
}  // namespace qnx
}  // namespace os
}  // namespace score

 