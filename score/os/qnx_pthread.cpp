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
#include "score/os/qnx_pthread.h"
#include <limits>

std::unique_ptr<score::os::Pthread> score::os::Pthread::Default() noexcept
{
    return std::make_unique<score::os::QnxPthread>();
}

/* KW_SUPPRESS_START:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST: */
/* score::cpp::pmr::make_unique takes non-const memory_resource */
score::cpp::pmr::unique_ptr<score::os::Pthread> score::os::Pthread::Default(
    score::cpp::pmr::memory_resource* memory_resource) noexcept
/* KW_SUPPRESS_END:MISRA.PPARAM.NEEDS.CONST, MISRA.VAR.NEEDS.CONST */
{
    return score::cpp::pmr::make_unique<score::os::QnxPthread>(memory_resource);
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */

score::cpp::expected_blank<score::os::Error> score::os::QnxPthread::setname_np(const pthread_t thread,
                                                                               const char* const name) const noexcept
{
    // Manual code analysis:
    // Implementation in QNX differs from documentation. Function always returns without error when thread exists. Even
    // when a too long name is provided. Name is cut off. As documented in POSIX 1003.1 and QNX documentation, function
    // returns errors via return value encoded as errno numbers.
    const std::int32_t ret = ::pthread_setname_np(thread, name);
    if (ret != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));  // LCOV_EXCL_LINE
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::QnxPthread::getname_np(const pthread_t thread,
                                                                               char* const name,
                                                                               const std::size_t length) const noexcept
{
    if (length > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()))  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createUnspecifiedError());  // LCOV_EXCL_LINE
        // Passing length greater than std::numeric_limits<std::int32_t>::max(), results in integer overflow.
    }

    // Manual code analysis:
    // ::pthread_getname_np() on QNX is documented as returning errors via its return value. The return value is
    // uses error codes to report issues. In contrast to the documentation, QNX does not trigger errors for wrong
    // arguments but silently fails to provide the name of the thread. Hence, error checking is impossible in unit
    // tests.
    const std::int32_t ret = ::pthread_getname_np(thread, name, static_cast<std::int32_t>(length));
    if (ret != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));  // LCOV_EXCL_LINE
    }
    return {};
}

score::cpp::expected_blank<score::os::Error> score::os::QnxPthread::getcpuclockid(const pthread_t id,
                                                                                  clockid_t* clock_id) const noexcept
{
    // Manual code analysis:
    //  Negative Test:- ::pthread_getcpuclockid() on QNX is documented as returning ESRCH if the value specified by id
    //  doesn't refer to an existing thread. Passing non-existing thread id is not possible through unit tests.
    const std::int32_t ret = ::pthread_getcpuclockid(id, clock_id);
    if (ret != 0)  // LCOV_EXCL_BR_LINE
    {
        return score::cpp::make_unexpected(Error::createFromErrno(ret));  // LCOV_EXCL_LINE
    }
    return {};
}

/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Use char to keep function signature. */
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Shaddowing function name is intended. */
