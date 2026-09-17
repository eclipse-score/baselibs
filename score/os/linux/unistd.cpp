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

std::int32_t score::os::daemon(const std::int32_t nochdir, const std::int32_t noclose)
{
    return ::daemon(nochdir, noclose);
}

std::int32_t score::os::pipe2(std::int32_t pipedes[2], const std::int32_t flags)
{
    return ::pipe2(pipedes, flags);
    /* underlying function may receive NULL and return particular error */
}
