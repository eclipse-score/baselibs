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
#ifndef SCORE_LIB_OS_SYS_WAIT_H
#define SCORE_LIB_OS_SYS_WAIT_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/wait.h>

namespace score
{
namespace os
{

class SysWait : public ObjectSeam<SysWait>
{
  public:
    static SysWait& instance() noexcept;

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<pid_t, Error> wait(std::int32_t* const stat_loc) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    /* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
    virtual score::cpp::expected<pid_t, Error> waitpid(const pid_t pid,
                                                       std::int32_t* const stat_loc,
                                                       const std::int32_t options) const noexcept = 0;
    /* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */

    virtual ~SysWait() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    SysWait(const SysWait&) = delete;
    SysWait& operator=(const SysWait&) = delete;
    SysWait(SysWait&& other) = delete;
    SysWait& operator=(SysWait&& other) = delete;

  protected:
    SysWait() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_SYS_WAIT_H
