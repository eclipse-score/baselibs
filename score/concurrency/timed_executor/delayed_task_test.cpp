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
#include "score/concurrency/timed_executor/delayed_task.h"

#include "score/concurrency/clock.h"

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
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
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
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    unique_task.reset();
}

TEST_F(DelayedTaskTest, ExecutesCallbackWithCorrectAttributes)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask invokes its callback with the stop token and the configured "
                   "execution time point.");

    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_));

    (*unit)(stop_source_.get_token());
}

TEST_F(DelayedTaskTest, UnderlyingTaskIsNotExecutedOnRequestedStop)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask whose stop was requested before execution does not invoke its "
                   "callback.");

    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    unit->GetStopSource().request_stop();

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_)).Times(0);

    (*unit)(stop_source_.get_token());
}

TEST_F(DelayedTaskTest, RetreivesCorrectExectuionTimePoint)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask reports the configured execution time point as next execution "
                   "point.");

    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    const auto next_execution_point = unit->GetNextExecutionPoint();

    ASSERT_TRUE(next_execution_point.has_value());
    EXPECT_EQ(next_execution_point.value(), execution_time_point_);
}

TEST_F(DelayedTaskTest, RetreivesNoExectuionTimePointWhenStopped)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask reports no next execution point after its stop was requested.");

    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    unit->GetStopSource().request_stop();

    const auto next_execution_point = unit->GetNextExecutionPoint();

    EXPECT_FALSE(next_execution_point.has_value());
}

TEST_F(DelayedTaskTest, RetreivesNoExectuionWhenExecuted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask reports no next execution point after it was executed once.");

    auto unit = DelayedTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) noexcept {
            invokable_.Invoke(std::move(stop_token), time_point);
        });
    (*unit)(stop_source_.get_token());

    const auto next_execution_point = unit->GetNextExecutionPoint();

    EXPECT_FALSE(next_execution_point.has_value());
}

TEST_F(DelayedTaskTest, ExecutesCallbackWithParameter)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description", "Check that a timed DelayedTask invokes its callback with the bound argument.");

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
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__task_result");
    RecordProperty("Description",
                   "Check that the result of an executed timed DelayedTask is accessible through its TaskResult.");

    // Given a task created using the make_* function
    auto unit = DelayedTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [](score::cpp::stop_token,
           const score::concurrency::testing::SteadyClock::time_point) noexcept -> std::uint8_t {
            return 42;
        });

    // When executing the function call operator
    (*unit.second)(unit.second->GetStopSource().get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_EQ(42, unit.first.Get().value());
}

TEST_F(DelayedTaskTest, MakeWithTaskResultCapturesAllArguments)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__delayed_task");
    RecordProperty("Description",
                   "Check that a timed DelayedTask created with a task result invokes its callback with all bound "
                   "arguments.");

    int arg1 = 42;
    double arg2 = 3.14;

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), execution_time_point_, arg1, arg2)).Times(1);

    auto unit = DelayedTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        execution_time_point_,
        [this](score::cpp::stop_token token,
               const score::concurrency::testing::SteadyClock::time_point time,
               int param1,
               double param2) noexcept {
            invokable_.Invoke(token, time, param1, param2);
        },
        arg1,
        arg2);

    (*unit.second)(stop_source_.get_token());

    // Then the future is valid and the task completes successfully
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

}  // namespace
}  // namespace concurrency
}  // namespace score
