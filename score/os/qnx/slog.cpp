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
#include "score/os/qnx/slog_impl.h"

#include <sys/slog.h>
#include <cstdarg>
#include <cstdio>

namespace score
{
namespace os
{
namespace qnx
{

score::cpp::pmr::unique_ptr<Slog> Slog::Default(score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    return score::cpp::pmr::make_unique<SlogImpl>(memory_resource);
}

// coverity[autosar_cpp14_a8_4_1_violation]: Required for wrapper method
score::cpp::expected<std::int32_t, score::os::Error> SlogImpl::slogf(const SlogCode code,
                                                                     const SlogSeverity severity,
                                                                     const char* const format,
                                                                     ...) const noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) POSIX method accepts va_list
    va_list args;
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) required to forward variadic args
    va_start(args, format);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) required for va_start
    // QNX has no vslogf(), so format into a stack buffer first. 4 KiB is a generous fixed bound;
    // longer messages get truncated.
    constexpr std::size_t kMaxLogMessageSize{4096U};
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays) fixed-size stack buffer for embedded-friendly logging
    char buffer[kMaxLogMessageSize];
    // NOLINTNEXTLINE(*array-decay, *pro-type-vararg,*array-to-pointer-decay) see comment above
    const std::int32_t formatted = std::vsnprintf(buffer, sizeof(buffer), format, args);
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) required to forward variadic args
    va_end(args);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) required for va_end
    // A negative return means an encoding error; the buffer is indeterminate, so bail out.
    if (formatted < 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) wrapping the variadic slogf
    const std::int32_t result =
        ::slogf(static_cast<std::int32_t>(code), static_cast<std::int32_t>(severity), "%s", buffer);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

}  // namespace qnx
}  // namespace os
}  // namespace score
