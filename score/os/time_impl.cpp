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
#include "score/os/time_impl.h"
#include "score/os/time.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> TimeImpl::clock_settime(const clockid_t clkid,
                                                           const struct timespec* const tp) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t ret{::clock_settime(clkid, tp)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> TimeImpl::clock_gettime(const clockid_t clkid,
                                                           struct timespec* const tp) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{

    const std::int32_t ret{::clock_gettime(clkid, tp)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, Error> TimeImpl::clock_getres(const clockid_t clkid,
                                                          struct timespec* const res) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{

    const std::int32_t ret{::clock_getres(clkid, res)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
struct tm* TimeImpl::localtime_r(const time_t* timer, struct tm* tm_local_time) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    struct tm* const local_time{::localtime_r(timer, tm_local_time)};
    return local_time;
}

score::cpp::expected<std::int32_t, Error> TimeImpl::timer_create(const clockid_t clock_id,
                                                          sigevent* const evp,
                                                          timer_t* const timerid) const noexcept
{
    const std::int32_t ret{::timer_create(clock_id, evp, timerid)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> TimeImpl::timer_delete(const timer_t timerid) const noexcept
{
    const std::int32_t ret{::timer_delete(timerid)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> TimeImpl::timer_settime(const timer_t timerid,
                                                           const int32_t flags,
                                                           const itimerspec* const value,
                                                           itimerspec* const ovalue) const noexcept
{
    const std::int32_t ret{::timer_settime(timerid, flags, value, ovalue)};
    if (ret < 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> TimeImpl::clock_getcpuclockid(const pid_t pid, clockid_t& clock_id) const noexcept
{
    const std::int32_t ret{::clock_getcpuclockid(pid, &clock_id)};
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

}  // namespace os
}  // namespace score
