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
#include "score/os/semaphore_impl.h"

namespace score
{
namespace os
{

// QNX variant of SemaphoreImpl::sem_timedwait_monotonic(); selected via BUILD select(). QNX provides a
// dedicated monotonic variant of sem_timedwait(): the timeout is measured against CLOCK_MONOTONIC and is
// therefore immune to wall-clock changes such as NTP steps.
score::cpp::expected_blank<Error> SemaphoreImpl::sem_timedwait_monotonic(
    sem_t* const sem,
    const struct timespec* const abs_time) const noexcept
{
    if (::sem_timedwait_monotonic(sem, abs_time) != 0)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

}  // namespace os
}  // namespace score
