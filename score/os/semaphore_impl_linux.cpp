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
#include "score/os/semaphore_impl.h"

#include <cerrno>
#include <time.h>

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

namespace score
{
namespace os
{

// Linux variant of SemaphoreImpl::sem_timedwait_monotonic(); selected via BUILD select(). glibc >= 2.30
// provides sem_clockwait(), which lets us pin the timeout to CLOCK_MONOTONIC (immune to wall-clock changes
// such as NTP steps). g++/clang++ predefine _GNU_SOURCE for C++, so the __USE_GNU-guarded declaration is
// visible.
/* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
score::cpp::expected_blank<Error> SemaphoreImpl::sem_timedwait_monotonic(
    sem_t* const sem,
    const struct timespec* const abs_time) const noexcept
/* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override  */
{
    // A16-0-1: version guard avoids a compile error on glibc < 2.30 (no sem_clockwait()).
    // coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__GLIBC__) && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 30)))
    if (::sem_clockwait(sem, CLOCK_MONOTONIC, abs_time) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else
    // glibc < 2.30: no monotonic wait primitive. Warn at build time, return ENOSYS at runtime.
    // coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#warning "sem_timedwait_monotonic(): requires glibc >= 2.30; returning ENOSYS"
    static_cast<void>(sem);
    static_cast<void>(abs_time);
    return score::cpp::make_unexpected(score::os::Error::createFromErrno(ENOSYS));
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

}  // namespace os
}  // namespace score
