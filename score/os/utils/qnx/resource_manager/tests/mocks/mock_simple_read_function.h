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
#ifndef SCORE_LIB_OS_MOCKLIB_QNX_SIMPLE_READ_FUNCTION_H
#define SCORE_LIB_OS_MOCKLIB_QNX_SIMPLE_READ_FUNCTION_H

#include "score/os/utils/qnx/resource_manager/include/simple_read_function.h"

#include "score/result/result.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

class MockSimpleReadFunction final : public SimpleReadFunction
{
  public:
    MockSimpleReadFunction(const score::os::MockSimpleReadFunction& function) : SimpleReadFunction(function) {}
    MockSimpleReadFunction(ResMgr& mgr, IoFunc& iofunc) : SimpleReadFunction(mgr, iofunc) {}
    MOCK_METHOD((score::Result<size_t>), read, (const off_t offset, size_t nbytes, uint64_t& result), (override));
    MOCK_METHOD((score::Result<void>), preRead, (off_t offset, size_t size), (override));
    MOCK_METHOD((score::Result<void>), postRead, (), (override));
};

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shadowing function name is intended. */

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_QNX_SIMPLE_READ_FUNCTION_H
