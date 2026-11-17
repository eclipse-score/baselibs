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
#include "score/concurrency/timed_executor/periodic_task.h"

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

class PeriodicTaskTest : public ::testing::Test
{
  protected:
    score::cpp::pmr::unique_ptr<TimedTask<score::concurrency::testing::SteadyClock>> WithTimedTaskedReturningVoid()
    {
        return PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
            score::cpp::pmr::get_default_resource(),
            first_execution_,
            interval_,
            [this](score::cpp::stop_token stop_token,
                   const score::concurrency::testing::SteadyClock::time_point time_point) -> void {
                invokable_.Invoke(std::move(stop_token), time_point);
            });
    }

    std::pair<TaskResult<void>, score::cpp::pmr::unique_ptr<TimedTask<score::concurrency::testing::SteadyClock>>>
    WithTimedTaskedReturningVoidResult()
    {
        return PeriodicTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
            score::cpp::pmr::get_default_resource(),
            first_execution_,
            interval_,
            [this](score::cpp::stop_token stop_token,
                   const score::concurrency::testing::SteadyClock::time_point time_point) -> void {
                invokable_.Invoke(std::move(stop_token), time_point);
            });
    }

    score::cpp::pmr::unique_ptr<TimedTask<score::concurrency::testing::SteadyClock>> WithTimedTaskedReturningNonVoid()
    {
        return PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
            score::cpp::pmr::get_default_resource(),
            first_execution_,
            interval_,
            [this](score::cpp::stop_token stop_token,
                   const score::concurrency::testing::SteadyClock::time_point time_point) -> std::uint32_t {
                invokable_.Invoke(std::move(stop_token), time_point);
                return 2;
            });
    }

    Invokable invokable_{};
    score::concurrency::testing::SteadyClock::time_point first_execution_{
        score::concurrency::testing::SteadyClock::now()};
    score::concurrency::testing::SteadyClock::duration interval_{std::chrono::milliseconds{100}};
    std::promise<void> promise_{};
    score::cpp::stop_source stop_source_{};
};

TEST_F(PeriodicTaskTest, ConstructionAndDestruction)
{
    auto unique_task = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept {});
    unique_task.reset();
}

TEST_F(PeriodicTaskTest, ConstructionAndDestructionOnHeapWithBasePointer)
{
    score::cpp::pmr::unique_ptr<Task> unique_task = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept {});
    unique_task.reset();
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningVoid)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask with a void callback invokes it at the first execution time.");

    // Given a PeriodicTask that returns void
    auto unit = WithTimedTaskedReturningVoid();

    // Expecting that the underlying invocable gets invoked
    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([this]() {
        score::concurrency::testing::SteadyClock::modify_time(interval_);
    }));

    // When invoking the task
    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningVoidChangesNextExecutionTimepoint)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask with a void callback moves its next execution point by one "
                   "interval after execution.");

    // Given a PeriodicTask that returns void
    auto unit = WithTimedTaskedReturningVoid();

    // When invoking the task
    (*unit)(unit->GetStopSource().get_token());

    // Then the execution time point is moved
    ASSERT_TRUE(unit->GetNextExecutionPoint().has_value());
    EXPECT_EQ(unit->GetNextExecutionPoint().value(), first_execution_ + interval_);
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningNonVoidChangesNextExecutionTimepoint)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask with a non-void callback moves its next execution point by one "
                   "interval after execution.");

    // Given a PeriodicTask that returns void
    auto unit = WithTimedTaskedReturningNonVoid();

    // When invoking the task
    (*unit)(unit->GetStopSource().get_token());

    // Then the execution time point is moved
    ASSERT_TRUE(unit->GetNextExecutionPoint().has_value());
    EXPECT_EQ(unit->GetNextExecutionPoint().value(), first_execution_ + interval_);
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningNonVoid)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask with a non-void callback invokes it at the first execution "
                   "time.");

    // Given a PeriodicTask that returns not void
    auto unit = WithTimedTaskedReturningNonVoid();

    // Expecting that the underlying callable is invoked
    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([this]() {
        score::concurrency::testing::SteadyClock::modify_time(interval_);
    }));

    // When invoking the task
    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, DoesNotReturnNextExectuionPointWhenStopped)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask reports no next execution point after its stop was requested.");

    // Given a PeriodicTask that returns void, which was requested to stop
    auto unit = WithTimedTaskedReturningVoid();
    unit->GetStopSource().request_stop();

    // When requesting the next execution point
    const auto next_execution_point = unit->GetNextExecutionPoint();

    // Then the next execution point should not exist
    EXPECT_FALSE(next_execution_point.has_value());
}

TEST_F(PeriodicTaskTest, StopingTaskSetsPromise)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__task_result");
    RecordProperty("Description",
                   "Check that the result of a stopped timed PeriodicTask becomes available after it was invoked.");

    // Given a PeriodicTask that returns void, which was requested to stop
    auto task_pair = WithTimedTaskedReturningVoidResult();
    auto unit = std::move(task_pair.second);
    auto future = std::move(task_pair.first);
    unit->GetStopSource().request_stop();

    // When invoking the task
    (*unit)(unit->GetStopSource().get_token());

    // Then the promise should be set
    auto result = future.Get();
    ASSERT_TRUE(result.has_value());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackWithCorrectAttributes)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask invokes its callback with the configured first execution time "
                   "point.");

    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([&unit]() {
        unit->GetStopSource().request_stop();
    }));

    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackWithParameter)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description", "Check that a timed PeriodicTask invokes its callback with the bound argument.");

    std::int32_t observer{0};
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this, &observer](score::cpp::stop_token stop_token,
                          const score::concurrency::testing::SteadyClock::time_point time_point,
                          const std::int32_t a) -> void {
            observer = a;
            invokable_.Invoke(std::move(stop_token), time_point);
        },
        42);

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([&unit]() {
        unit->GetStopSource().request_stop();
    }));

    (*unit)(unit->GetStopSource().get_token());

    EXPECT_EQ(observer, 42);
}

TEST_F(PeriodicTaskTest, MakeWithTaskResultLeadsToAssociatedTaskAndTaskResult)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__task_result");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask created with a task result provides a valid result that "
                   "completes after the task ended.");

    // Given a task created using the make_* function
    auto unit = PeriodicTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept -> void {
        });

    // When executing the function call operator
    unit.second->GetStopSource().request_stop();
    (*unit.second)(unit.second->GetStopSource().get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

TEST_F(PeriodicTaskTest, WillTerminateWhenCalculationOfTimePointForNextExecutionWouldOverflow)
{
    first_execution_ = score::concurrency::testing::SteadyClock::time_point::max() - std::chrono::milliseconds{1};
    score::concurrency::testing::SteadyClock::modify_time(first_execution_ -
                                                          score::concurrency::testing::SteadyClock::now());
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept -> void {
        });

    EXPECT_EXIT((*unit)(unit->GetStopSource().get_token()), ::testing::KilledBySignal(SIGABRT), "");
}

TEST_F(PeriodicTaskTest, MakeWithTaskResultCapturesAllArguments)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__periodic_task");
    RecordProperty("Description",
                   "Check that a timed PeriodicTask created with a task result invokes its callback with all bound "
                   "arguments.");

    int arg1 = 42;
    double arg2 = 3.14;

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), first_execution_, arg1, arg2)).Times(1);

    auto unit = PeriodicTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](const score::cpp::stop_token stop,
               const score::concurrency::testing::SteadyClock::time_point time,
               int param1,
               double param2) {
            invokable_.Invoke(stop, time, param1, param2);
            return false;
        },
        arg1,
        arg2);

    // When executing the function call operator
    (*unit.second)(stop_source_.get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

}  // namespace
}  // namespace concurrency
}  // namespace score
