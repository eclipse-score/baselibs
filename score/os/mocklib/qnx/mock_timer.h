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

#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_TIMER_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_TIMER_H

#include "score/os/qnx/timer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

class MockTimer : public Timer
{
  public:
    MOCK_METHOD((score::cpp::expected<std::int32_t, score::os::Error>),
                TimerCreate,
                (const clockid_t id, const struct sigevent* event),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>,
                TimerSettime,
                (const timer_t id, const std::int32_t flags, const struct _itimer* itime, struct _itimer* oitime),
                (const, noexcept, override));

    MOCK_METHOD(score::cpp::expected_blank<score::os::Error>, TimerDestroy, (const timer_t id), (const, noexcept, override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_TIMER_H
