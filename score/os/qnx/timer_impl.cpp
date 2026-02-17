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

#include "score/os/qnx/timer_impl.h"

namespace score
{
namespace os
{
namespace qnx
{
/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected_blank<score::os::Error> TimerImpl::TimerSettime(const timer_t id,
                                                            const std::int32_t flags,
                                                            const struct _itimer* const itime,
                                                            struct _itimer* const oitime) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    if (::TimerSettime(id, flags, itime, oitime) == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
score::cpp::expected<std::int32_t, score::os::Error> TimerImpl::TimerCreate(const clockid_t id,
                                                                   const struct sigevent* const event) const noexcept
/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
{
    const std::int32_t result = ::TimerCreate(id, event);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

score::cpp::expected_blank<score::os::Error> TimerImpl::TimerDestroy(const timer_t id) const noexcept
{
    const std::int32_t result = ::TimerDestroy(id);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return {};
}

}  // namespace qnx
}  // namespace os
}  // namespace score
