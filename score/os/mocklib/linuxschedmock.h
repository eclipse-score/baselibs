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
#ifndef SCORE_LIB_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H
#define SCORE_LIB_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H

#include "score/os/linux/linuxsched.h"
#include <gmock/gmock.h>

namespace score
{
namespace os
{

class LinuxSchedMock
{
  public:
    LinuxSchedMock();
    MOCK_METHOD(std::int32_t, sched_getcpu_os, ());
};

} /* namespace os */
} /* namespace score */

#endif  // SCORE_LIB_OS_LINUX_MOCKLIB_LINUX_SCHED_MOCK_H
