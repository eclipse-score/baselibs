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
#include "score/os/linux/unistd.h"

std::int32_t
score::os::daemon(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
                  const std::int32_t nochdir,
                  const std::int32_t noclose)
{
    return ::daemon(nochdir, noclose);
}

/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
std::int32_t
score::os::pipe2(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
                 std::int32_t pipedes[2],
                 const std::int32_t flags)
{
    return ::pipe2(pipedes, flags); /* KW_SUPPRESS:MISRA.FUNC.ARRAY.PARAMS: can't be changed to &pipedes[0], */
    /* underlying function may receive NULL and return particular error */
} /* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE:Wrapped function's signature requires C-style array */
