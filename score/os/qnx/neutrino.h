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

#ifndef SCORE_LIB_OS_QNX_NEUTRINO_H
#define SCORE_LIB_OS_QNX_NEUTRINO_H

#include "score/bitmanipulation/bitmask_operators.h"
#include "score/os/ObjectSeam.h"
#include "score/os/errno.h"
#include "score/os/sigevent.h"
#include "score/os/version.h"
#include <score/expected.hpp>

#include <sys/neutrino.h>
#include <optional>

namespace score
{
namespace os
{
namespace qnx
{

class Neutrino : public ObjectSeam<Neutrino>
{
  public:
// Suppress "AUTOSAR C++14 A16-0-1" rule findings. This rule stated: "The pre-processor shall only be used for
// unconditional and conditional file inclusion and include guards, and using the following directives: (1) #ifndef,
// #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif, (9) #include.".
// Rationale: Pre-processor commands are used to allow different datatypes for TimerFlagsType with QNX7 and QNX8 to
// exist in the same file. It also prevents compiler errors in QNX8 code when compiling for QNX and vice versa.
// coverity[autosar_cpp14_a16_0_1_violation]
#ifdef SPP_OS_QNX8  // QNX 8.x: TimerTimeout uses uint32_t flags
    using TimerFlagsType = std::uint32_t;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#else  // QNX 7.x: TimerTimeout uses uint32_t flags
    using TimerFlagsType = std::int32_t;
// coverity[autosar_cpp14_a16_0_1_violation], see above rationale
#endif
    enum class TCtlCommands : int
    {
        kTCtlIoPriv = 1,
        kTCtlIo = 14,
    };

    // As clarified in: Ticket-145671, the following flags used in bitmasks are unsigned from the kernel's point of view
    enum class TimerTimeoutFlag : std::uint32_t
    {
        kReceive = _NTO_TIMEOUT_RECEIVE,
        kSend = _NTO_TIMEOUT_SEND,
        kReply = _NTO_TIMEOUT_REPLY,
        kSignalSuspend = _NTO_TIMEOUT_SIGSUSPEND,
        kSignalWaitInfo = _NTO_TIMEOUT_SIGWAITINFO,
        kMutex = _NTO_TIMEOUT_MUTEX,
        kConditionVariable = _NTO_TIMEOUT_CONDVAR,
        kJoin = _NTO_TIMEOUT_JOIN,
        kInterrupt = _NTO_TIMEOUT_INTR,
        kSemaphore = _NTO_TIMEOUT_SEM,

        // manipulate otime in TimerTimeout function
        kTimerTolerance = TIMER_TOLERANCE,    // store the previous timer tolerance
        kNanoSleep = _NTO_TIMEOUT_NANOSLEEP,  // the time remaining in sleep

        // to be used along with kTimerTolerance
        kTimerAbsoluteTime = TIMER_ABSTIME,
        kTimerPrecise = TIMER_PRECISE,
    };

    // As clarified in: Ticket-145677, CLOCK_MONOTONIC, CLOCK_REALTIME and CLOCK_SOFTTIME are the only valid clock types
    enum class ClockType
    {
        kRealtime,
        kMonotonic,
        kSoftTime
    };

    // The following assertions are made because the values in the ChannelFlag enum class are used in bitmasks
    // smallest and largest values are unsigned
    static_assert(std::numeric_limits<decltype(_NTO_CHF_FIXED_PRIORITY)>::is_signed == false);
    static_assert(std::numeric_limits<decltype(_NTO_CHF_INHERIT_RUNMASK)>::is_signed == false);
    enum class ChannelFlag : std::uint32_t
    {
        // all values are > 0, by design
        kFixedPriority = _NTO_CHF_FIXED_PRIORITY,
        kUnblock = _NTO_CHF_UNBLOCK,
        kThreadDeath = _NTO_CHF_THREAD_DEATH,
        kDisconnect = _NTO_CHF_DISCONNECT,
        kNetMessage = _NTO_CHF_NET_MSG,
        kConnectionIdDisconnect = _NTO_CHF_COID_DISCONNECT,
        kPrivate = _NTO_CHF_PRIVATE,
        kMessagePausing = _NTO_CHF_MSG_PAUSING,
        kInheritRunmask = _NTO_CHF_INHERIT_RUNMASK,
    };

    static Neutrino& instance() noexcept;
    // TODO: Ticket-25608 Replace score::cpp::expected with score::Result

    virtual score::cpp::expected<int32_t, score::os::Error> ThreadCtl(const int32_t cmd, void* data) const noexcept = 0;

    virtual std::int32_t InterruptWait_r(std::int32_t flags, const std::uint64_t* timeout) = 0;

    virtual std::int32_t InterruptAttachEvent(std::int32_t intr, const struct sigevent* event, unsigned flags) = 0;

    virtual std::int32_t InterruptDetach(std::int32_t id) = 0;

    virtual std::int32_t InterruptUnmask(std::int32_t intr, std::int32_t id) = 0;

    [[deprecated(
        "SPP_DEPRECATION: Please use \'ChannelCreate(const Neutrino::ChannelFlag flags)\'")]] virtual std::int32_t
    ChannelCreate(std::uint32_t flags) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> ChannelCreate(const Neutrino::ChannelFlag flags) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> ChannelDestroy(const std::int32_t channel_id) const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> ClockAdjust(clockid_t id,
                                                           const _clockadjust* _new,
                                                           _clockadjust* old) const noexcept = 0;

    virtual std::uint64_t ClockCycles() const noexcept = 0;

    virtual score::cpp::expected<std::int32_t, Error> ClockId(pid_t pid, std::int32_t tid) const noexcept = 0;

    [[deprecated(
        "SPP_DEPRECATION: Please use other overloads of the \'TimerTimeout\'")]] virtual score::cpp::expected<std::int32_t,
                                                                                                       Error>
    TimerTimeout(clockid_t id,
                 TimerFlagsType flags,
                 const sigevent* notify,
                 const std::uint64_t* ntime,
                 std::uint64_t* otime) const noexcept = 0;

    [[deprecated("SPP_DEPRECATION: Please use the latest overload of the \'TimerTimeout\'")]]
    // This is intented, we don't force users to fill the otime parameter unless needed
    // NOLINTNEXTLINE(google-default-arguments): See above
    virtual score::cpp::expected<std::int32_t, Error> TimerTimeout(
        const ClockType clock_type,
        const TimerTimeoutFlag flags,
        const sigevent* notify,
        const std::chrono::nanoseconds& ntime,
        std::optional<std::chrono::nanoseconds> otime = std::nullopt) const noexcept = 0;

    // This is intented, we don't force users to fill the otime parameter unless needed
    // NOLINTNEXTLINE(google-default-arguments): See above
    virtual score::cpp::expected<std::int32_t, Error> TimerTimeout(
        const ClockType clock_type,
        const TimerTimeoutFlag flags,
        const std::unique_ptr<SigEvent> signal_event,
        const std::chrono::nanoseconds& ntime,
        std::optional<std::chrono::nanoseconds> otime = std::nullopt) const noexcept = 0;

    virtual ~Neutrino() = default;
    // Below member functions declared to avoid autosar_cpp14_a12_0_1_violation
    Neutrino(const Neutrino&) = delete;
    Neutrino& operator=(const Neutrino&) = delete;
    Neutrino(Neutrino&& other) = delete;
    Neutrino& operator=(Neutrino&& other) = delete;

  protected:
    Neutrino() = default;
};

}  // namespace qnx
}  // namespace os

template <>
struct enable_bitmask_operators<score::os::qnx::Neutrino::TimerTimeoutFlag>
{
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.”.
    // Here, variable value is used as part of templatized struct, hence false positive.
    // coverity[autosar_cpp14_a0_1_1_violation]
    static constexpr bool value{true};
};

template <>
struct enable_bitmask_operators<score::os::qnx::Neutrino::ChannelFlag>
{
    // Suppress "AUTOSAR C++14 A0-1-1" rule finding: "A project shall not contain instances of
    // non-volatile variables being given values that are not subsequently used.”.
    // Here, variable value is used as part of templatized struct, hence false positive.
    // coverity[autosar_cpp14_a0_1_1_violation]
    static constexpr bool value{true};
};

}  // namespace score

#endif  // SCORE_LIB_OS_QNX_NEUTRINO_H
