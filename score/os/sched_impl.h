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
#ifndef SCORE_LIB_OS_SCHED_IMPL_H
#define SCORE_LIB_OS_SCHED_IMPL_H

#include "score/os/sched.h"

#include "score/expected.hpp"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
namespace score
{
namespace os
{

// Suppress "AUTOSAR C++14 M3-2-3" rule finding: "A type, object or function that is used in multiple translation units
// shall be declared in one and only one file.".
// This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class SchedImpl final : public Sched
{
  public:
    constexpr SchedImpl() = default;
    score::cpp::expected<std::int32_t, score::os::Error> sched_getparam(
        const pid_t pid,
        struct sched_param* const parms) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_getscheduler(const pid_t pid) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_setparam(
        const pid_t pid,
        const struct sched_param* const parms) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_setscheduler(
        const pid_t pid,
        const std::int32_t policy,
        const struct sched_param* const parms) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_yield(void) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_rr_get_interval(
        const pid_t pid,
        struct timespec* const t) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_get_priority_min(
        const std::int32_t alg) const noexcept override;
    score::cpp::expected<std::int32_t, score::os::Error> sched_get_priority_max(
        const std::int32_t alg) const noexcept override;
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different implementations for linux and QNX to exist
// in the same file. It also prevents compiler errors in linux code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined(__QNX__)
    score::cpp::expected<std::int32_t, score::os::Error>
    sched_get_priority_adjust(std::int32_t prio, std::int32_t alg, std::int32_t adjust) const noexcept override;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif  //__QNX__
};
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN: Wrapper function is identifiable through namespace usage */
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SCHED_IMPL_H
