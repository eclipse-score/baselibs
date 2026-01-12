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
#include "score/os/qnx/neutrino_impl.h"

namespace score
{
namespace os
{
namespace qnx
{

/* The only non-const variables or parameters may actually be modified by the QNX */
score::cpp::expected<int32_t, score::os::Error> NeutrinoImpl::ThreadCtl(const int32_t cmd, void* data) const noexcept
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function): See above
    const int32_t ret = ::ThreadCtl(cmd, data);
    if (ret == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return ret;
}

std::int32_t NeutrinoImpl::InterruptWait_r(std::int32_t flags, const std::uint64_t* timeout)
{
    return ::InterruptWait_r(flags, timeout);
}

std::int32_t NeutrinoImpl::InterruptAttachEvent(std::int32_t intr, const struct sigevent* event, unsigned flags)
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function): See above
    return ::InterruptAttachEvent(intr, event, flags);
}

std::int32_t NeutrinoImpl::InterruptDetach(std::int32_t id)
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function): See above
    return ::InterruptDetach(id);
}

std::int32_t NeutrinoImpl::InterruptUnmask(std::int32_t intr, std::int32_t id)
{
    // Suppressed here because usage of this OSAL method is on banned list
    // NOLINTNEXTLINE(score-banned-function): See above
    return ::InterruptUnmask(intr, id);
}

std::int32_t NeutrinoImpl::ChannelCreate(std::uint32_t flags) const noexcept
{
    return ::ChannelCreate(flags);
}

score::cpp::expected<std::int32_t, Error> NeutrinoImpl::ChannelCreate(const Neutrino::ChannelFlag flags) const noexcept
{
    const std::int32_t created_channel_id = ::ChannelCreate(ChannelFlagToNativeFlag(flags));
    if (created_channel_id == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return created_channel_id;
}

score::cpp::expected<std::int32_t, Error> NeutrinoImpl::ChannelDestroy(const std::int32_t channel_id) const noexcept
{
    const std::int32_t destroy_result = ::ChannelDestroy(channel_id);
    if (destroy_result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return destroy_result;
}

score::cpp::expected<std::int32_t, Error> NeutrinoImpl::ClockAdjust(clockid_t id,
                                                             const _clockadjust* _new,
                                                             _clockadjust* old) const noexcept
{
    const std::int32_t ret{::ClockAdjust(id, _new, old)};
    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

score::cpp::expected<std::int32_t, Error> NeutrinoImpl::TimerTimeout(clockid_t id,
                                                              TimerFlagsType flags,
                                                              const sigevent* notify,
                                                              const std::uint64_t* ntime,
                                                              std::uint64_t* otime) const noexcept
{

    const std::int32_t ret{::TimerTimeout(id, flags, notify, ntime, otime)};

    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }
    return ret;
}

std::uint64_t NeutrinoImpl::ClockCycles() const noexcept
{
    return ::ClockCycles();
}

score::cpp::expected<std::int32_t, Error> NeutrinoImpl::ClockId(pid_t pid, std::int32_t tid) const noexcept
{
    const std::int32_t result = ::ClockId(pid, tid);
    if (result == -1)
    {
        return score::cpp::make_unexpected(score::os::Error::createFromErrno());
    }
    return result;
}

// This is intented, we don't force users to fill the otime parameter unless needed
// NOLINTNEXTLINE(google-default-arguments): See above
score::cpp::expected<std::int32_t, Error> NeutrinoImpl::TimerTimeout(
    const Neutrino::ClockType clock_type,
    const Neutrino::TimerTimeoutFlag flags,
    const sigevent* notify,
    const std::chrono::nanoseconds& ntime,
    std::optional<std::chrono::nanoseconds> otime) const noexcept
{
    const auto nano_in = static_cast<std::uint64_t>(ntime.count());
    std::uint64_t nano_out{};
    std::uint64_t* nano_time = otime.has_value() ? &nano_out : nullptr;

    const std::int32_t ret{::TimerTimeout(ClockTypeToNativeClock(clock_type),
                                          // As clarified in Ticket-145671, TimerTimeout flags in bitmask are unsigned
                                          // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions): See above
                                          static_cast<TimerFlagsType>(TimerTimeoutFlagToNativeFlag(flags)),
                                          notify,
                                          &nano_in,
                                          nano_time)};

    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }

    if (((static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(Neutrino::TimerTimeoutFlag::kNanoSleep)) !=
         0U) ||
        ((static_cast<std::uint32_t>(flags) &
          static_cast<std::uint32_t>(Neutrino::TimerTimeoutFlag::kTimerTolerance)) != 0U))
    {
        otime = std::chrono::nanoseconds{nano_out};
    }
    return ret;
}

// This is intented, we don't force users to fill the otime parameter unless needed
// NOLINTNEXTLINE(google-default-arguments): See above
score::cpp::expected<std::int32_t, Error> NeutrinoImpl::TimerTimeout(
    const Neutrino::ClockType clock_type,
    const Neutrino::TimerTimeoutFlag flags,
    const std::unique_ptr<SigEvent> signal_event,
    const std::chrono::nanoseconds& ntime,
    std::optional<std::chrono::nanoseconds> otime) const noexcept
{
    if (signal_event == nullptr)
    {
        return score::cpp::make_unexpected(Error::createUnspecifiedError());
    }

    const auto nano_in = static_cast<std::uint64_t>(ntime.count());
    std::uint64_t nano_out{};
    auto nano_time = otime.has_value() ? &nano_out : nullptr;
    const auto& raw_signal_event = signal_event->GetSigevent();

    const std::int32_t ret{::TimerTimeout(ClockTypeToNativeClock(clock_type),
                                          // As clarified in Ticket-145671, TimerTimeout flags in bitmask are unsigned
                                          // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions): See above
                                          static_cast<TimerFlagsType>(TimerTimeoutFlagToNativeFlag(flags)),
                                          &raw_signal_event,
                                          &nano_in,
                                          nano_time)};

    if (ret == -1)
    {
        return score::cpp::make_unexpected(Error::createFromErrno());
    }

    if (((static_cast<std::uint32_t>(flags) & static_cast<std::uint32_t>(Neutrino::TimerTimeoutFlag::kNanoSleep)) !=
         0U) ||
        ((static_cast<std::uint32_t>(flags) &
          static_cast<std::uint32_t>(Neutrino::TimerTimeoutFlag::kTimerTolerance)) != 0U))
    {
        otime = std::chrono::nanoseconds{nano_out};
    }
    return ret;
}

clockid_t NeutrinoImpl::ClockTypeToNativeClock(const Neutrino::ClockType clock_type) const noexcept
{
    clockid_t native_clock_type{-1};
    switch (clock_type)
    {
        case Neutrino::ClockType::kRealtime:
        {
            native_clock_type = CLOCK_REALTIME;
            break;
        }

        case Neutrino::ClockType::kMonotonic:
        {
            native_clock_type = CLOCK_MONOTONIC;
            break;
        }

        case Neutrino::ClockType::kSoftTime:
        {
            native_clock_type = CLOCK_SOFTTIME;
            break;
        }

        default:
            break;
    }
    return native_clock_type;
}

constexpr std::underlying_type_t<Neutrino::TimerTimeoutFlag> NeutrinoImpl::TimerTimeoutFlagToNativeFlag(
    const Neutrino::TimerTimeoutFlag flags) const noexcept
{
    return static_cast<std::underlying_type_t<Neutrino::TimerTimeoutFlag>>(flags);
}

constexpr std::underlying_type_t<Neutrino::ChannelFlag> NeutrinoImpl::ChannelFlagToNativeFlag(
    const Neutrino::ChannelFlag flags) const noexcept
{
    return static_cast<std::underlying_type_t<Neutrino::ChannelFlag>>(flags);
}

}  // namespace qnx
}  // namespace os
}  // namespace score
