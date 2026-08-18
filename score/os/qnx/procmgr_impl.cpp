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
#include "score/os/qnx/procmgr_impl.h"

#include <limits>

namespace score
{
namespace os
{

// Suppress "AUTOSAR C++14 A0-1-3", The rule states: "Every function defined in an anonymous namespace, or static
// function with internal linkage, or private member function shall be used."  Justification: ProcMgrImpl is
// instantiated as static via global function, hence procmgr_ability will be used through it.
// coverity[autosar_cpp14_a0_1_3_violation]
score::cpp::expected_blank<score::os::Error> ProcMgrImpl::procmgr_ability(const pid_t pid,
                                                                          const std::uint32_t ability) const noexcept
{
    if ((ability & PROCMGR_AOP_SUBRANGE) != 0U)
    {
        // asked for subrange, but no subrange parameters available
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }
    std::int32_t result{};
    if ((ability & PROCMGR_AID_MASK) == PROCMGR_AID_EOL)
    {
        // "catch-all" case
        // Suppressed here because POSIX method accepts c-style vararg
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
        result = ::procmgr_ability(pid, ability);
    }
    else
    {
        // single ability case
        // Suppressed here because POSIX method accepts c-style vararg
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
        result = ::procmgr_ability(pid, ability, PROCMGR_AID_EOL);
    }
    if (result != EOK)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return {};
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected_blank<score::os::Error> ProcMgrImpl::procmgr_ability(const pid_t pid,
                                                                          const std::uint32_t ability,
                                                                          const std::uint32_t subrange_ability,
                                                                          const std::uint64_t subrange_min,
                                                                          const std::uint64_t subrange_max,
                                                                          const std::uint32_t eol) const noexcept
{
    // Suppressed here because POSIX method accepts c-style vararg
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) see comment above
    std::int32_t result = ::procmgr_ability(pid, ability, subrange_ability, subrange_min, subrange_max, eol);
    if (result != EOK)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return {};
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected_blank<score::os::Error> ProcMgrImpl::procmgr_daemon(const std::int32_t status,
                                                                         const std::uint32_t flags) const noexcept
{
    std::int32_t result = ::procmgr_daemon(status, flags);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return {};
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected<std::int32_t, score::os::Error> ProcMgrImpl::procmgr_event_notify_add(
    const std::uint32_t flags,
    const struct sigevent* event) const noexcept
{
    if (event == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }
    std::int32_t result = ::procmgr_event_notify_add(flags, event);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return result;
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected_blank<score::os::Error> ProcMgrImpl::procmgr_event_notify_delete(
    const std::int32_t handle) const noexcept
{
    std::int32_t result = ::procmgr_event_notify_delete(handle);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return {};
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected<std::int32_t, score::os::Error> ProcMgrImpl::procmgr_value_notify_add(
    const std::uint32_t type,
    const std::int32_t sub_id,
    const std::uint64_t value,
    const struct sigevent* event) const noexcept
{
    if (event == nullptr)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(EINVAL));
    }
    std::int32_t result = ::procmgr_value_notify_add(type, sub_id, value, event);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return result;
}

// coverity[autosar_cpp14_a0_1_3_violation] , see justification above
score::cpp::expected<std::uint64_t, score::os::Error> ProcMgrImpl::procmgr_value_current(
    const std::int32_t id) const noexcept
{
    std::uint64_t result = ::procmgr_value_current(id);
    if (result == std::numeric_limits<std::uint64_t>::max())
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));
    }
    return result;
}

}  // namespace os
}  // namespace score
