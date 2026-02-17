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

#ifndef SCORE_LIB_OS_MOCKLIB_QNX_MOCK_THREAD_CTL_H
#define SCORE_LIB_OS_MOCKLIB_QNX_MOCK_THREAD_CTL_H

#include "score/os/qnx/thread_ctl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace qnx
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

class MockThreadCtl : public ThreadCtl
{
  public:
    MOCK_METHOD((score::cpp::expected_blank<score::os::Error>),
                ThreadCtlExt,
                (const pid_t pid, const std::int32_t tid, const std::int32_t cmd, void* const data),
                (const, noexcept, override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_MOCK_THREAD_CTL_H
