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
#ifndef SCORE_LIB_OS_MOCKLIB_MOCK_PTHREAD_H
#define SCORE_LIB_OS_MOCKLIB_MOCK_PTHREAD_H

#include "score/os/pthread.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pthread.h>
#include <cstddef>
#include <cstdint>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

class MockPthread : public Pthread
{
  public:
    MOCK_METHOD(score::cpp::expected_blank<Error>,
                getcpuclockid,
                (const pthread_t id, clockid_t* clock_id),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                setname_np,
                (const pthread_t thread, const char* const name),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                getname_np,
                (const pthread_t thread, char* const name, const std::size_t length),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                condattr_init,
                (pthread_condattr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                condattr_setpshared,
                (pthread_condattr_t * attr, int pshared),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                condattr_destroy,
                (pthread_condattr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                cond_init,
                (pthread_cond_t * cond, const pthread_condattr_t* attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>, cond_destroy, (pthread_cond_t * cond), (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                mutexattr_init,
                (pthread_mutexattr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                mutexattr_setpshared,
                (pthread_mutexattr_t * attr, int pshared),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                mutexattr_destroy,
                (pthread_mutexattr_t * attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                mutex_init,
                (pthread_mutex_t * mutex, const pthread_mutexattr_t* attr),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<Error>,
                mutex_destroy,
                (pthread_mutex_t * mutex),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                pthread_setschedparam,
                (pthread_t thread, int policy, const struct sched_param* param),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                pthread_getschedparam,
                (pthread_t thread, int* policy, struct sched_param* param),
                (const, noexcept, override));

    MOCK_METHOD(pthread_t, self, (), (override));
};

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_MOCK_PTHREAD_H
