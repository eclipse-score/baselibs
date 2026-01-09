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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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
    RecordProperty("TestType", "interface-test");
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

}  // namespace
}  // namespace os
}  // namespace score
