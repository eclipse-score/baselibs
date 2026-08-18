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

#ifndef SCORE_LIB_OS_QNX_PROCMGR_IMPL_H
#define SCORE_LIB_OS_QNX_PROCMGR_IMPL_H

#include "score/os/qnx/procmgr.h"

namespace score
{
namespace os
{

class ProcMgrImpl final : public ProcMgr
{
  public:
    constexpr ProcMgrImpl() = default;

    score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid,
                                                                 const std::uint32_t ability) const noexcept override;

    score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid,
                                                                 const std::uint32_t ability,
                                                                 const std::uint32_t subrange_ability,
                                                                 const std::uint64_t subrange_min,
                                                                 const std::uint64_t subrange_max,
                                                                 const std::uint32_t eol) const noexcept override;

    score::cpp::expected_blank<score::os::Error> procmgr_daemon(const std::int32_t status,
                                                                const std::uint32_t flags) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> procmgr_event_notify_add(
        const std::uint32_t flags,
        const struct sigevent* event) const noexcept override;

    score::cpp::expected_blank<score::os::Error> procmgr_event_notify_delete(
        const std::int32_t handle) const noexcept override;

    score::cpp::expected<std::int32_t, score::os::Error> procmgr_value_notify_add(
        const std::uint32_t type,
        const std::int32_t sub_id,
        const std::uint64_t value,
        const struct sigevent* event) const noexcept override;

    score::cpp::expected<std::uint64_t, score::os::Error> procmgr_value_current(
        const std::int32_t id) const noexcept override;
};

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_PROCMGR_IMPL_H
