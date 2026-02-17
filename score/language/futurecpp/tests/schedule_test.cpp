/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/execution.hpp>
#include <score/execution.hpp> //  // test include guard

#include <cstdint>
#include <utility>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace
{

struct scheduler_sender
{
    using sender_concept = sender_t;

    std::int32_t v;
};

struct scheduler
{
    using scheduler_concept = scheduler_t;

    scheduler_sender schedule() & { return {v}; }
    scheduler_sender schedule() && = delete;
    scheduler_sender schedule() const& = delete;
    scheduler_sender schedule() const&& = delete;

    std::int32_t v;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenRValueSender_ExpectSchedulerSender)
{
    scheduler sch{42};
    const scheduler_sender s{schedule(std::move(sch))};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenLValueSender_ExpectSchedulerSender)
{
    scheduler sch{42};
    const scheduler_sender s{schedule(sch)};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenConstRValueSender_ExpectSchedulerSender)
{
    const scheduler sch{42};
    const scheduler_sender s{schedule(std::move(sch))};

    EXPECT_EQ(42, s.v);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#40946837
TEST(schedule_test, schedule_GivenConstLValueSender_ExpectSchedulerSender)
{
    const scheduler sch{42};
    const scheduler_sender s{schedule(sch)};

    EXPECT_EQ(42, s.v);
}

} // namespace
} // namespace execution
} // namespace score::cpp
