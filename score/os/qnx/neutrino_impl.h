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

#ifndef SCORE_LIB_OS_QNX_NEUTRINO_IMPL_H
#define SCORE_LIB_OS_QNX_NEUTRINO_IMPL_H

#include "score/os/qnx/neutrino.h"

namespace score
{
namespace os
{
namespace qnx
{

class NeutrinoImpl final : public Neutrino
{
  public:
    constexpr NeutrinoImpl() = default;
    /* The only non-const variables or parameters may actually be modified by the QNX */
    score::cpp::expected<int32_t, score::os::Error> ThreadCtl(const int32_t cmd, void* data) const noexcept override;
    std::int32_t InterruptWait_r(std::int32_t flags, const std::uint64_t* timeout) override;

    std::int32_t InterruptAttachEvent(std::int32_t intr, const struct sigevent* event, unsigned flags) override;

    std::int32_t InterruptDetach(std::int32_t id) override;

    std::int32_t InterruptUnmask(std::int32_t intr, std::int32_t id) override;

    [[deprecated("SPP_DEPRECATION: Please use \'ChannelCreate(const Neutrino::ChannelFlag flags)\'")]] std::int32_t
    ChannelCreate(std::uint32_t flags) const noexcept override;

    score::cpp::expected<std::int32_t, Error> ChannelCreate(const Neutrino::ChannelFlag flags) const noexcept override;

    score::cpp::expected<std::int32_t, Error> ChannelDestroy(const std::int32_t channel_id) const noexcept override;

    score::cpp::expected<std::int32_t, Error> ClockAdjust(clockid_t id,
                                                   const _clockadjust* _new,
                                                   _clockadjust* old) const noexcept override;

    std::uint64_t ClockCycles() const noexcept override;

    score::cpp::expected<std::int32_t, Error> ClockId(pid_t pid, std::int32_t tid) const noexcept override;

    [[deprecated(
        "SPP_DEPRECATION: Please use other overloads of the \'TimerTimeout\'")]] score::cpp::expected<std::int32_t, Error>
    TimerTimeout(clockid_t id,
                 TimerFlagsType flags,
                 const sigevent* notify,
                 const std::uint64_t* ntime,
                 std::uint64_t* otime) const noexcept override;

    // NOLINTBEGIN(google-default-arguments): See rationale below
    // Suppress "AUTOSAR C++14 M8-3-1" rule finding: "Parameters in an overriding virtual function shall either use the
    // same default arguments as the function they override, or else shall not specify any default arguments.”
    // Rationale: This is intented, we don't force users to fill the otime parameter unless needed
    [[deprecated(
        "SPP_DEPRECATION: Please use the latest overload of the \'TimerTimeout\'")]] score::cpp::expected<std::int32_t, Error>
    TimerTimeout(const Neutrino::ClockType clock_type,
                 const Neutrino::TimerTimeoutFlag flags,
                 const struct sigevent* notify,
                 const std::chrono::nanoseconds& ntime,
                 // coverity[autosar_cpp14_m8_3_1_violation]
                 std::optional<std::chrono::nanoseconds> otime = std::nullopt) const noexcept override;
    // NOLINTEND(google-default-arguments)

    // NOLINTBEGIN(google-default-arguments): See rationale below
    // Suppress "AUTOSAR C++14 M8-3-1" rule finding: "Parameters in an overriding virtual function shall either use the
    // same default arguments as the function they override, or else shall not specify any default arguments.”
    // Rationale: This is intented, we don't force users to fill the otime parameter unless needed
    score::cpp::expected<std::int32_t, Error> TimerTimeout(
        const Neutrino::ClockType clock_type,
        const Neutrino::TimerTimeoutFlag flags,
        const std::unique_ptr<SigEvent> signal_event,
        const std::chrono::nanoseconds& ntime,
        // coverity[autosar_cpp14_m8_3_1_violation]
        std::optional<std::chrono::nanoseconds> otime = std::nullopt) const noexcept override;
    // NOLINTEND(google-default-arguments)
  private:
    constexpr std::underlying_type_t<Neutrino::TimerTimeoutFlag> TimerTimeoutFlagToNativeFlag(
        const Neutrino::TimerTimeoutFlag flags) const noexcept;
    clockid_t ClockTypeToNativeClock(const Neutrino::ClockType clock_type) const noexcept;
    constexpr std::underlying_type_t<Neutrino::ChannelFlag> ChannelFlagToNativeFlag(
        const Neutrino::ChannelFlag flags) const noexcept;
};

}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_QNX_NEUTRINO_IMPL_H
