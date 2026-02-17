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
#include "score/os/time.h"

#include <gtest/gtest.h>

#if defined(__QNX__)
#include <sys/procmgr.h>
#endif

namespace score
{
namespace os
{
namespace
{

TEST(TimeImplTest, ClockSettimeFailure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Clock Settime Failure");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec new_time;
    ::clock_gettime(CLOCK_MONOTONIC, &new_time);

    new_time.tv_sec += (60 * 60) * 24L;
    new_time.tv_nsec = 0;

    const auto result = Time::instance().clock_settime(CLOCK_MONOTONIC, &new_time);
    // You can't set the time for CLOCK_MONOTONIC
    EXPECT_FALSE(result.has_value());

    struct timespec current_time;
    ::clock_gettime(CLOCK_MONOTONIC, &current_time);

    EXPECT_LT(current_time.tv_sec, new_time.tv_sec);
}

TEST(TimeImplTest, ClockGetTimeSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Clock Get Time Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec get_time{};

    const auto result = Time::instance().clock_gettime(CLOCK_REALTIME, &get_time);
    EXPECT_TRUE(result.has_value());

    struct timespec current_time;
    ::clock_gettime(CLOCK_REALTIME, &current_time);

    EXPECT_EQ(get_time.tv_sec, current_time.tv_sec);
}

TEST(TimeImplTest, GettimeFailsWithInvalidClockId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Gettime Fails With Invalid Clock Id");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec get_time{};
    constexpr auto kInvalidClockId{-1};

    const auto result = Time::instance().clock_gettime(kInvalidClockId, &get_time);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, ClockSettimeSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Clock Settime Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec new_time;
    ::clock_gettime(CLOCK_REALTIME, &new_time);

    new_time.tv_nsec += 500;

#if defined(__QNX__)
    ::procmgr_ability(0, PROCMGR_AID_CLOCKSET | PROCMGR_ADN_NONROOT, PROCMGR_AID_EOL);
#endif

    auto result = Time::instance().clock_settime(CLOCK_REALTIME, &new_time);
    EXPECT_TRUE(result.has_value());

    struct timespec current_time;
    ::clock_gettime(CLOCK_REALTIME, &current_time);

    EXPECT_GE(current_time.tv_sec, new_time.tv_sec);
}

TEST(TimeImplTest, ClockGetResSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Clock Get Res Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec get_res{};

    const auto result = Time::instance().clock_getres(CLOCK_REALTIME, &get_res);
    EXPECT_TRUE(result.has_value());

    struct timespec current_res;
    ::clock_getres(CLOCK_REALTIME, &current_res);

    EXPECT_EQ(get_res.tv_nsec, current_res.tv_nsec);
}

TEST(TimeImplTest, ClockGetResFailsWithInvalidClockId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Clock Get Res Fails With Invalid Clock Id");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec get_res{};
    constexpr auto kInvalidClockId{-1};

    const auto result = Time::instance().clock_getres(kInvalidClockId, &get_res);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, LocaltimeRSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Localtime RSuccess");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto current_time = std::chrono::system_clock::now();
    const std::time_t current_time_t{std::chrono::system_clock::to_time_t(current_time)};

    std::tm tm_local_time_test{};
    std::tm* const local_time_test = Time::instance().localtime_r(&current_time_t, &tm_local_time_test);

    ASSERT_TRUE(local_time_test != nullptr);

    std::tm tm_local_time{};
    std::tm* const local_time = ::localtime_r(&current_time_t, &tm_local_time);

    ASSERT_TRUE(local_time != nullptr);

    EXPECT_EQ(local_time->tm_sec, local_time_test->tm_sec);
    EXPECT_EQ(local_time->tm_min, local_time_test->tm_min);
    EXPECT_EQ(local_time->tm_hour, local_time_test->tm_hour);
    EXPECT_EQ(local_time->tm_mday, local_time_test->tm_mday);
    EXPECT_EQ(local_time->tm_mon, local_time_test->tm_mon);
    EXPECT_EQ(local_time->tm_year, local_time_test->tm_year);
    EXPECT_EQ(local_time->tm_wday, local_time_test->tm_wday);
    EXPECT_EQ(local_time->tm_yday, local_time_test->tm_yday);
    EXPECT_EQ(local_time->tm_isdst, local_time_test->tm_isdst);
}

TEST(TimeImplTest, TimerCreateAndDeleteSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies TimeImplTest Timer Create And Delete Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    EXPECT_TRUE(create_result.has_value());
    EXPECT_EQ(create_result.value(), 0);

    const auto delete_result = Time::instance().timer_delete(timerid);
    EXPECT_TRUE(delete_result.has_value());
    EXPECT_EQ(delete_result.value(), 0);
}

TEST(TimeImplTest, TimerCreateFailsWithInvalidClockId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Timer Create Fails With Invalid Clock Id");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;
    constexpr auto kInvalidClockId{-1};

    const auto result = Time::instance().timer_create(kInvalidClockId, &event, &timerid);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, TimerDeleteFailsWithInvalidTimerId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Timer Delete Fails With Invalid Timer Id");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // create an invalid timer_t by using a value that's unlikely to be valid
    timer_t invalid_timerid{};
    std::memset(&invalid_timerid, 0xFF, sizeof(timer_t));

    const auto result = Time::instance().timer_delete(invalid_timerid);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, TimerSettimeSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Timer Settime Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(create_result.has_value());

    itimerspec new_value{};
    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    itimerspec old_value{};
    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, &old_value);
    EXPECT_TRUE(settime_result.has_value());
    EXPECT_EQ(settime_result.value(), 0);

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, TimerSettimeFailsWithInvalidTimerId)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Timer Settime Fails With Invalid Timer Id");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t invalid_timerid{};
    std::memset(&invalid_timerid, 0xFF, sizeof(timer_t));

    itimerspec new_value{};
    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    const auto result = Time::instance().timer_settime(invalid_timerid, 0, &new_value, nullptr);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, ClockGetCpuClockIdSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Clock Get Cpu Clock Id Success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    clockid_t clock_id{};
    const pid_t current_pid = 1;

    const auto result = Time::instance().clock_getcpuclockid(current_pid, clock_id);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // verify that the obtained clock_id is valid by using it
    struct timespec ts{};
    const auto gettime_result = Time::instance().clock_gettime(clock_id, &ts);
    EXPECT_TRUE(gettime_result.has_value());
}

TEST(TimeImplTest, ClockGetCpuClockIdFailsWithInvalidPid)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies Clock Get Cpu Clock Id Failure with Invalid Pid");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    clockid_t clock_id{};
    const pid_t current_pid = -1;

    const auto result = Time::instance().clock_getcpuclockid(current_pid, clock_id);
    EXPECT_FALSE(result.has_value());
}

TEST(TimeImplTest, TimerCreateWithNullEvent)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the timer Creation With Null Event success");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};

    const auto result = Time::instance().timer_create(CLOCK_REALTIME, nullptr, &timerid);
    ASSERT_TRUE(result.has_value());

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, RelativeZeroTimerCreationSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the relative timer creation with zero it_value disarms the timer");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(create_result.has_value());

    // Setting zero value disarms the timer
    itimerspec new_value{};
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, nullptr);
    EXPECT_TRUE(settime_result.has_value());
    EXPECT_EQ(settime_result.value(), 0);

    // Verify timer is disarmed by getting its current value using native call
    itimerspec current_value{};
    const std::int32_t gettime_result = ::timer_gettime(timerid, &current_value);
    EXPECT_EQ(gettime_result, 0);
    EXPECT_EQ(current_value.it_value.tv_sec, 0);
    EXPECT_EQ(current_value.it_value.tv_nsec, 0);

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, PeriodicTimerCreationSuccess)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the periodic timer creation is successful");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;

    const auto result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(result.has_value());

    // Set periodic timer: initial expiration 50ms, interval 50ms
    itimerspec new_value{};
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 50000000;  // 50ms initial
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 50000000;  // 50ms periodic

    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, nullptr);
    ASSERT_TRUE(settime_result.has_value());

    // Wait for first expiration
    struct timespec sleep_time{};
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 51000000;      // 51ms
    ::nanosleep(&sleep_time, nullptr);  // Suspend a thread until a timeout of the specified time

    // Get timer value - should show the interval is still set
    itimerspec current_value{};
    const std::int32_t gettime_result = ::timer_gettime(timerid, &current_value);
    EXPECT_EQ(gettime_result, 0);

    // For periodic timer, interval should remain set
    EXPECT_EQ(current_value.it_interval.tv_sec, 0);
    EXPECT_EQ(current_value.it_interval.tv_nsec, 50000000);

    // Timer should be armed with remaining time less than interval
    EXPECT_GE(current_value.it_value.tv_nsec, 0);
    EXPECT_LT(current_value.it_value.tv_nsec, 50000000);

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, TimerExpiresAtCorrectTime)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TimeImplTest Timer Expires At Correct Time");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_NONE;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(create_result.has_value());

    // Set timer to expire in 100 milliseconds
    itimerspec new_value{};
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 100000000;  // 100ms
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    struct timespec start_time{};
    std::ignore = ::clock_gettime(CLOCK_REALTIME, &start_time);

    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, nullptr);
    ASSERT_TRUE(settime_result.has_value());
    EXPECT_EQ(settime_result.value(), 0);

    // Wait for timer to expire (sleep for 110ms to ensure it expired)
    struct timespec sleep_time{};
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 110000000;  // 110ms
    ::nanosleep(&sleep_time, nullptr);

    // Check that timer has expired by getting remaining time
    itimerspec current_value{};
    const std::int32_t gettime_result = ::timer_gettime(timerid, &current_value);
    EXPECT_EQ(gettime_result, 0);

    // Timer should be disarmed (value should be 0)
    EXPECT_EQ(current_value.it_value.tv_sec, 0);
    EXPECT_EQ(current_value.it_value.tv_nsec, 0);

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, TimerWithSigEvThreadNotificationIsSuccessful)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the timer Creation With SIGEV_THREAD notification is successful");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = [](union sigval) { /* dummy callback */ };
    event.sigev_notify_attributes = nullptr;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(create_result.has_value());

    // Set timer value
    itimerspec new_value{};
    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, nullptr);
    EXPECT_TRUE(settime_result.has_value());
    EXPECT_EQ(settime_result.value(), 0);

    Time::instance().timer_delete(timerid);
}

TEST(TimeImplTest, TimerWithSigEvSignalNotificationIsSuccessful)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the timer Creation With SIGEV_SIGNAL notification is successful");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    timer_t timerid{};
    sigevent event{};
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGUSR1;

    const auto create_result = Time::instance().timer_create(CLOCK_REALTIME, &event, &timerid);
    ASSERT_TRUE(create_result.has_value());

    // Set timer to expire in 50ms
    itimerspec new_value{};
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_nsec = 50000000;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    const auto settime_result = Time::instance().timer_settime(timerid, 0, &new_value, nullptr);
    EXPECT_TRUE(settime_result.has_value());

    // Set up to wait for the signal
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    timespec timeout{0, 100000000};  // timeout is 100ms
    // Wait for signal with timeout
    const int signal_received = sigtimedwait(&mask, nullptr, &timeout);
    // Verify that we received the expected signal
    EXPECT_EQ(signal_received, SIGUSR1);

    Time::instance().timer_delete(timerid);
}

}  // namespace
}  // namespace os
}  // namespace score
