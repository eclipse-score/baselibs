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
#ifndef SCORE_LIB_OS_QNX_RESMGR_IMPL_H
#define SCORE_LIB_OS_QNX_RESMGR_IMPL_H

#include "score/os/errno.h"
#include "score/os/qnx/resmgr.h"

#include "score/expected.hpp"

#include <sys/resmgr.h>

namespace score
{
namespace os
{

class ResMgrImpl final : public ResMgr
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<size_t, score::os::Error> resmgr_msgwrite(resmgr_context_t* ctp,
                                                                   const void* msg,
                                                                   const size_t size,
                                                                   const size_t offset) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_RESMGR_IMPL_H
