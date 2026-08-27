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
#include "score/concurrency/delayed_task.h"

#include "score/concurrency/clock.h"
#include "score/concurrency/interruptible_conditional_variable_mock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdint>
#include <future>
#include <memory>
#include <utility>

namespace score
{
namespace concurrency
{
namespace
{

class Invokable
{
  public:
    MOCK_METHOD(void,
                Invoke,
                (score::cpp::stop_token, const score::concurrency::testing::SteadyClock::time_point),
                (const));
    MOCK_METHOD(void,
                Invoke,
                (score::cpp::stop_token, const score::concurrency::testing::SteadyClock::time_point, int, double),
                (const));
};

using InvokableMock = Invokable;

class DelayedTaskTest : public ::testing::Test
{
  protected:
    Invokable invokable_{};
    score::concurrency::testing::SteadyClock::time_point execution_time_point_{
        score::concurrency::testing::SteadyClock::now()};
    std::promise<void> promise_{};
    score::cpp::stop_source stop_source_{};
};

TEST_F(DelayedTaskTest, ConstructionAndDestruction)
{
    auto unique_task = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    unique_task.reset();
}

TEST_F(DelayedTaskTest, ConstructionAndDestructionOnHeapWithBasePointer)
{
    score::cpp::pmr::unique_ptr<Task> unique_task = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    unique_task.reset();
}

TEST_F(DelayedTaskTest, ExecutesCallbackWithCorrectAttributes)
{
    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_));

    (*unit)(stop_source_.get_token());
}

TEST_F(DelayedTaskTest, WaitsSpecifiedAmountOfTimeForInvocation)
{
    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token token,
               const score::concurrency::testing::SteadyClock::time_point time_point) -> void {
            invokable_.Invoke(std::move(token), time_point);
        });

    testing::InterruptibleConditionalVariableMock mock_cv{};

    ::testing::InSequence seq{};
    EXPECT_CALL(mock_cv, wait_until(::testing::_, stop_source_.get_token(), execution_time_point_, ::testing::_));
    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_));

    (*unit)(stop_source_.get_token(), mock_cv);
}

TEST_F(DelayedTaskTest, ExecutesCallbackWithParameter)
{
    std::int32_t observer{0};
    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [&observer](score::cpp::stop_token,
                    const score::concurrency::testing::SteadyClock::time_point,
                    const std::int32_t a) noexcept -> void {
            observer = a;
        },
        42);

    (*unit)(stop_source_.get_token());

    EXPECT_EQ(observer, 42);
}

TEST_F(DelayedTaskTest, MakeWithTaskResultLeadsToAssociatedTaskAndTaskResult)
{
    // Given a task created using the make_* function
    auto unit = DelayedTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [](score::cpp::stop_token, const score::concurrency::testing::SteadyClock::time_point) noexcept {});

    // When executing the function call operator
    unit.second->GetStopSource().request_stop();
    (*unit.second)(unit.second->GetStopSource().get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

TEST_F(DelayedTaskTest, MakeWithTaskResultCapturesAllArguments)
{
    int arg1 = 42;
    double arg2 = 3.14;

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_, arg1, arg2)).Times(1);

    auto unit = DelayedTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token token,
               const score::concurrency::testing::SteadyClock::time_point time,
               int param1,
               double param2) {
            invokable_.Invoke(token, time, param1, param2);
        },
        arg1,
        arg2);

    // When executing the function call operator
    stop_source_.request_stop();
    (*unit.second)(stop_source_.get_token());

    // Then the future is valid and the task completes successfully
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

}  // namespace
}  // namespace concurrency
}  // namespace score
