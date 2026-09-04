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
#include "score/os/syslog_impl.h"

#include <cstdarg>

namespace score
{
namespace os
{

void SyslogImpl::openlog(const char* const ident, const std::int32_t option, const std::int32_t facility) const noexcept
{
    ::openlog(ident, option, facility);
}

// coverity[autosar_cpp14_a8_4_1_violation]: see above
void SyslogImpl::syslog(const std::int32_t priority, const char* const format, ...) const noexcept
{
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    va_list args;
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_start(args, format);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(*array-decay, *pro-type-vararg,*array-to-pointer-decay) see comment above
    ::vsyslog(priority, format, args);
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_end(args);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
}

void SyslogImpl::closelog() const noexcept
{
    ::closelog();
}

}  // namespace os
}  // namespace score
