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
#include "score/os/syslog_impl.h"

#include <cstdarg>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
void SyslogImpl::openlog(const char* const ident, const std::int32_t option, const std::int32_t facility) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    ::openlog(ident, option, facility);
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:MISRA.FUNC.VARARG:Required for wrapper method */
// coverity[autosar_cpp14_a8_4_1_violation]: see above
void SyslogImpl::syslog(const std::int32_t priority, const char* const format, ...) const noexcept
/* KW_SUPPRESS_END:MISRA.FUNC.VARARG:Required for wrapper method */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    va_list args;
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_start(args, format);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(*array-decay, *pro-type-vararg,*array-to-pointer-decay) see comment above
    ::vsyslog(priority, format, args);
    /* KW_SUPPRESS_START:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
    // Suppressed here because POSIX method accepts va_list
    // NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-type-vararg) see comment above
    va_end(args);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay) see comment above
    /* KW_SUPPRESS_END:MISRA.USE.EXPANSION:Using library-defined macro to ensure correct operation */
}

void SyslogImpl::closelog() const noexcept
{
    ::closelog();
}

}  // namespace os
}  // namespace score
