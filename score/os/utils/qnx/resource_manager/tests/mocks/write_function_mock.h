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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_TEST_WRITE_HANDLER_INTERFACE_MOCK_H
#define SCORE_LIB_OS_QNX_RESOURCE_TEST_WRITE_HANDLER_INTERFACE_MOCK_H

#include "gtest/gtest.h"
#include "score/os/qnx/types/types.h"
#include <gmock/gmock.h>

namespace score
{
namespace os
{
namespace test
{
///
/// @brief Mock class for the write handler interface used only for testing
///
class WriteFunctionMock final : public IWriteFunction
{
  public:
    MOCK_METHOD(int, CallOperator, (resmgr_context_t * ctp, io_write_t* msg, RESMGR_OCB_T* ocb));
    virtual int operator()(resmgr_context_t* ctp, io_write_t* msg, RESMGR_OCB_T* ocb) override
    {
        return CallOperator(ctp, msg, ocb);
    }
};

}  // namespace test
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESOURCE_TEST_WRITE_HANDLER_INTERFACE_MOCK_H
