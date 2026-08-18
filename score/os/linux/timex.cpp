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
#include "score/os/linux/timex.h"
#include <sys/timex.h>

/* KW_SUPPRESS_START:MISRA.VAR.NEEDS.CONST:Wrapped function doesn't use const struct ptr as param */
std::int32_t score::os::clock_adjtime(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace
                                         usage */
                                      const clockid_t clkid,
                                      struct timex* tx)
{
    return ::clock_adjtime(clkid, tx);
} /* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST:Wrapped function doesn't use const struct ptr as param */
