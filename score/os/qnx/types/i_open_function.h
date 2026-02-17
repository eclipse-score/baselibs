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

#ifndef SCORE_LIB_OS_QNX_RESOURCE_OPEN_HANDLER_INTERFACE_H
#define SCORE_LIB_OS_QNX_RESOURCE_OPEN_HANDLER_INTERFACE_H

// clang-format off
// the order is important to correctly define RESMGR_HANDLE_T, see QNX resmgr_attach() documentation
#include "score/os/qnx/types/iofunc_types.h"
// clang-format on

namespace score
{
namespace os
{

/// @brief Abstraction class to provide configurability for open() handler.
///
/// This interface implemented with QNX signature. shall be overriden  by the application that is
/// running the resource manager with the right functionality
class IOpenFunction
{
  public:
    virtual ~IOpenFunction() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    IOpenFunction(const IOpenFunction&) = delete;
    IOpenFunction& operator=(const IOpenFunction&) = delete;
    IOpenFunction(IOpenFunction&& other) = delete;
    IOpenFunction& operator=(IOpenFunction&& other) = delete;
    virtual int operator()(resmgr_context_t* ctp, io_open_t* msg, RESMGR_HANDLE_T* handle, void* extra) = 0;

  protected:
    IOpenFunction() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESOURCE_OPEN_HANDLER_INTERFACE_H
