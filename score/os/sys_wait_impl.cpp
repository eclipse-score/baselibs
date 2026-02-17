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

#include "score/os/sys_wait_impl.h"

namespace score
{
namespace os
{

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<pid_t, Error> SysWaitImpl::wait(std::int32_t* const stat_loc) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const pid_t result = ::wait(stat_loc);
    if (result == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<pid_t, Error> SysWaitImpl::waitpid(const pid_t pid,
                                                 std::int32_t* const stat_loc,
                                                 const std::int32_t options) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const pid_t result = ::waitpid(pid, stat_loc, options);
    if (result == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return result;
}

}  // namespace os
}  // namespace score
