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
#ifndef SCORE_LIB_OS_QNX_SLOG_H
#define SCORE_LIB_OS_QNX_SLOG_H

#include "score/os/errno.h"

#include "score/expected.hpp"
#include "score/memory.hpp"

#include <sys/slogcodes.h>

namespace score
{
namespace os
{
namespace qnx
{

/// @brief Log codes mapping the QNX _SLOGC_* facility macros.
enum class SlogCode : std::int32_t
{
    Network = _SLOGC_NETWORK,
};

/// @brief Severity levels mapping the QNX _SLOG_* macros.
enum class SlogSeverity : std::int32_t
{
    ShutdownFatal = _SLOG_SHUTDOWN,
    Critical = _SLOG_CRITICAL,
    Error = _SLOG_ERROR,
    Warning = _SLOG_WARNING,
    Notice = _SLOG_NOTICE,
    Info = _SLOG_INFO,
    Debug1 = _SLOG_DEBUG1,
    Debug2 = _SLOG_DEBUG2,
    MaxSeverity = _SLOG_SEVMAXVAL,
};

/// @brief OSAL wrapper around the QNX legacy slogf() logger.
///
/// Wraps the legacy slog facility (<sys/slog.h>), unlike Slog2 which wraps the
/// separate slogger2 facility (<sys/slog2.h>). Exposed as an interface so it can
/// be mocked in tests.
class Slog
{
  public:
    static score::cpp::pmr::unique_ptr<Slog> Default(score::cpp::pmr::memory_resource* memory_resource) noexcept;

    /// @brief Wrapper around the QNX slogf().
    /// @param code The facility/log code (see SlogCode)
    /// @param severity The severity level (see SlogSeverity)
    /// @param format Printf-style format string
    /// @param ... Format arguments
    /// @return The number of bytes written on success, or an Error on failure.
    virtual score::cpp::expected<std::int32_t, score::os::Error> slogf(const SlogCode code,
                                                                       const SlogSeverity severity,
                                                                       const char* const format,
                                                                       ...) const noexcept
        __attribute__((format(printf, 4, 5))) = 0;

    /// @brief Construct a new Slog object
    ///
    Slog() = default;

    virtual ~Slog() = default;
    // Deleted to avoid autosar_cpp14_a12_0_1_violation
    Slog(const Slog&) = delete;
    Slog& operator=(const Slog&) = delete;
    Slog(Slog&& other) = delete;
    Slog& operator=(Slog&& other) = delete;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_SLOG_H
