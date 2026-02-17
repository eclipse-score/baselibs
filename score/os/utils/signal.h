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
#ifndef SCORE_LIB_OS_UTILS_SIGNAL_H
#define SCORE_LIB_OS_UTILS_SIGNAL_H

#include <csignal>
#include <cstdint>
#include <memory>

#include <score/expected.hpp>

#include "score/memory.hpp"
#include "score/os/errno.h"

namespace score
{
namespace os
{

class Signal
{
  public:
    virtual ~Signal() = default;
    // Below special member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&& other) = delete;
    Signal& operator=(Signal&& other) = delete;

    using Sighandler = void(std::int32_t);

    /**
     * A note on the Sighandler* signal(const std::int32_t sig, Sighandler handler)
     * We choose not to support this function as its behavior varies across UNIX versions and implementations.
     * Please use SigAction instead.
     */
    virtual score::cpp::expected<std::int32_t, Error> AddTerminationSignal(sigset_t& add_signal) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SendSelfSigterm() const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> GetCurrentBlockedSignals(sigset_t& signals) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> IsSignalBlocked(const std::int32_t signal_id) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigIsMember(sigset_t& signals,
                                                           const std::int32_t signal_id) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigFillSet(sigset_t& set) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigEmptySet(sigset_t& set) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigWait(const sigset_t& set, std::int32_t& sig) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigAddSet(sigset_t& set, const std::int32_t signo) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const sigset_t& signals) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> PthreadSigMask(const std::int32_t how,
                                                              const sigset_t& set,
                                                              sigset_t& oldset) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> SigAction(const std::int32_t signum,
                                                         const struct sigaction& action,
                                                         struct sigaction& old_action) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> Kill(const pid_t pid, const std::int32_t sig) const noexcept = 0;

  protected:
    Signal() = default;
};

}  // namespace os
}  // namespace score

#endif
