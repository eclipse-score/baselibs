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
#ifndef SCORE_LIB_OS_MOCKLIB_NEUTRINO_MOCK_H
#define SCORE_LIB_OS_MOCKLIB_NEUTRINO_MOCK_H

#include "score/os/qnx/neutrino.h"

#include <gmock/gmock.h>

namespace score
{
namespace os
{
namespace qnx
{

class NeutrinoMock : public Neutrino
{
  public:
    ~NeutrinoMock() override = default;
    MOCK_METHOD((score::cpp::expected<int, score::os::Error>), ThreadCtl, (int cmd, void* data), (const, noexcept, override));
    MOCK_METHOD((std::int32_t), InterruptWait_r, (std::int32_t flags, const std::uint64_t* timeout), (override));
    MOCK_METHOD((std::int32_t),
                InterruptAttachEvent,
                (std::int32_t intr, const struct sigevent* event, unsigned flags),
                (override));
    MOCK_METHOD((std::int32_t), InterruptDetach, (std::int32_t id), (override));
    MOCK_METHOD((std::int32_t), InterruptUnmask, (std::int32_t intr, std::int32_t id), (override));
    MOCK_METHOD((std::int32_t), ChannelCreate, (std::uint32_t flags), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>), ChannelCreate, (ChannelFlag flags), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                ChannelDestroy,
                (const std::int32_t channel_id),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                ClockAdjust,
                (clockid_t id, const _clockadjust* _new, _clockadjust* old),
                (const, noexcept, override));
    MOCK_METHOD(
        (score::cpp::expected<std::int32_t, Error>),
        TimerTimeout,
        (clockid_t id, TimerFlagsType flags, const sigevent* notify, const std::uint64_t* ntime, std::uint64_t* otime),
        (const, noexcept, override));
    MOCK_METHOD(std::uint64_t, ClockCycles, (), (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                ClockId,
                (pid_t pid, std::int32_t tid),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                TimerTimeout,
                (const ClockType clock_type,
                 const TimerTimeoutFlag flags,
                 const sigevent* notify,
                 const std::chrono::nanoseconds& ntime,
                 std::optional<std::chrono::nanoseconds> otime),
                (const, noexcept, override));
    MOCK_METHOD((score::cpp::expected<std::int32_t, Error>),
                TimerTimeout,
                (const ClockType clock_type,
                 const TimerTimeoutFlag flags,
                 const std::unique_ptr<SigEvent> signal_event,
                 const std::chrono::nanoseconds& ntime,
                 std::optional<std::chrono::nanoseconds> otime),
                (const, noexcept, override));
};
}  // namespace qnx
}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_MOCKLIB_NEUTRINO_MOCK_H
