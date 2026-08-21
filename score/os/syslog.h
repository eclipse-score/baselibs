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
#ifndef SCORE_LIB_OS_SYSLOG_H
#define SCORE_LIB_OS_SYSLOG_H

#include "score/os/errno.h"

#include "score/memory.hpp"

#include <syslog.h>

namespace score
{
namespace os
{

/// @brief Object-seam wrapper around the POSIX/glibc syslog(3) family.
///
/// Mirrors the QNX @ref score::os::qnx::Slog2 wrapper so that the platform-native
/// system-logger backend (LogMode::kSystem) can be host-unit-tested through an
/// injectable seam (real SyslogImpl / gmock MockSyslog).
class Syslog
{
  public:
    static score::cpp::pmr::unique_ptr<Syslog> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual void openlog(const char* const ident, const std::int32_t option, const std::int32_t facility) const
        noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    /* KW_SUPPRESS_START:MISRA.FUNC.VARARG:Required for wrapper method */
    virtual void syslog(const std::int32_t priority, const char* const format, ...) const noexcept
        /* KW_SUPPRESS_END:MISRA.FUNC.VARARG:Required for wrapper method */
        /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

        __attribute__((format(printf, 3, 4))) = 0;

    virtual void closelog() const noexcept = 0;

    /// @brief Construct a new Syslog object
    ///
    Syslog() = default;

    virtual ~Syslog() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Syslog(const Syslog&) = delete;
    Syslog& operator=(const Syslog&) = delete;
    Syslog(Syslog&& other) = delete;
    Syslog& operator=(Syslog&& other) = delete;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYSLOG_H
