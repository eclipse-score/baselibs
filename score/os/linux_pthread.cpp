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
#include "score/os/linux_pthread.h"

std::unique_ptr<score::os::Pthread> score::os::Pthread::Default() noexcept
{
    return std::make_unique<score::os::LinuxPthread>();
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::Pthread> score::os::Pthread::Default(
    score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<score::os::LinuxPthread>(memory_resource);
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

score::cpp::expected_blank<score::os::Error> score::os::LinuxPthread::setname_np(const pthread_t thread,
                                                                                 const char* const name) const noexcept
{
    const std::int32_t ret = ::pthread_setname_np(thread, name);
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));
    }
    return {};
}

// LCOV_EXCL_START: Linux specific code, scope of codecoverage is only for qnx code
score::cpp::expected_blank<score::os::Error>
score::os::LinuxPthread::getname_np(const pthread_t thread, char* const name, const std::size_t length) const noexcept
{
    const std::int32_t ret = ::pthread_getname_np(thread, name, length);
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::LinuxPthread::getcpuclockid(const pthread_t id,
                                                                                    clockid_t* clock_id) const noexcept
{
    const std::int32_t ret = ::pthread_getcpuclockid(id, clock_id);
    if (ret != 0)
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));
    }
    return {};
}
// LCOV_EXCL_STOP

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
