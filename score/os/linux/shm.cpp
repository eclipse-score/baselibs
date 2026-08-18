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
#include "score/os/linux/shm.h"

std::int32_t score::os::shmctl(/* KW_SUPPRESS:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage
                                */
                               const std::int32_t shmid,
                               const std::int32_t cmd,
                               struct shmid_ds* buf)
{
    return ::shmctl(shmid, cmd, buf);
}
