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
#include "score/os/utils/signal_impl.h"
#include "score/os/unistd.h"

#include <score/utility.hpp>

namespace score
{
namespace os
{

namespace
{

/* The macros usage is restricted to this enum class, to reduce overall number of macro usages. */
enum class SignalValue : std::int32_t
{
    SigBlock = SIG_BLOCK,
    SigSetMask = SIG_SETMASK,
    SigTerm = SIGTERM,
};

std::underlying_type_t<SignalValue> SignalValueToPosixType(const SignalValue signal_value)
{
    return static_cast<std::underlying_type_t<SignalValue>>(signal_value);
}

score::cpp::expected<std::int32_t, Error> ConvertReturnValueToExpected(const std::int32_t value)
{
    if (value == -1) /* LCOV_EXCL_BR_LINE: Error cannot be reproduced in scope of unit tests.*/
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno()); /* LCOV_EXCL_LINE */
        /* Error cannot be reproduced in scope of unit tests. */
    }
    return {value};
}
}  // namespace

score::cpp::expected<std::int32_t, Error> SignalImpl::SigEmptySet(sigset_t& set) const noexcept
{
    const std::int32_t ret = ::sigemptyset(&set);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::SigAddSet(sigset_t& set, const std::int32_t signo) const noexcept
{
    const std::int32_t ret = ::sigaddset(&set, signo);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::PthreadSigMask(const sigset_t& signals) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::pthread_sigmask(SignalValueToPosixType(SignalValue::SigSetMask), &signals, nullptr);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::pthread_sigmask(how, &set, nullptr);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set,
                                                              sigset_t& oldset) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::pthread_sigmask(how, &set, &oldset);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::SendSelfSigterm() const noexcept
{
    const std::unique_ptr<score::os::Unistd> unistd = score::os::Unistd::Default();
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    return SignalImpl::Kill(unistd->getpid(), SignalValueToPosixType(SignalValue::SigTerm));
}

score::cpp::expected<std::int32_t, Error> SignalImpl::AddTerminationSignal(sigset_t& add_signal) const noexcept
{
    return SignalImpl::SigAddSet(add_signal, SignalValueToPosixType(SignalValue::SigTerm));
}

score::cpp::expected<std::int32_t, Error> SignalImpl::GetCurrentBlockedSignals(sigset_t& signals) const noexcept
{
    const auto result(SignalImpl::SigEmptySet(signals));
    if (result.has_value() == false) /* LCOV_EXCL_BR_LINE */
    {
        return result;  // LCOV_EXCL_LINE
        /*Not possible to cover through unit test. It is not possible to make sigempty return -1. */
    }

    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTBEGIN(score-banned-function) see comment above
    const std::int32_t ret = ::pthread_sigmask(SignalValueToPosixType(SignalValue::SigBlock), nullptr, &signals);
    // NOLINTEND(score-banned-function) see comment above
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::IsSignalBlocked(const std::int32_t signal_id) const noexcept
{
    sigset_t signals{};
    auto result(SignalImpl::SigEmptySet(signals));
    if (result.has_value() == false) /* LCOV_EXCL_BR_LINE */
    {
        return result;  // LCOV_EXCL_LINE
        /*Not possible to cover through unit test. It is not possible to make sigempty return -1. */
    }
    result = SignalImpl::GetCurrentBlockedSignals(signals);
    if (result.has_value() == false) /* LCOV_EXCL_BR_LINE */
    {
        return result;  // LCOV_EXCL_LINE
        /*Not possible to cover through unit test. It is not possible to make GetCurrentBlockedSignals return an
         * error.*/
    }
    return SignalImpl::SigIsMember(signals, signal_id);
}
score::cpp::expected<std::int32_t, Error> SignalImpl::SigIsMember(sigset_t& signals,
                                                           const std::int32_t signal_id) const noexcept
{
    const std::int32_t ret = ::sigismember(&signals, signal_id);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::SigFillSet(sigset_t& set) const noexcept
{
    const std::int32_t ret = ::sigfillset(&set);
    return ConvertReturnValueToExpected(ret);
}

/* It is being tested by ITF. It appears to be impossible to test it using a unit test. */
/* Negative Test: CI fails on it, thread sanitizer reports sigset_t pointer could not be nullptr. */
/* Positive Test: If a signal is raised gtest detects it and mark test as failed. */
/* LCOV_EXCL_START */
score::cpp::expected<std::int32_t, Error> SignalImpl::SigWait(const sigset_t& set, std::int32_t& sig) const noexcept
{
    const std::int32_t ret = ::sigwait(&set, &sig);
    return ConvertReturnValueToExpected(ret);
}
/* LCOV_EXCL_STOP */

score::cpp::expected<std::int32_t, Error> SignalImpl::SigAction(const std::int32_t signum,
                                                         const struct sigaction& action,
                                                         struct sigaction& old_action) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::sigaction(signum, &action, &old_action);
    return ConvertReturnValueToExpected(ret);
}

score::cpp::expected<std::int32_t, Error> SignalImpl::Kill(const pid_t pid, const std::int32_t sig) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function) see comment above
    const std::int32_t ret = ::kill(pid, sig);
    return ConvertReturnValueToExpected(ret);
}

}  // namespace os
}  // namespace score
