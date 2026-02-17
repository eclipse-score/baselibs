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

#include "score/os/qnx/thread_ctl_impl.h"

namespace score
{
namespace os
{
namespace qnx
{
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> ThreadCtlImpl::ThreadCtlExt(const pid_t pid,
                                                                const std::int32_t tid,
                                                                const std::int32_t cmd,
                                                                void* const data) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::ThreadCtlExt(pid, tid, cmd, data) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

}  // namespace qnx
}  // namespace os
}  // namespace score
