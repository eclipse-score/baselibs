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
#include "score/os/mocklib/mock_syslog.h"

#include <cstdarg>
#include <cstdio>
#include <string>

namespace score
{
namespace os
{

void MockSyslog::syslog(std::int32_t priority, const char* format, ...) const noexcept
{
    // Create a va_list to hold the variable arguments
    va_list args;
    va_start(args, format);

    // Determine required buffer size
    const auto message_length = std::vsnprintf(nullptr, 0, format, args);

    // Reset the va_list to be able to use it again
    va_end(args);
    va_start(args, format);

    // Create buffer
    std::string message{};
    message.resize(static_cast<std::size_t>(message_length));

    // Write formatted string to buffer
    const auto length_including_terminator = static_cast<std::size_t>(message_length) + 1U;
    std::vsnprintf(&message[0], length_including_terminator, format, args);

    // Clean up the va_list
    va_end(args);

    MockedSyslog(priority, message);
}

}  // namespace os
}  // namespace score
