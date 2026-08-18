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
#include "sched_impl.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_getparam(
    const pid_t pid,
    struct sched_param* const parms) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_getparam(pid, parms);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_getscheduler(const pid_t pid) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_getscheduler(pid);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_setparam(
    const pid_t pid,
    const struct sched_param* const parms) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_setparam(pid, parms);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_setscheduler(
    const pid_t pid,
    const std::int32_t policy,
    const struct sched_param* const parms) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_setscheduler(pid, policy, parms);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_yield(void) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_yield();
    if (result != 0)  // LCOV_EXCL_BR_LINE
    {
        // Manual code analysis:
        // According to QNX documentation, the function always succeeds and returns 0. However, according
        // to Linux documentation, on error, -1 is returned, and errno is set to indicate the error.
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }

    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_rr_get_interval(
    const pid_t pid,
    struct timespec* const t) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_rr_get_interval(pid, t);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_get_priority_min(
    const std::int32_t alg) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_get_priority_min(alg);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SchedImpl::sched_get_priority_max(
    const std::int32_t alg) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::sched_get_priority_max(alg);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
score::cpp::expected<std::int32_t, score::os::Error>
SchedImpl::sched_get_priority_adjust(std::int32_t prio, std::int32_t alg, std::int32_t adjust) const noexcept
{
    // Manual code analysis:
    // Failure only happens when the value of the alg parameter doesn't represent a defined scheduling policy. Function
    // always return value greater than 0, even for invalid policies like -1, 1000 etc. As per QNX documentation
    // in case of an error ::sched_get_priority_adjust return a value less than 0, which is the negative of the errno
    // value.
    const std::int32_t result = ::sched_get_priority_adjust(prio, alg, adjust);
    if (result < 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());  // LCOV_EXCL_LINE
    }
    return result;
}
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  //__QNX__

}  // namespace os
}  // namespace score
