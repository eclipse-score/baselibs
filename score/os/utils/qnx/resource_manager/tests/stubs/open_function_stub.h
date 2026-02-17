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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_TEST_OPEN_HANDLER_INTERFACE_STUB_H
#define SCORE_LIB_OS_QNX_RESOURCE_TEST_OPEN_HANDLER_INTERFACE_STUB_H

#include "score/os/utils/qnx/resource_manager/include/resource.h"

namespace score
{
namespace os
{
namespace test
{

///
/// @brief Stub class for the open handler interface used only for testing
///
class OpenFunctionStub final : public score::os::IOpenFunction
{
    int operator()(resmgr_context_t*, io_open_t*, RESMGR_HANDLE_T*, void*)
    {
        return 0;
    }
};

}  // namespace test
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESOURCE_TEST_OPEN_HANDLER_INTERFACE_STUB_H
