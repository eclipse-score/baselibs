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
#ifndef SCORE_LIB_OS_MOCKLIB_TIME_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_TIME_MOCK_H

#include "score/os/time.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class TimeMock : public Time
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                clock_settime,
                (const clockid_t, const struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                clock_gettime,
                (const clockid_t, struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                clock_getres,
                (const clockid_t, struct timespec*),
                (const, noexcept, override));
    MOCK_METHOD(struct tm*, localtime_r, (const time_t*, struct tm*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                timer_create,
                (const clockid_t, sigevent*, timer_t*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), timer_delete, (const timer_t), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                timer_settime,
                (const timer_t, const int32_t, const itimerspec*, itimerspec*),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                clock_getcpuclockid,
                (const pid_t, clockid_t&),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_TIME_MOCK_H
