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
#ifndef SCORE_LIB_OS_QNX_PROCMGR_H
#define SCORE_LIB_OS_QNX_PROCMGR_H

#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"

#include "score/expected.hpp"

#include <sys/procmgr.h>

namespace score
{
namespace os
{

class ProcMgr : public ObjectSeam<ProcMgr>
{
  public:
    static ProcMgr& instance() noexcept;

    // the native interface for procmgr_ability() is MISRA.FUNC.VARARG. It is dangerous to expose directly,
    // and it is impossible to implement in a generic non-vararg way (due to the "otherwise" functionality),
    // so please add your use cases as separate ad-hoc function signatures if you cannot find a matching already
    // implemented one.

    virtual score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid, const std::uint32_t ability)
        const noexcept = 0;
    virtual score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid,
                                                                         const std::uint32_t ability,
                                                                         const std::uint32_t subrange_ability,
                                                                         const std::uint64_t subrange_min,
                                                                         const std::uint64_t subrange_max,
                                                                         const std::uint32_t eol) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> procmgr_daemon(const std::int32_t status,
                                                                        const std::uint32_t flags) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, score::os::Error> procmgr_event_notify_add(
        const std::uint32_t flags,
        const struct sigevent* event) const noexcept = 0;

    virtual score::cpp::expected_blank<score::os::Error> procmgr_event_notify_delete(
        const std::int32_t handle) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, score::os::Error> procmgr_value_notify_add(
        const std::uint32_t type,
        const std::int32_t sub_id,
        const std::uint64_t value,
        const struct sigevent* event) const noexcept = 0;

    virtual score::cpp::expected<std::uint64_t, score::os::Error> procmgr_value_current(
        const std::int32_t id) const noexcept = 0;

    virtual ~ProcMgr() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    ProcMgr(const ProcMgr&) = delete;
    ProcMgr& operator=(const ProcMgr&) = delete;
    ProcMgr(ProcMgr&& other) = delete;
    ProcMgr& operator=(ProcMgr&& other) = delete;

  protected:
    ProcMgr() = default;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_PROCMGR_H
