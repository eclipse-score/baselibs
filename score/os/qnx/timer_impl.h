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

#ifndef SCORE_LIB_OS_QNX_TIMER_IMPL_H
#define SCORE_LIB_OS_QNX_TIMER_IMPL_H

#include "score/os/qnx/timer.h"

namespace score
{
namespace os
{
namespace qnx
{

class TimerImpl final : public Timer
{
  public:
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected_blank<score::os::Error> TimerSettime(const timer_t id,
                                                     const std::int32_t flags,
                                                     const struct _itimer* const itime,
                                                     struct _itimer* const oitime) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<std::int32_t, score::os::Error> TimerCreate(const clockid_t id,
                                                            const struct sigevent* const event) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    score::cpp::expected_blank<score::os::Error> TimerDestroy(const timer_t id) const noexcept override;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_TIMER_IMPL_H
