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

#ifndef SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H
#define SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H

#include "score/os/utils/signal.h"

namespace score::mw::log::detail
{

class SignalHandling
{
  public:
    /// \brief Blocks the SIGTERM signal for the current thread.
    /// \param signal The signal interface to use for blocking.
    /// \returns The result of the pthread_sigmask call, or an error if the operation failed.
    static score::cpp::expected<std::int32_t, score::os::Error> PThreadBlockSigTerm(score::os::Signal& signal) noexcept;

    /// \brief Unblocks the SIGTERM signal for the current thread.
    /// \param signal The signal interface to use for unblocking.
    /// \returns The result of the pthread_sigmask call, or an error if the operation failed.
    static score::cpp::expected<std::int32_t, score::os::Error> PThreadUnblockSigTerm(score::os::Signal& signal) noexcept;

    /// \brief Executes a function with SIGTERM blocked for the current thread.
    /// \details This is a RAII-style helper that blocks SIGTERM before executing the provided function
    ///          and automatically unblocks it after the function completes (regardless of the outcome).
    ///          This is useful for protecting critical sections from being interrupted by termination signals.
    /// \tparam Func The type of the callable to execute.
    /// \param signal The signal interface to use for blocking/unblocking.
    /// \param func The callable to execute while SIGTERM is blocked e.g. creating a new thread which should not
    /// intercept SIGTERM handling.
    /// \returns The result of the provided callable.
    /// \note Errors from blocking/unblocking operations are silently ignored.
    template <typename Func>
    static auto WithSigTermBlocked(score::os::Signal& signal, Func&& func) noexcept -> decltype(func())
    {
        const auto block_result = PThreadBlockSigTerm(signal);
        auto result = func();
        const auto unblock_result = PThreadUnblockSigTerm(signal);
        std::ignore = unblock_result;
        return result;
    }
};

}  //  namespace score::mw::log::detail

#endif  //  SCORE_MW_LOG_DETAIL_UTILS_SIGNAL_HANDLING_SIGNAL_HANDLING_H
