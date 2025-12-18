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
#include "score/os/mocklib/qnx/neutrino_qnx_mock.h"
#include "score/os/qnx/sigevent_qnx_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using Neutrino = score::os::qnx::Neutrino;

namespace
{
static constexpr auto kTimeout = std::chrono::milliseconds{100};
static constexpr auto kClockType{Neutrino::ClockType::kRealtime};
static constexpr auto kTimeoutFlags{Neutrino::TimerTimeoutFlag::kSend | Neutrino::TimerTimeoutFlag::kReply};
}  // namespace

using namespace testing;

struct NeutrinoMockTest : ::testing::Test
{
    void SetUp() override
    {
        unblock_event.sigev_notify = SIGEV_UNBLOCK;
        neutrino_mock_ptr = std::make_shared<score::os::qnx::NeutrinoMock>();
        neutrino_ptr = neutrino_mock_ptr;
    }

    void TearDown() override
    {
        EXPECT_TRUE(::testing::Mock::VerifyAndClearExpectations(neutrino_mock_ptr.get()));
        neutrino_ptr.reset();
    }

    std::shared_ptr<score::os::qnx::NeutrinoMock> neutrino_mock_ptr{};
    std::shared_ptr<score::os::qnx::Neutrino> neutrino_ptr{};
    sigevent unblock_event{};
};

TEST_F(NeutrinoMockTest, TimerTimeoutRawSigevent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    sigevent raw_signal_event;
    EXPECT_CALL(*neutrino_mock_ptr, TimerTimeout(kClockType, _, testing::Matcher<const sigevent*>(_), _, _)).Times(1);
    neutrino_ptr->TimerTimeout(kClockType, kTimeoutFlags, &raw_signal_event, kTimeout, std::nullopt);
}

TEST_F(NeutrinoMockTest, TimerTimeout)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::unique_ptr<score::os::SigEvent> signal_event{std::make_unique<score::os::SigEventQnxImpl>()};
    EXPECT_CALL(*neutrino_mock_ptr,
                TimerTimeout(kClockType, _, testing::Matcher<std::unique_ptr<score::os::SigEvent>>(_), _, _))
        .Times(1);
    neutrino_ptr->TimerTimeout(kClockType, kTimeoutFlags, std::move(signal_event), kTimeout, std::nullopt);
}

TEST_F(NeutrinoMockTest, ClockAdjust)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Clock Adjust");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    _clockadjust adjustment{0, 0};

    EXPECT_CALL(*neutrino_mock_ptr, ClockAdjust);
    neutrino_ptr->ClockAdjust(CLOCK_REALTIME, &adjustment, nullptr);
}

TEST_F(NeutrinoMockTest, ThreadCtl)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Thread Ctl");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t cmd = _NTO_TCTL_IO;

    EXPECT_CALL(*neutrino_mock_ptr, ThreadCtl);
    neutrino_ptr->ThreadCtl(cmd, nullptr);
}

TEST_F(NeutrinoMockTest, InterruptWait_r)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Interrupt Wait R");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t flags = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptWait_r);
    neutrino_ptr->InterruptWait_r(flags, nullptr);
}

TEST_F(NeutrinoMockTest, InterruptAttachEvent)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Interrupt Attach Event");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t intr = _NTO_INTR_CLASS_EXTERNAL;
    const unsigned flags = _NTO_INTR_FLAGS_END;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptAttachEvent);
    neutrino_ptr->InterruptAttachEvent(intr, &unblock_event, flags);
}

TEST_F(NeutrinoMockTest, InterruptDetach)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Interrupt Detach");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t id = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptDetach);
    neutrino_ptr->InterruptDetach(id);
}

TEST_F(NeutrinoMockTest, InterruptUnmask)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Interrupt Unmask");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t intr = _NTO_INTR_CLASS_EXTERNAL;
    const std::int32_t id = 0;

    EXPECT_CALL(*neutrino_mock_ptr, InterruptUnmask);
    neutrino_ptr->InterruptUnmask(intr, id);
}

TEST_F(NeutrinoMockTest, ChannelCreate)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Create");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto flags{Neutrino::ChannelFlag::kConnectionIdDisconnect};

    EXPECT_CALL(*neutrino_mock_ptr, ChannelCreate(flags));
    neutrino_ptr->ChannelCreate(flags);
}

TEST_F(NeutrinoMockTest, ChannelDestroy)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Destroy");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    const auto dummy_channel_id = 0;
    EXPECT_CALL(*neutrino_mock_ptr, ChannelDestroy(dummy_channel_id));
    neutrino_ptr->ChannelDestroy(dummy_channel_id);
}
