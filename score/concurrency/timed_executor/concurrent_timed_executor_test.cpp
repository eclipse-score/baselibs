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
#include "score/concurrency/timed_executor/concurrent_timed_executor.h"

#include "score/concurrency/clock.h"
#include "score/concurrency/executor_mock.h"
#include "score/concurrency/thread_pool.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <score/memory.hpp>

namespace score::concurrency
{

class ConcurrentTimedExecutorFixture : public ::testing::Test
{
  public:
    ConcurrentTimedExecutor<testing::SteadyClock> CreateConcurrentTimedExecutor()
    {
        auto executor_mock =
            score::cpp::pmr::make_unique<testing::ExecutorMock>(score::cpp::pmr::new_delete_resource());
        executor_mock_ = executor_mock.get();

        return ConcurrentTimedExecutor<testing::SteadyClock>{score::cpp::pmr::get_default_resource(),
                                                             std::move(executor_mock_stealed_)};
    }

    void WithThreads(const std::size_t number_of_threads)
    {
        if (executor_mock_ == nullptr)
        {
            assert(executor_mock_stealed_ != nullptr);
            executor_mock_ = executor_mock_stealed_.get();
        }
        ON_CALL(*executor_mock_, MaxConcurrencyLevel).WillByDefault(::testing::Return(number_of_threads));
        EXPECT_CALL(*executor_mock_, Enqueue(::testing::_)).Times(static_cast<int>(number_of_threads));
    }

    ConcurrentTimedExecutor<testing::SteadyClock> CreateConcurrentTimedExecutorWithTwoThreads()
    {
        WithThreads(2);
        return CreateConcurrentTimedExecutor();
    }

    ConcurrentTimedExecutor<std::chrono::steady_clock> CreateConcurrentTimedExecutorWithRealThreadPool()
    {
        return ConcurrentTimedExecutor<std::chrono::steady_clock>{
            score::cpp::pmr::get_default_resource(),
            score::cpp::pmr::make_unique<ThreadPool>(score::cpp::pmr::new_delete_resource(), 1)};
    }

    score::cpp::pmr::unique_ptr<TimedTask<testing::SteadyClock>> CreateDelayedTask(const std::size_t increment)
    {
        return DelayedTaskFactory::Make<testing::SteadyClock>(score::cpp::pmr::new_delete_resource(),
                                                              testing::SteadyClock::now(),
                                                              [this, increment](auto, auto) noexcept {
                                                                  executed_ += increment;
                                                              });
    }

    score::cpp::pmr::unique_ptr<TimedTask<testing::SteadyClock>> CreatePeriodicTask()
    {
        return PeriodicTaskFactory::Make<testing::SteadyClock>(score::cpp::pmr::new_delete_resource(),
                                                               testing::SteadyClock::now(),
                                                               std::chrono::milliseconds{100},
                                                               [this](auto, auto) noexcept {
                                                                   executed_++;
                                                               });
    }

    void Tick(ConcurrentTimedExecutor<concurrency::testing::SteadyClock>& unit, score::cpp::stop_token token = {})
    {
        unit.Work(token, conditional_variable);
    }

    testing::ExecutorMock* executor_mock_{nullptr};
    std::atomic<std::size_t> executed_{0};
    ThreadPool pool_{1};
    std::shared_ptr<concurrency::InterruptibleConditionalVariable> conditional_variable{
        std::make_shared<concurrency::InterruptibleConditionalVariable>()};

  private:
    score::cpp::pmr::unique_ptr<testing::ExecutorMock> executor_mock_stealed_{
        score::cpp::pmr::make_unique<testing::ExecutorMock>(score::cpp::pmr::new_delete_resource())};
};

namespace
{

using ::testing::_;
using ::testing::Return;

TEST_F(ConcurrentTimedExecutorFixture, CanConstruct)
{
    EXPECT_NO_FATAL_FAILURE(CreateConcurrentTimedExecutor());
}

TEST_F(ConcurrentTimedExecutorFixture, HoldsExpectedConcurrentActions)
{
    // Given a ConcurrentTimedExecutor with two threads
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();

    // When asking for the maximum concurrency level
    const auto concurrency_level = unit.MaxConcurrencyLevel();

    // Then that should be two
    EXPECT_EQ(concurrency_level, 2);
}

TEST_F(ConcurrentTimedExecutorFixture, ShutdownRequestedIsFalseIfNotReqeusted)
{
    // Given a ConcurrentTimedExecutor with two threads
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();

    // When asking if a shutdown has been requested
    const auto shutdown_requested = unit.ShutdownRequested();

    // Then that should be false
    EXPECT_FALSE(shutdown_requested);
}

TEST_F(ConcurrentTimedExecutorFixture, RequestingShutdownChangesState)
{
    // Given a ConcurrentTimedExecutor with two threads
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();

    // When shutting down
    unit.Shutdown();

    // Then it should be seen, that the shutdown was requested
    EXPECT_TRUE(unit.ShutdownRequested());
}

TEST_F(ConcurrentTimedExecutorFixture, MemoryResourceIsNotChanged)
{
    // Given a ConcurrentTimedExecutor with two threads
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();

    // When getting the memory resource
    const auto memory_resource = unit.GetMemoryResource();

    // Then it should equal to the expected resource
    EXPECT_EQ(memory_resource, score::cpp::pmr::new_delete_resource());
}

TEST_F(ConcurrentTimedExecutorFixture, ExecutesDelayedTaskIfDelayPointWasAlreadyReached)
{
    // Given a ConcurrentTimedExecutor with two threads, where a delayed Task was scheduled
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();
    unit.Post(CreateDelayedTask(1U));

    // When a tick happened
    Tick(unit);

    // Then the callback was executed once
    EXPECT_EQ(executed_, 1);
}

TEST_F(ConcurrentTimedExecutorFixture, ExecutesDelayedTaskOnlyOnce)
{
    // Given a ConcurrentTimedExecutor where one task was already executed
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();
    unit.Post(CreateDelayedTask(1U));
    Tick(unit);

    // When executing another one
    unit.Post(CreateDelayedTask(3U));
    Tick(unit);

    // Then the first task is not executed again
    EXPECT_EQ(executed_, 4);
}

TEST_F(ConcurrentTimedExecutorFixture, WorkIsAborted)
{
    // Given a ConcurrentTimedExecutor with two threads, with a stop source, which stop was already requested
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();
    score::cpp::stop_source stop_source{};
    stop_source.request_stop();

    // When running a tick
    Tick(unit, stop_source.get_token());

    // Then nothing blocks
}

TEST_F(ConcurrentTimedExecutorFixture, ExecutesPeriodicTasksPeriodically)
{
    // Given a ConcurrentTimedExecutor with two threads, where a periodic Task was scheduled and executed once
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();
    unit.Post(CreatePeriodicTask());
    Tick(unit);

    // When the time passed and another tick happens
    testing::SteadyClock::modify_time(std::chrono::milliseconds{100});
    Tick(unit);

    // Then the task was executed twice
    EXPECT_EQ(executed_, 2);
}

TEST_F(ConcurrentTimedExecutorFixture, ExecutesTasksInRightOrder)
{
    using namespace std::chrono_literals;

    // Given a ConcurrentTimedExecutor with two threads
    auto unit = CreateConcurrentTimedExecutorWithTwoThreads();

    // When scheduling three tasks with different execution orders
    std::size_t order{0};
    // to be executed at +5ms
    unit.Post(testing::SteadyClock::now() + 5ms, [&order](auto, auto) noexcept {
        order = 2U;
    });
    // to be executed at +0ms and then again +100ms
    unit.Post(100ms, [&order](auto, auto) noexcept {
        order = 1U;
    });
    // to be executed at 10ms and then again +60ms (10ms delay + 50ms cycle)
    unit.Post(testing::SteadyClock::now() + 10ms, 50ms, [&order, counter = 0U](auto, auto) mutable noexcept {
        if (counter == 0)
        {
            order = 3U;
            counter++;
        }
        else
        {
            order = 4U;
        }
    });

    // Then this order is hold
    Tick(unit);
    ASSERT_EQ(order, 1U);

    testing::SteadyClock::modify_time(5ms);
    Tick(unit);
    ASSERT_EQ(order, 2U);

    testing::SteadyClock::modify_time(5ms);
    Tick(unit);
    ASSERT_EQ(order, 3U);

    testing::SteadyClock::modify_time(60ms);
    Tick(unit);
    EXPECT_EQ(order, 4U);
}

TEST_F(ConcurrentTimedExecutorFixture, CanConstructAndDestructWithoutARaceCondition)
{
    auto unit = CreateConcurrentTimedExecutorWithRealThreadPool();
}

TEST_F(ConcurrentTimedExecutorFixture, EnsureThreadSafety)
{
    using namespace std::chrono_literals;
    std::atomic<std::chrono::steady_clock::time_point> slot_1{std::chrono::steady_clock::time_point::max()};
    std::atomic<std::chrono::steady_clock::time_point> slot_2{std::chrono::steady_clock::time_point::max()};
    std::atomic<std::chrono::steady_clock::time_point> slot_3{std::chrono::steady_clock::time_point::max()};
    std::atomic<std::chrono::steady_clock::time_point> slot_4{std::chrono::steady_clock::time_point::max()};

    // Given a ConcurrentTimedExecutor with a real thread pool
    auto unit = CreateConcurrentTimedExecutorWithRealThreadPool();

    // When scheduling three tasks with different execution orders

    // to be executed at +0ms and then again after 999 hours
    unit.Post(999h, [&slot_1](auto, auto) noexcept {
        slot_1 = std::chrono::steady_clock::now();
    });
    // to be executed at +5ms
    unit.Post(std::chrono::steady_clock::now() + 5ms, [&slot_2](auto, auto) noexcept {
        slot_2 = std::chrono::steady_clock::now();
    });
    // to be executed at 10ms and then again +60ms (10ms delay + 50ms cycle)
    unit.Post(
        std::chrono::steady_clock::now() + 10ms, 50ms, [&slot_3, &slot_4, counter = 0U](auto, auto) mutable noexcept {
            if (counter == 0)
            {
                slot_3 = std::chrono::steady_clock::now();
                counter++;
            }
            else
            {
                slot_4 = std::chrono::steady_clock::now();
                counter = 4U;
            }
        });

    // Waiting until all 4 tasks have been executed
    while (slot_4.load() == std::chrono::steady_clock::time_point::max())
    {
        std::this_thread::yield();
    }

    // Then the tasks have been scheduled in the right relative order
    ASSERT_LT(slot_1.load(), slot_2.load());
    ASSERT_LT(slot_2.load(), slot_3.load());
    EXPECT_LT(slot_3.load(), slot_4.load());
}

TEST_F(ConcurrentTimedExecutorFixture, EnsureWaitingForTasksExecutionPoint)
{
    using namespace std::chrono_literals;
    std::atomic<std::chrono::steady_clock::time_point> slot_1{std::chrono::steady_clock::time_point::max()};

    // Given a ConcurrentTimedExecutor with a real thread pool
    auto unit = CreateConcurrentTimedExecutorWithRealThreadPool();

    // When scheduling a task that will be executed after 10ms
    auto time_of_post = std::chrono::steady_clock::now();
    unit.Post(std::chrono::steady_clock::now() + 10ms, [&slot_1](auto, auto) noexcept {
        slot_1 = std::chrono::steady_clock::now();
    });

    // Waiting until this task was executed
    while (slot_1.load() == std::chrono::steady_clock::time_point::max())
    {
        std::this_thread::yield();
    }

    // Then we waited at least 10ms
    ASSERT_LE(time_of_post + 10ms, slot_1.load());

    // Note: There is no way to guarantee _exact_ timings, since this heavily depends on the system executed and
    // the system resource utilization.
}

TEST_F(ConcurrentTimedExecutorFixture, EnsureTaskIsRetrievedAndExecuted)
{
    using namespace std::chrono_literals;

    // Given a ConcurrentTimedExecutor with a real thread pool
    auto unit = CreateConcurrentTimedExecutorWithRealThreadPool();

    std::promise<void> task_executed_promise;
    std::future<void> task_executed_future = task_executed_promise.get_future();

    // When scheduling a task that will be executed immediately
    unit.Post(std::chrono::steady_clock::now(), [&task_executed_promise](auto, auto) noexcept {
        task_executed_promise.set_value();
    });

    // Wait until the task is executed
    task_executed_future.wait();

    ASSERT_TRUE(task_executed_future.valid());
}

TEST_F(ConcurrentTimedExecutorFixture, EnsureWakeUpNotifiesWaitingWorker)
{
    using namespace std::chrono_literals;

    // Given a ConcurrentTimedExecutor with a real thread pool
    auto unit = CreateConcurrentTimedExecutorWithRealThreadPool();

    std::promise<void> worker_notified_promise;
    std::future<void> worker_notified_future = worker_notified_promise.get_future();

    std::promise<void> task_executed_promise;
    std::future<void> task_executed_future = task_executed_promise.get_future();

    // When scheduling a task that will execute in 100ms
    auto future_time = std::chrono::steady_clock::now() + 100ms;
    unit.Post(future_time, [&task_executed_promise](auto, auto) noexcept {
        task_executed_promise.set_value();
    });

    unit.Post(std::chrono::steady_clock::now(), [&worker_notified_promise](auto, auto) noexcept {
        worker_notified_promise.set_value();
    });

    // Wait until the worker is actually notified
    worker_notified_future.wait();

    // Post another task to trigger WakeUp
    unit.Post(std::chrono::steady_clock::now(), [](auto, auto) noexcept {});

    // Wait until the future task executes
    task_executed_future.wait();

    ASSERT_TRUE(task_executed_future.valid());
}

}  // namespace
}  // namespace score::concurrency
