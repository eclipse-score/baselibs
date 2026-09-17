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
#include "score/os/qnx/sysmgr_impl.h"

/* It is not possible to test because the ::sysmgr_reboot() function reboots the system. */
/* LCOV_EXCL_START */
score::cpp::expected<std::int32_t, score::os::Error> score::os::qnx::SysMgrImpl::sysmgr_reboot() const noexcept
{
    const std::int32_t result = ::sysmgr_reboot();
    if (result != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}
/* LCOV_EXCL_STOP */
