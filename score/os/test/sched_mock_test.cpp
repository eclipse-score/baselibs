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
#include "score/os/mocklib/sched_mock.h"

namespace score
{
namespace os
{
namespace
{

constexpr std::int32_t kPosixOk = 0;
constexpr std::int32_t kSchedulerPolicy = SCHED_RR;
constexpr pid_t kValidPid{};

// @class SchedMockFixture
// @brief The SchedMockFixture class provides the common auxiliary data and functionality to test the Scheduling API
// mock library
class SchedMockFixture : public testing::Test
{
  public:
    // @brief Sets up the test API instance
    void SetUp() override
    {
        score::os::Sched::set_testing_instance(test_instance_);
    }

    // @brief Restores the default API instance
    void TearDown() override
    {
        score::os::Sched::restore_instance();
    }

    // The mock version API to test
    SchedMock test_instance_;
};

TEST_F(SchedMockFixture, Sched_Getparam_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_getparam(testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_getparam(kValidPid, nullptr).has_value());
}

TEST_F(SchedMockFixture, Sched_Getscheduler_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_getscheduler(testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_getscheduler(kValidPid).has_value());
}

TEST_F(SchedMockFixture, Sched_Setparam_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_setparam(testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_setparam(kValidPid, nullptr).has_value());
}

TEST_F(SchedMockFixture, Sched_Setscheduler_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_setscheduler(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_setscheduler(kValidPid, kSchedulerPolicy, nullptr).has_value());
}

TEST_F(SchedMockFixture, Sched_Yield_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_yield()).WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_yield().has_value());
}

TEST_F(SchedMockFixture, Sched_Rr_Get_Interval_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_rr_get_interval(testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_rr_get_interval(kValidPid, nullptr).has_value());
}

TEST_F(SchedMockFixture, Sched_Get_Priority_Min_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_get_priority_min(testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_get_priority_min(kSchedulerPolicy).has_value());
}

TEST_F(SchedMockFixture, Sched_Get_Priority_Max_Call_Expected)
{
    EXPECT_CALL(test_instance_, sched_get_priority_max(testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_get_priority_max(kSchedulerPolicy).has_value());
}

#if defined(__QNX__)
TEST_F(SchedMockFixture, Sched_Get_Priority_Adjust_Call_Expected)
{
    constexpr std::int32_t priority = 3;
    constexpr std::int32_t adjust = 4;

    EXPECT_CALL(test_instance_, sched_get_priority_adjust(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(score::cpp::expected<std::int32_t, Error>(kPosixOk)));
    EXPECT_TRUE(score::os::Sched::instance().sched_get_priority_adjust(priority, kSchedulerPolicy, adjust).has_value());
}
#endif  //__QNX__

}  // namespace
}  // namespace os
}  // namespace score
