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
#ifndef SCORE_LIB_OS_TIME_H
#define SCORE_LIB_OS_TIME_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <cstdint>
#include <ctime>

namespace score
{
namespace os
{

class Time : public ObjectSeam<Time>
{
  public:
    /// \brief thread-safe singleton accessor
    /// \return Either concrete OS-dependent instance or respective set mock instance
    static Time& instance() noexcept;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_settime(const clockid_t clkid,
                                                             const struct timespec* const tp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_gettime(const clockid_t clkid,
                                                             struct timespec* const tp) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<std::int32_t, Error> clock_getres(const clockid_t clkid,
                                                            struct timespec* const res) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual struct tm* localtime_r(const time_t* timer, struct tm* tm_local_time) const noexcept = 0;

    /// @brief Creates a timer
    /// @see https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/t/timer_create.html
    virtual score::cpp::expected<std::int32_t, Error> timer_create(const clockid_t clock_id,
                                                            sigevent* const evp,
                                                            timer_t* const timerid) const noexcept = 0;

    /// @brief Deletes a timer
    /// @see https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/t/timer_delete.html
    virtual score::cpp::expected<std::int32_t, Error> timer_delete(const timer_t timerid) const noexcept = 0;

    /// @brief Arms a timer with the given expiration time, disarm it, or set its tolerance
    /// @see https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/t/timer_settime.html
    virtual score::cpp::expected<std::int32_t, Error> timer_settime(const timer_t timerid,
                                                             const int32_t flags,
                                                             const itimerspec* const value,
                                                             itimerspec* const ovalue) const noexcept = 0;

    /// @brief Obtains ID of a process CPU-time clock
    /// @see https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/c/clock_getcpuclockid.html
    virtual score::cpp::expected<std::int32_t, Error> clock_getcpuclockid(const pid_t pid,
                                                                   clockid_t& clock_id) const noexcept = 0;

    virtual ~Time() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&& other) = delete;
    Time& operator=(Time&& other) = delete;

  protected:
    Time() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_TIME_H
