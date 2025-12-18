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
#include "score/os/qnx/sigevent_qnx_impl.h"
#include <gtest/gtest.h>
#include <sys/neutrino.h>
#include <iostream>

using namespace score::os::qnx;

class NeutrinoImplFixture : public ::testing::Test
{
  protected:
    NeutrinoImpl neutrino_;
};

TEST_F(NeutrinoImplFixture, ThreadCtlTestReturnsError)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Thread Ctl Test returns Error");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t cmd = 0xFFFFFFFF;
    std::int32_t data = 10;

    const auto val = Neutrino::instance().ThreadCtl(cmd, &data);
    EXPECT_FALSE(val.has_value());
    EXPECT_EQ(val.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(NeutrinoImplFixture, ThreadCtlGetThreadNme)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Thread Ctl Get Thread Nme");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::int32_t valid_cmd{_NTO_TCTL_NAME};
    struct _thread_name thread_name{};
    thread_name.new_name_len = -1;
    void* data = &thread_name;
    const auto result = neutrino_.ThreadCtl(valid_cmd, data);

    ASSERT_TRUE(result.has_value());
    EXPECT_GE(result.value(), 0);

    char name_buffer[32U];
    ASSERT_EQ(::pthread_getname_np(0, name_buffer, 32U), EOK);
    EXPECT_STREQ(thread_name.name_buf, name_buffer);
}

TEST_F(NeutrinoImplFixture, InterruptAttachAndDetachTest)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Interrupt Attach And Detach Test");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::uint64_t timeout{0U};
    std::int32_t intr{10};
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGUSR1;
    event.sigev_value.sival_ptr = NULL;
    unsigned int new_flags{0U};

    std::int32_t id = neutrino_.InterruptAttachEvent(intr, &event, new_flags);
    EXPECT_NE(id, -1);
#if defined(__QNX__)
#if __QNX__ < 800  // QNX 7.x or earlier
    EXPECT_NE(neutrino_.InterruptUnmask(intr, id), -1);
#endif
#endif

    EXPECT_NE(neutrino_.InterruptWait_r(id, &timeout), EOK);

    std::uint32_t ret = neutrino_.InterruptDetach(id);
    EXPECT_NE(ret, -1);
}

TEST_F(NeutrinoImplFixture, ChannelCreateDeprecatedSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Create Deprecated Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::uint32_t valid_flags{0U};
    const auto result = neutrino_.ChannelCreate(valid_flags);

    ASSERT_GE(result, 0);
}

TEST_F(NeutrinoImplFixture, ChannelCreateDeprecatedFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Create Deprecated Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto invalid_flags{std::numeric_limits<std::uint32_t>::max()};
    const auto result = neutrino_.ChannelCreate(invalid_flags);

    ASSERT_EQ(result, -1);
    // errno value is OS specific
}

TEST_F(NeutrinoImplFixture, ChannelCreateSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Create Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto result = neutrino_.ChannelCreate(Neutrino::ChannelFlag::kDisconnect);
    ASSERT_TRUE(result.has_value());
}

TEST_F(NeutrinoImplFixture, ChannelCreateFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Create Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto result =
        neutrino_.ChannelCreate(static_cast<Neutrino::ChannelFlag>(std::numeric_limits<std::uint32_t>::max()));
    ASSERT_FALSE(result.has_value());
    // errno value is OS specific
}

TEST_F(NeutrinoImplFixture, ChannelDestroySuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Destroy Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto channel_id = neutrino_.ChannelCreate(Neutrino::ChannelFlag::kDisconnect);
    ASSERT_TRUE(channel_id.has_value());

    const auto destroy_result = neutrino_.ChannelDestroy(channel_id.value());
    ASSERT_TRUE(destroy_result.has_value());
}

TEST_F(NeutrinoImplFixture, ChannelDestroyFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Channel Destroy Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const std::int32_t failed_channel_id = 0;
    const auto destroy_result = neutrino_.ChannelDestroy(failed_channel_id);
    ASSERT_FALSE(destroy_result.has_value());
}

TEST_F(NeutrinoImplFixture, ClockAdjustSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Clock Adjust Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    clockid_t clockid{CLOCK_REALTIME};
    const auto tick_nsec{500000000};
    _clockadjust new_adjust{1, tick_nsec};
    _clockadjust old_adjust{0, 0};

    const auto result = neutrino_.ClockAdjust(clockid, &new_adjust, &old_adjust);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    EXPECT_GE((new_adjust.tick_count * new_adjust.tick_nsec_inc) - (old_adjust.tick_count * old_adjust.tick_nsec_inc),
              tick_nsec);
}

TEST_F(NeutrinoImplFixture, ClockAdjust_Failure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Clock Adjust Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    clockid_t invalid_clockid{-1};
    _clockadjust new_adjust{0, 0};

    const auto result = neutrino_.ClockAdjust(invalid_clockid, &new_adjust, nullptr);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(NeutrinoImplFixture, TimerTimeoutDeprecatedSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Deprecated Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    clockid_t clockid{CLOCK_REALTIME};
    std::int32_t flags{0};
    const sigevent* notify{nullptr};
    const std::uint64_t ntime{1000000U};
    std::uint64_t otime{0U};

    const auto result1 = neutrino_.TimerTimeout(clockid, flags, notify, &ntime, &otime);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 0);

    std::chrono::milliseconds min_sleep{2};
    auto result2 =
        neutrino_.TimerTimeout(Neutrino::ClockType::kRealtime, Neutrino::TimerTimeoutFlag::kSend, nullptr, min_sleep);
    ASSERT_TRUE(result2.has_value());
    result2 =
        neutrino_.TimerTimeout(Neutrino::ClockType::kMonotonic, Neutrino::TimerTimeoutFlag::kSend, nullptr, min_sleep);
    ASSERT_TRUE(result2.has_value());
    result2 =
        neutrino_.TimerTimeout(Neutrino::ClockType::kSoftTime, Neutrino::TimerTimeoutFlag::kSend, nullptr, min_sleep);
    ASSERT_TRUE(result2.has_value());

    std::chrono::milliseconds sleep_left{0};
    const auto result3 = neutrino_.TimerTimeout(
        Neutrino::ClockType::kRealtime, Neutrino::TimerTimeoutFlag::kTimerTolerance, nullptr, sleep_left);
    ASSERT_TRUE(result3.has_value());
}

TEST_F(NeutrinoImplFixture, TimerTimeoutDeprecatedFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Deprecated Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    clockid_t clockid{-1};
    std::int32_t flags{0};
    const sigevent* notify{nullptr};
    const std::uint64_t ntime{1000000U};
    std::uint64_t otime{0U};

    const auto result1 = neutrino_.TimerTimeout(clockid, flags, notify, &ntime, &otime);
    ASSERT_FALSE(result1.has_value());
    EXPECT_EQ(result1.error(), score::os::Error::Code::kInvalidArgument);

    std::chrono::milliseconds min_sleep{2};
    const auto result2 = neutrino_.TimerTimeout(
        static_cast<Neutrino::ClockType>(-1), Neutrino::TimerTimeoutFlag::kSend, nullptr, min_sleep);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), score::os::Error::Code::kInvalidArgument);
}

TEST_F(NeutrinoImplFixture, TimerTimeoutSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Timer Timeout Success");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::unique_ptr<score::os::SigEvent> signal_event1 = std::make_unique<score::os::SigEventQnxImpl>();
    std::chrono::milliseconds min_sleep{2};

    auto* signal_event_qnx_ptr = dynamic_cast<score::os::SigEventQnxImpl*>(signal_event1.get());
    ASSERT_NE(signal_event_qnx_ptr, nullptr);

    // Proceed only if cast succeeded
    signal_event_qnx_ptr->SetUnblock();

    auto result = neutrino_.TimerTimeout(
        Neutrino::ClockType::kRealtime, Neutrino::TimerTimeoutFlag::kSend, std::move(signal_event1), min_sleep);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    std::optional<std::chrono::nanoseconds> otime = std::chrono::nanoseconds{0};
    std::unique_ptr<score::os::SigEvent> signal_event2 = std::make_unique<score::os::SigEventQnxImpl>();

    signal_event_qnx_ptr = dynamic_cast<score::os::SigEventQnxImpl*>(signal_event2.get());
    ASSERT_NE(signal_event_qnx_ptr, nullptr);

    // Proceed only if cast succeeded
    signal_event_qnx_ptr->SetUnblock();

    result = neutrino_.TimerTimeout(Neutrino::ClockType::kMonotonic,
                                    Neutrino::TimerTimeoutFlag::kNanoSleep,
                                    std::move(signal_event2),
                                    min_sleep,
                                    otime);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

TEST_F(NeutrinoImplFixture, TimerTimeoutFailure)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test TimerTimeout Failure");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    std::unique_ptr<score::os::SigEvent> signal_event = nullptr;
    std::chrono::milliseconds ntime{2};

    auto result = neutrino_.TimerTimeout(
        Neutrino::ClockType::kRealtime, Neutrino::TimerTimeoutFlag::kSend, std::move(signal_event), ntime);
    ASSERT_FALSE(result.has_value());

    signal_event = std::make_unique<score::os::SigEventQnxImpl>();
    result = neutrino_.TimerTimeout(
        static_cast<Neutrino::ClockType>(-1), Neutrino::TimerTimeoutFlag::kSend, std::move(signal_event), ntime);
    ASSERT_FALSE(result.has_value());
}

TEST_F(NeutrinoImplFixture, ClockCyclesMonotonicity)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Clock Cycles Monotonicity");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto start = neutrino_.ClockCycles();
    const auto end = neutrino_.ClockCycles();
    ASSERT_GE(end, start);
}

TEST_F(NeutrinoImplFixture, ClockCyclesElapsedTime)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Test Clock Cycles Elapsed Time");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto start = neutrino_.ClockCycles();
    for (volatile int i = 0; i < 1000000; ++i)
        ;
    const auto end = neutrino_.ClockCycles();
    ASSERT_GT(end, start);
}

TEST_F(NeutrinoImplFixture, ClockIdSuccess)
{
    RecordProperty("ParentRequirement", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "");
    RecordProperty("TestingTechnique", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    pid_t pid = ::getpid();
    int32_t tid = ::gettid();

    const auto clock_id = neutrino_.ClockId(pid, tid);
    ASSERT_TRUE(clock_id.has_value());
    ASSERT_GE(clock_id.value(), 0);
}
