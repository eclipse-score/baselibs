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
#include "score/concurrency/clock.h"

#include "score/concurrency/future/interruptible_promise.h"

#include <score/stop_token.hpp>

#include <gtest/gtest.h>

#include <future>

namespace score
{
namespace concurrency
{
namespace
{

template <typename Clock>
class CommonClockTest : public ::testing::Test
{
};

using MyTypes = ::testing::Types<score::concurrency::testing::SteadyClock, score::concurrency::testing::NonSteadyClock>;
TYPED_TEST_SUITE(CommonClockTest, MyTypes, /*unused*/);

TYPED_TEST(CommonClockTest, CanGetTime)
{
    EXPECT_NO_FATAL_FAILURE(TypeParam::now());
}

TYPED_TEST(CommonClockTest, CanTurnForwardTime)
{
    constexpr auto offset = typename TypeParam::duration{10};
    typename TypeParam::time_point current_time = TypeParam::now();
    TypeParam::modify_time(offset);
    EXPECT_EQ(TypeParam::now(), current_time + offset);
}

// FIXME: Investigation of bug in standard library required (Ticket-47423)
TYPED_TEST(CommonClockTest, DISABLED_CanUseWithStdLib)
{
    typename TypeParam::duration offset{1};

    auto future = std::async([offset]() {
        std::promise<void> promise;
        typename TypeParam::time_point time_point = TypeParam::now() + offset;
        promise.get_future().wait_until(time_point);
    });

    EXPECT_EQ(future.wait_for(std::chrono::milliseconds{50}), std::future_status::timeout);
    TypeParam::modify_time(offset);
    future.wait();
}

TYPED_TEST(CommonClockTest, CanUseWithInterruptiblePromise)
{
    typename TypeParam::duration offset{1};
    InterruptiblePromise<void> promise{};
    InterruptibleSharedFuture<void> future{promise.GetInterruptibleFuture().value().Share()};
    score::cpp::stop_source stop_source{};

    // Take the deadline before the waiter starts. Taken inside it, a waiter that only starts after modify_time() below
    // would wait for a time the clock never reaches, since nothing advances it again.
    const typename TypeParam::time_point time_point = TypeParam::now() + offset;
    auto async_future = std::async([time_point, future, stop_source]() {
        future.WaitUntil(stop_source.get_token(), time_point);
    });

    EXPECT_FALSE(future.WaitFor(stop_source.get_token(), std::chrono::milliseconds{50}).has_value());
    TypeParam::modify_time(offset);
    async_future.wait();
}

TEST(SteadyClockTest, IsSteady)
{
    EXPECT_EQ(score::concurrency::testing::SteadyClock::is_steady, true);
}

TEST(SteadyClockTest, CanNotTurnBackTime)
{
    constexpr auto offset = std::chrono::milliseconds{-10};
    EXPECT_EXIT(score::concurrency::testing::SteadyClock::modify_time(offset), ::testing::KilledBySignal(SIGABRT), "");
}

TEST(NonSteadyClockTest, IsNotSteady)
{
    EXPECT_EQ(score::concurrency::testing::NonSteadyClock::is_steady, false);
}

TEST(NonSteadyClockTest, CanTurnBackTime)
{
    constexpr auto offset = std::chrono::milliseconds{10};
    score::concurrency::testing::NonSteadyClock::time_point current_time =
        score::concurrency::testing::NonSteadyClock::now();
    score::concurrency::testing::NonSteadyClock::modify_time(-offset);
    EXPECT_EQ(score::concurrency::testing::NonSteadyClock::now(), current_time - offset);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
