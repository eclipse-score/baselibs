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
#ifndef SCORE_LIB_OS_MOCKLIB_SEMAPHOREOMOCK_H
#define SCORE_LIB_OS_MOCKLIB_SEMAPHOREOMOCK_H

#include "score/os/semaphore.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SemaphoreMock : public Semaphore
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                sem_init,
                (sem_t*, const std::int32_t, const std::uint32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<sem_t*, Error>),
                sem_open,
                (const char*, const OpenFlag, const ModeFlag, const std::uint32_t),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<sem_t*, Error>),
                sem_open,
                (const char*, const OpenFlag),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_destroy, (sem_t*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_trywait, (sem_t*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_wait, (sem_t*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_post, (sem_t*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_close, (sem_t*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>), sem_unlink, (const char*), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                sem_waittimeout,
                (sem_t*, const std::chrono::nanoseconds, const bool),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                sem_timedwait,
                (sem_t*,
                 (const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>),
                 const bool),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                sem_timedwait_monotonic,
                (sem_t*,
                 (const std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>),
                 const bool),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected_blank<Error>),
                sem_getvalue,
                (sem_t*, std::int32_t*),
                (const, noexcept, override));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_SEMAPHOREOMOCK_H
