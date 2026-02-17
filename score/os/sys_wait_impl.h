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

#ifndef SCORE_LIB_OS_SYS_WAIT_IMPL_H
#define SCORE_LIB_OS_SYS_WAIT_IMPL_H

#include "score/os/sys_wait.h"

namespace score
{
namespace os
{

class SysWaitImpl final : public SysWait
{
  public:
    constexpr SysWaitImpl() = default;
    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<pid_t, Error> wait(std::int32_t* const stat_loc) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    score::cpp::expected<pid_t, Error> waitpid(const pid_t pid,
                                        std::int32_t* const stat_loc,
                                        const std::int32_t options) const noexcept override;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
};
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYS_WAIT_IMPL_H
