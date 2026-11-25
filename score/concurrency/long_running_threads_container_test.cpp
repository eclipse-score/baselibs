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
#include "score/concurrency/long_running_threads_container.h"

#include "gtest/gtest.h"

#include <thread>

namespace score
{
namespace concurrency
{
namespace
{

TEST(LongRunningThreadsContainer, ConstructionAndDestructionOnStack)
{
    LongRunningThreadsContainer unit{};
}

TEST(ThreadPool, ConstructionAndDestructionOnHeap)
{
    auto unit = std::make_unique<LongRunningThreadsContainer>();
    unit.reset();
}

TEST(ThreadPool, ConstructionAndDestructionOnHeapWithBasePointer)
{
    std::unique_ptr<LongRunningThreadsContainer> unit = std::make_unique<LongRunningThreadsContainer>();
    unit.reset();
}

TEST(ThreadPool, ExecutesSubmittedCallables)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__long_running_threads");
    RecordProperty("Description", "Check that a LongRunningThreadsContainer executes both submitted callables.");

    // Given a LongRunningThreadsContainer
    LongRunningThreadsContainer unit{};

    // When submitting two tasks
    std::atomic<std::size_t> counter{0};
    auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });
    auto f2 = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });

    // That both tasks are executed
    EXPECT_TRUE(f.Get());
    EXPECT_TRUE(f2.Get());
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, CorrectMaxConcurrencyLevel)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__thread_count_reporting");
    RecordProperty("Description",
                   "Check that MaxConcurrencyLevel() of a LongRunningThreadsContainer reports more than 100 "
                   "threads.");

    // Given a LongRunningThreadsContainer
    LongRunningThreadsContainer unit{};

    // When querying the maximum concurrency level
    // That an eternal number is returned
    ASSERT_GT(unit.MaxConcurrencyLevel(), 100);
}

TEST(ThreadPool, StopRequestFunctional)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__long_running_threads");
    RecordProperty("Description",
                   "Check that ShutdownRequested() is false before and true after Shutdown() on a "
                   "LongRunningThreadsContainer with running tasks.");

    // Given a LongRunningThreadsContainer with two threads, that includes two long running tasks
    LongRunningThreadsContainer unit{};

    std::atomic<std::size_t> counter{0};
    unit.Post([&counter](const score::cpp::stop_token& token) noexcept {
        counter++;
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    });
    unit.Post([&counter](const score::cpp::stop_token& token) noexcept {
        counter++;
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    });

    // Ensuring that both tasks have started
    while (counter != 2)
    {
        std::this_thread::yield();
    }

    // When calling the ThreadPool to shutdown
    ASSERT_FALSE(unit.ShutdownRequested());
    unit.Shutdown();

    // That the shutdown request is executed respectively
    ASSERT_TRUE(unit.ShutdownRequested());
}

TEST(ThreadPool, DestructionStopsAndJoinsThreads)
{
    // Given a LongRunningThreadsContainer with two threads, that includes one long running tasks
    LongRunningThreadsContainer unit{};
    unit.Post([](const score::cpp::stop_token& token) noexcept {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    });

    // When destructing it
    // That it shutdown the threads
}

TEST(ThreadPool, CanAbortThread)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__task_cancellation");
    RecordProperty("Description",
                   "Check that aborting the result of a long running task requests a stop on the running task.");

    // Given a ThreadPool with two threads, that includes one long running tasks
    LongRunningThreadsContainer unit{};

    std::atomic<bool> abort_token_set{false};
    auto result = unit.Submit([&abort_token_set](const score::cpp::stop_token& token) noexcept {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
        abort_token_set = true;
    });

    // When the user is aborting the long running task
    result.Abort();

    // That it is requested to shutdown
    while (!abort_token_set)
    {
        std::this_thread::yield();
    };
}

TEST(ThreadPool, ExecuteTaskWhileThreadPoolWasAlreadyRequestedToShutDown)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__executor_interface");
    RecordProperty("Description",
                   "Check that a task submitted to a LongRunningThreadsContainer after Shutdown() is still "
                   "executed.");

    std::atomic<std::size_t> counter{0};

    // Given a LongRunningThreadsContainer with one thread that is already shutdown
    LongRunningThreadsContainer unit{};
    unit.Shutdown();

    // When submitting one tasks
    auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });

    // That this task is executed
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, ExecuteTaskWhileThreadPoolWasAlreadyDestructed)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "error-guessing");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__long_running_threads");
    RecordProperty("Description",
                   "Check that a task submitted right before destroying a LongRunningThreadsContainer is still "
                   "executed.");

    std::atomic<std::size_t> counter{0};

    {
        // Given a LongRunningThreadsContainer with one threads
        LongRunningThreadsContainer unit{};

        // When submitting one tasks while the LongRunningThreadsContainer is destructed in parallel
        auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
            counter++;
        });
    }

    // That this task is executed
    ASSERT_EQ(counter, 1U);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
