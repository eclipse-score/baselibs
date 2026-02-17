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
#ifndef SCORE_LIB_OS_MOCKLIB_SCHED_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_SCHED_MOCK_H

#include "score/os/sched.h"
#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SchedMock : public Sched
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_getparam,
                (const pid_t, sched_param* const),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_getscheduler,
                (const pid_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_setparam,
                (const pid_t, const sched_param* const),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_setscheduler,
                (const pid_t, const std::int32_t, const struct sched_param* const),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>), sched_yield, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_rr_get_interval,
                (const pid_t, timespec* const),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_get_priority_min,
                (const std::int32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_get_priority_max,
                (const std::int32_t),
                (const, noexcept, override));
#if defined(__QNX__)
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                sched_get_priority_adjust,
                (const std::int32_t, const std::int32_t, const std::int32_t),
                (const, noexcept, override));
#endif  //__QNX__
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_SCHED_MOCK_H
