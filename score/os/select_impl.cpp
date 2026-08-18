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
#include "score/os/select_impl.h"

namespace score
{
namespace os
{
/* KW_SUPPRESS_START:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> SelectImpl::select(const std::int32_t nfds,
                                                                        fd_set* const readfds,
                                                                        fd_set* const writefds,
                                                                        fd_set* const exceptfds,
                                                                        struct timeval* const timeout) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
/* KW_SUPPRESS_END:MISRA.PTR.TO_PTR_TO_PTR:Used parameters match the param requirements of wrapped function */
{
    // This is a wrapper over C banned function, thus the suppression is justified.
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t output = ::select(nfds, readfds, writefds, exceptfds, timeout);
    if (output == -1) /* LCOV_EXCL_BR_LINE: Not possible to make ::select return -1 value through unit test */
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
        /* Unable to cover through unit test for failure test case, CI fails on memcheck report */
    }
    return output;
}
}  // namespace os
}  // namespace score
