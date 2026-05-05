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
#include "score/concurrency/task_result.h"
#include "score/concurrency/shared_task_result.h"

#include "score/concurrency/executor_mock.h"
#include "score/concurrency/simple_task.h"

#include "gtest/gtest.h"

namespace score
{
namespace concurrency
{
namespace
{

class TaskResultTest : public ::testing::Test
{
  protected:
    testing::ExecutorMock executor_;
};

TEST_F(TaskResultTest, CanWaitForVoidReturn)
{
    // Given a task that is executed
    bool executed{false};
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(),
                                                      [&executed](const score::cpp::stop_token&) noexcept {
                                                          executed = true;
                                                      });
    (*(task.second))(score::cpp::stop_token{});

    // When waiting for its results
    EXPECT_TRUE(task.first.Get());

    // We continue after it is executed
    EXPECT_TRUE(executed);
}

TEST_F(TaskResultTest, CanRetrieveReturnType)
{
    // Given a task that is executed
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {
        return 42;
    });
    (*(task.second))(score::cpp::stop_token{});

    // When waiting for its result
    const auto result = task.first.Get().value();

    // Then the result is the expected value
    EXPECT_EQ(result, 42);
}

TEST_F(TaskResultTest, CanAbortTask)
{
    // Given a task that is executed
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {});
    (*(task.second))(task.second->GetStopSource().get_token());

    // When trying to abort the task
    task.first.Abort();

    // Then the respective stop_token is set
    EXPECT_TRUE(task.second->GetStopSource().stop_requested());
}

TEST_F(TaskResultTest, ReportsAbortedState)
{
    // Given a task that is executed and _not_ yet aborted
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {});
    (*(task.second))(task.second->GetStopSource().get_token());
    ASSERT_FALSE(task.first.Aborted());

    // When trying to abort the task
    task.first.Abort();

    // Then the abortion is reported
    EXPECT_TRUE(task.first.Aborted());
}

TEST_F(TaskResultTest, SharedTaskResultCanBeCopied)
{
    // Given a task that is executed
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {
        return 42;
    });
    (*(task.second))(score::cpp::stop_token{});

    SharedTaskResult<int> shared_task = task.first.Share();
    auto copied_task = shared_task;

    // Check if the shared state is available
    ASSERT_TRUE(shared_task.Valid());
    ASSERT_TRUE(copied_task.Valid());

    auto value = shared_task.Get().value();
    auto copied_task_value = copied_task.Get().value();

    ASSERT_EQ(value, 42);
    ASSERT_EQ(copied_task_value, 42);
}

TEST_F(TaskResultTest, TaskResultIsInvalidAfterBeingShared)
{
    auto task = SimpleTaskFactory::MakeWithTaskResult(score::cpp::pmr::get_default_resource(), [](const score::cpp::stop_token&) {
        return 42;
    });
    (*(task.second))(score::cpp::stop_token{});
    auto shared_task = task.first.Share();

    ASSERT_FALSE(task.first.Valid());
    ASSERT_TRUE(shared_task.Valid());
}

class TaskResultType
{
  public:
    using TaskType = TaskResult<int>;
    static TaskResult<int> Make(concurrency::InterruptiblePromise<int>& promise)
    {
        concurrency::InterruptibleFuture<int> future = promise.GetInterruptibleFuture().value();
        score::cpp::stop_source stop_source;

        return TaskResult<int>(std::move(future), stop_source);
    }
};

class SharedTaskResultType
{
  public:
    using TaskType = SharedTaskResult<int>;
    static SharedTaskResult<int> Make(concurrency::InterruptiblePromise<int>& promise)
    {
        concurrency::InterruptibleFuture<int> future = promise.GetInterruptibleFuture().value();
        score::cpp::stop_source stop_source;
        TaskResult<int> task_result(std::move(future), stop_source);
        SharedTaskResult<int> shared_task_result = task_result.Share();
        return shared_task_result;
    }
};

template <class T>
class TaskResultContinuationTest : public ::testing::Test
{
  public:
    typename T::TaskType Make()
    {
        return T::Make(promise_);
    }

    void ExpectCallNTimes(std::uint16_t n)
    {
        EXPECT_EQ(this->invoked_, n);
    }

    void PrepareForScopedContinuation(typename T::TaskType& task_result)
    {
        task_result.Then(
            safecpp::MoveOnlyScopedFunction<void(score::Result<int>&)>{scope_, [this](score::Result<int>&) noexcept {
                                                                         this->invoked_++;
                                                                     }});
    }

  protected:
    concurrency::InterruptiblePromise<int> promise_;

  private:
    std::uint32_t invoked_{0};
    safecpp::Scope<> scope_;
};

typedef ::testing::Types<TaskResultType, SharedTaskResultType> TestTypes;

TYPED_TEST_SUITE(TaskResultContinuationTest, TestTypes, /* unused */);

TYPED_TEST(TaskResultContinuationTest, TaskResultScopedContinuationExecutedWhenStateIsReady)
{
    auto task_result = this->Make();
    this->PrepareForScopedContinuation(task_result);
    this->promise_.SetValue(42);
    this->ExpectCallNTimes(1);
}

TYPED_TEST(TaskResultContinuationTest, TaskResultScopedContinuationCanBeExecutedMultipleTimes)
{
    auto task_result = this->Make();
    this->PrepareForScopedContinuation(task_result);
    this->PrepareForScopedContinuation(task_result);
    this->promise_.SetValue(42);
    this->ExpectCallNTimes(2);
}

TYPED_TEST(TaskResultContinuationTest, TaskResultScopedContinuationIsExecutedWhenResultIsError)
{
    auto task_result = this->Make();
    this->PrepareForScopedContinuation(task_result);
    this->promise_.SetError(Error::kNoState);
    this->ExpectCallNTimes(1);
}

TYPED_TEST(TaskResultContinuationTest, TaskResultScopedContinuationIsNotExecutedWhenNoResultIsReady)
{
    auto task_result = this->Make();
    this->PrepareForScopedContinuation(task_result);
    this->ExpectCallNTimes(0);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
