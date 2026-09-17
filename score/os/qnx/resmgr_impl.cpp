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
#include "score/os/qnx/resmgr_impl.h"
/* The only non-const variables or parameters may actually be modified by the QNX */
score::cpp::expected<size_t, score::os::Error> score::os::ResMgrImpl::resmgr_msgwrite(
    resmgr_context_t* ctp,
    const void* msg,
    const size_t size,
    const size_t offset) const noexcept
{
    const ssize_t result = ::resmgr_msgwrite(ctp, msg, size, offset);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    else
    {
        return static_cast<size_t>(result);
    }
}
