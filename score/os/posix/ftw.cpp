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
#include "score/os/posix/ftw.h"

#include <ftw.h>

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

score::cpp::expected<std::int32_t, score::os::Error> FtwPosix::ftw(const char* const path,
                                                                   std::int32_t (*const fn)(const char* fname,
                                                                                            const struct stat* sbuf,
                                                                                            std::int32_t flag),
                                                                   const std::int32_t ndirs) const noexcept
{
    const std::int32_t res = ::ftw(path, fn, ndirs);
    if (res == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    else
    {
        // return whatever value was returned by fn()
        return res;
    }
}

}  // namespace os
}  // namespace score

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
