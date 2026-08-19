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
#ifndef SCORE_LIB_OS_MOCKLIB_SYS_WAIT_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_SYS_WAIT_MOCK_H

#include "score/os/sys_wait.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{

class SysWaitMock : public SysWait
{
  public:
    MOCK_METHOD((score::cpp::expected<pid_t, score::os::Error>),
                wait,
                (std::int32_t* stat_loc),
                (override, const, noexcept));
    MOCK_METHOD((score::cpp::expected<pid_t, score::os::Error>),
                waitpid,
                (pid_t pid, std::int32_t* stat_loc, std::int32_t options),
                (override, const, noexcept));
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_SYS_WAIT_MOCK_H
