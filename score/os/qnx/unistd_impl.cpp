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
#include "score/os/qnx/unistd_impl.h"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> score::os::qnx::QnxUnistdImpl::setgroupspid(
    const std::int32_t gidsetsize,
    const gid_t* const grouplist,
    const pid_t pid) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::setgroupspid(gidsetsize, grouplist, pid);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}
