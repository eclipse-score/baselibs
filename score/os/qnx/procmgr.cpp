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
#include "score/os/qnx/procmgr.h"

/* KW_SUPPRESS_START:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */

namespace score
{
namespace os
{

namespace
{

class ProcMgrImpl final : public ProcMgr
{
  public:
    constexpr ProcMgrImpl() = default;

    // Suppress "AUTOSAR C++14 A0-1-3", The rule states: "Every function defined in an anonymous namespace, or static
    // function with internal linkage, or private member function shall be used."  Justification: ProcMgrImpl is
    // instantiated as static via global function, hence procmgr_ability will be used through it.
    // coverity[autosar_cpp14_a0_1_3_violation]
    score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid,
                                                        const std::uint32_t ability) const noexcept override
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
    score::cpp::expected_blank<score::os::Error> procmgr_ability(const pid_t pid,
                                                        const std::uint32_t ability,
                                                        const std::uint32_t subrange_ability,
                                                        const std::uint64_t subrange_min,
                                                        const std::uint64_t subrange_max,
                                                        const std::uint32_t eol) const noexcept override
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
    score::cpp::expected_blank<score::os::Error> procmgr_daemon(const std::int32_t status,
                                                       const std::uint32_t flags) const noexcept override
    {
        std::int32_t result = ::procmgr_daemon(status, flags);
        // Negative case: It appears to be impossible to test it using a unit test.
        // Error cases are not provided in qnx documentation.
        // And passing an invalid flag to procmgr_daemon does not result in error.
        if (result == -1)  // LCOV_EXCL_BR_LINE: rationale above
        {
            return score::cpp::make_unexpected(score::os::Error::createFromErrno(result));  // LCOV_EXCL_LINE
        }
        return {};
    }
};

}  // namespace
}  // namespace os
}  // namespace score

score::os::ProcMgr& score::os::ProcMgr::instance() noexcept
{
    static score::os::ProcMgrImpl instance;
    return select_instance(instance);
}

/* KW_SUPPRESS_END:MISRA.VAR.HIDDEN:Wrapper function is identifiable through namespace usage */
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC:Char is used in respect to the wrapped function's signature */
