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
#include "score/os/sched_impl.h"

#include <gtest/gtest.h>
#include <climits>

namespace score
{
namespace os
{

namespace
{

constexpr pid_t kInvalidPid{INT_MAX};
constexpr std::int32_t kPolicy{SCHED_FIFO};
constexpr std::int32_t kInvalidPolicy{INT_MAX};
constexpr pid_t kCurrentPid{0};

struct SchedImplTest : ::testing::Test
{
    score::os::SchedImpl sched_;
};

TEST_F(SchedImplTest, sched_setparam_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_setparam_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sched_param set_params;
    set_params.sched_priority = 2;
    ASSERT_TRUE(sched_.sched_setparam(kCurrentPid, &set_params).has_value());

    struct sched_param get_params;
    ASSERT_EQ(::sched_getparam(kCurrentPid, &get_params), 0);
    EXPECT_EQ(get_params.sched_priority, set_params.sched_priority);
}

TEST_F(SchedImplTest, sched_getparam_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_getparam_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sched_param set_params;
    set_params.sched_priority = 2;
    ASSERT_EQ(::sched_setparam(kCurrentPid, &set_params), 0);

    struct sched_param get_params;
    ASSERT_TRUE(sched_.sched_getparam(kCurrentPid, &get_params).has_value());
    EXPECT_EQ(get_params.sched_priority, set_params.sched_priority);
}

TEST_F(SchedImplTest, sched_getparam_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_getparam_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_getparam(kInvalidPid, nullptr).has_value());
}

TEST_F(SchedImplTest, sched_setscheduler_failure)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_setscheduler_failure");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sched_param param;
    param.sched_priority = 50;

    EXPECT_FALSE(sched_.sched_setscheduler(kInvalidPid, kPolicy, &param).has_value());
}

TEST_F(SchedImplTest, sched_setscheduler_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_setscheduler_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sched_param params;
    params.sched_priority = 4;
    ASSERT_TRUE(sched_.sched_setscheduler(kCurrentPid, kPolicy, &params).has_value());

    const auto sched_policy = ::sched_getscheduler(kCurrentPid);
    ASSERT_NE(sched_policy, -1);
    EXPECT_EQ(sched_policy, kPolicy);
}

TEST_F(SchedImplTest, sched_getscheduler_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_getscheduler_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct sched_param params;
    params.sched_priority = 4;
    ASSERT_EQ(::sched_setscheduler(kCurrentPid, kPolicy, &params), 0);

    const auto sched_policy = sched_.sched_getscheduler(kCurrentPid);
    ASSERT_TRUE(sched_policy.has_value());
    EXPECT_EQ(sched_policy.value(), kPolicy);
}

TEST_F(SchedImplTest, sched_getscheduler_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_getscheduler_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_getscheduler(kInvalidPid).has_value());
}

TEST_F(SchedImplTest, sched_setparam_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_setparam_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_setparam(kInvalidPid, nullptr).has_value());
}

TEST_F(SchedImplTest, sched_yield_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_yield_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_TRUE(sched_.sched_yield().has_value());
}

TEST_F(SchedImplTest, sched_rr_get_interval_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_rr_get_interval_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    struct timespec ts;
    const auto result = sched_.sched_rr_get_interval(kCurrentPid, &ts);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
    EXPECT_GT(ts.tv_nsec, 0);
}

TEST_F(SchedImplTest, sched_rr_get_interval_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_rr_get_interval_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_rr_get_interval(kInvalidPid, nullptr).has_value());
}

TEST_F(SchedImplTest, sched_get_priority_max_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_get_priority_max_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto prio_max = sched_.sched_get_priority_max(kPolicy);
    ASSERT_TRUE(prio_max.has_value());
    EXPECT_LT(prio_max.value(), std::numeric_limits<std::int32_t>::max());

    const auto sched_max_prio = ::sched_get_priority_max(kPolicy);
    ASSERT_NE(sched_max_prio, -1);
    ASSERT_EQ(prio_max.value(), sched_max_prio);
}

TEST_F(SchedImplTest, sched_get_priority_min_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_get_priority_min_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    const auto prio_min = sched_.sched_get_priority_min(kPolicy);
    ASSERT_TRUE(prio_min.has_value());
    EXPECT_GE(prio_min.value(), 0);

    const auto sched_min_prio = ::sched_get_priority_min(kPolicy);
    ASSERT_NE(sched_min_prio, -1);
    EXPECT_EQ(prio_min.value(), sched_min_prio);
}

TEST_F(SchedImplTest, sched_get_priority_min_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_get_priority_min_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_get_priority_min(kInvalidPolicy).has_value());
}

TEST_F(SchedImplTest, sched_get_priority_max_fails)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_get_priority_max_fails");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    ASSERT_FALSE(sched_.sched_get_priority_max(kInvalidPolicy).has_value());
}

TEST(SchedTest, CanGetInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedTest Can Get Instance");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    EXPECT_NO_FATAL_FAILURE(Sched::instance());
}

#if defined(__QNX__)
TEST_F(SchedImplTest, sched_get_priority_adjust_success)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "SchedImplTest sched_get_priority_adjust_success");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr std::int32_t priority = 3;
    constexpr std::int32_t adjust = 4;

    const auto result = sched_.sched_get_priority_adjust(priority, kPolicy, adjust);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 7);
}

#endif
}  // namespace
}  // namespace os
}  // namespace score
