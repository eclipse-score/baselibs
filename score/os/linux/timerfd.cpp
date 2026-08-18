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
#include "score/os/linux/timerfd.h"

std::int32_t score::os::timerfd_create(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through
                                          namespace usage */
                                       const std::int32_t clock_id,
                                       const std::int32_t flags)
{
    return ::timerfd_create(clock_id, flags);
}

std::int32_t score::os::timerfd_gettime(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through
                                           namespace usage */
                                        const std::int32_t fd,
                                        struct itimerspec* curr_value)
{
    return ::timerfd_gettime(fd, curr_value);
}

std::int32_t score::os::timerfd_settime(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through
                                           namespace usage */
                                        const std::int32_t fd,
                                        const std::int32_t flags,
                                        const struct itimerspec* new_value,
                                        struct itimerspec* old_value)
{
    return ::timerfd_settime(fd, flags, new_value, old_value);
}
