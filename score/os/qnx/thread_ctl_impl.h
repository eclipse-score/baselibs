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

#ifndef SCORE_LIB_OS_QNX_THREAD_CTL_IMPL_H
#define SCORE_LIB_OS_QNX_THREAD_CTL_IMPL_H

#include "score/os/qnx/thread_ctl.h"

namespace score
{
namespace os
{
namespace qnx
{

class ThreadCtlImpl final : public ThreadCtl
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> ThreadCtlExt(const pid_t pid,
                                                     const std::int32_t tid,
                                                     const std::int32_t cmd,
                                                     void* const data) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_THREAD_CTL_IMPL_H
