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
#include "score/os/mocklib/qnx/mock_slog2.h"

#include <cstdarg>
#include <cstdio>
#include <string>

namespace score
{
namespace os
{
namespace qnx
{

score::cpp::expected<std::int32_t, score::os::Error> MockSlog2::slog2f(slog2_buffer_t buffer,
                                                                       std::uint16_t code,
                                                                       std::uint8_t severity,
                                                                       const char* format,
                                                                       ...) const noexcept
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
    message.resize(message_length);

    // Write formatted string to buffer
    const auto length_including_terminator = message_length + 1;
    std::vsnprintf(&message[0], length_including_terminator, format, args);

    // Clean up the va_list
    va_end(args);

    return MockedSlog2f(buffer, code, severity, message);
}

}  // namespace qnx
}  // namespace os
}  // namespace score
