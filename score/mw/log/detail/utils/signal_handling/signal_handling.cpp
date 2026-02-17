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

#include "score/mw/log/detail/utils/signal_handling/signal_handling.h"

namespace score::mw::log::detail
{

score::cpp::expected<std::int32_t, score::os::Error> SignalHandling::PThreadBlockSigTerm(score::os::Signal& signal) noexcept
{
    sigset_t sig_set;

    score::cpp::expected<std::int32_t, score::os::Error> return_error_result{};
    auto result = signal.SigEmptySet(sig_set);
    if (result.has_value())
    {
        // signal handling is tolerated by design. Argumentation: Ticket-101432
        // coverity[autosar_cpp14_m18_7_1_violation]
        result = signal.SigAddSet(sig_set, SIGTERM);
        if (result.has_value())
        {
            /* NOLINTNEXTLINE(score-banned-function) using PthreadSigMask by design. Argumentation: Ticket-101432 */
            result = signal.PthreadSigMask(SIG_BLOCK, sig_set);
        }
    }
    return result;
}

score::cpp::expected<std::int32_t, score::os::Error> SignalHandling::PThreadUnblockSigTerm(score::os::Signal& signal) noexcept
{
    sigset_t sig_set;

    score::cpp::expected<std::int32_t, score::os::Error> return_error_result{};
    auto result = signal.SigEmptySet(sig_set);
    if (result.has_value())
    {
        // signal handling is tolerated by design. Argumentation: Ticket-101432
        // coverity[autosar_cpp14_m18_7_1_violation]
        result = signal.SigAddSet(sig_set, SIGTERM);
        if (result.has_value())
        {
            /* NOLINTNEXTLINE(score-banned-function) using PthreadSigMask by design. Argumentation: Ticket-101432 */
            result = signal.PthreadSigMask(SIG_UNBLOCK, sig_set);
        }
    }
    return result;
}

}  //  namespace score::mw::log::detail
