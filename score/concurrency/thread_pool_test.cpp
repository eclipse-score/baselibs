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
#include "score/concurrency/thread_pool.h"
#include "score/concurrency/notification.h"

#include "score/memory_resource.hpp"

#include "gtest/gtest.h"

#include <optional>
#include <thread>

namespace score
{
namespace concurrency
{
namespace
{

TEST(ThreadPool, ConstructionAndDestructionOnStack)
{
    ThreadPool thread_pool{1U};
}

TEST(ThreadPool, ConstructionWithCustomName)
{
    // Given a ThreadPool constructed with a custom name and a positive thread count
    ThreadPool unit{1U, "custom_pool"};

    // When submitting a task
    std::atomic<std::size_t> counter{0};
    auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });

    // Then the task is executed
    score::cpp::ignore = f.Get();
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, ConstructionAndDestructionOnHeap)
{
    auto unique_thread_pool = std::make_unique<ThreadPool>(1U);
    unique_thread_pool.reset();
}

TEST(ThreadPool, ConstructionAndDestructionOnHeapWithBasePointer)
{
    std::unique_ptr<ThreadPool> unique_thread_pool = std::make_unique<ThreadPool>(1U);
    unique_thread_pool.reset();
}

TEST(ThreadPool, executesSubmittedCallables)
{
    // Given a ThreadPool with two threads
    ThreadPool unit{2U};

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

TEST(ThreadPool, correctMaxConcurrencyLevel)
{
    // Given a ThreadPool with 5 threads
    ThreadPool unit{5U};

    // When querying the maximum concurrency level
    // That 5 is returned
    ASSERT_EQ(unit.MaxConcurrencyLevel(), 5U);
}

TEST(ThreadPool, stopRequestFunctional)
{
    // Given a ThreadPool with two threads, that includes two long running tasks
    ThreadPool unit{2U};

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
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool with two threads
    std::optional<ThreadPool> unit{2U};

    // When posting two long running tasks
    unit->Post([&counter](const score::cpp::stop_token& token) noexcept {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
        ++counter;
    });
    unit->Post([&counter](const score::cpp::stop_token& token) noexcept {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
        ++counter;
    });

    // And immediately destroying the ThreadPool afterwards
    unit.reset();

    // Then it must shutdown its worker threads
    // And still guarantee the execution of all queued tasks
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, CanAbortThread)
{
    // Given a ThreadPool with two threads, that includes one long running tasks
    ThreadPool unit{2U};

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

TEST(ThreadPool, PostTaskFromWithinAnotherTask)
{
    std::atomic<std::size_t> counter{0};
    concurrency::Notification first_done{};
    concurrency::Notification second_done{};

    // Given a ThreadPool with two threads
    ThreadPool unit{2U};

    // When posting a task which itself posts another task
    unit.Post([&](const score::cpp::stop_token&) {
        unit.Post([&](const score::cpp::stop_token&) {
            ++counter;
            second_done.notify();
        });
        ++counter;
        first_done.notify();
    });

    second_done.waitWithAbort({});
    first_done.waitWithAbort({});

    // Then both tasks must have gotten executed asynchronously
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, SubmitTaskFromWithinAnotherTask)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool with two threads
    ThreadPool unit{2U};

    // When submitting a task which itself submits another task
    unit.Submit([&](const score::cpp::stop_token&) noexcept {
            unit.Submit([&](const score::cpp::stop_token&) noexcept {
                    ++counter;
                })
                .Wait();
            ++counter;
        })
        .Wait();

    // Then both tasks must have gotten executed asynchronously
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, PostTaskFromWithinAnotherTaskAfterShutdown)
{
    std::atomic<std::size_t> counter{0};
    concurrency::Notification first_done{};
    concurrency::Notification second_done{};

    // Given a ThreadPool without any thread which got already shutdown
    ThreadPool unit{0U};
    unit.Shutdown();

    ASSERT_TRUE(unit.ShutdownRequested());

    // When posting a task which itself posts another task
    unit.Post([&](const score::cpp::stop_token&) {
        unit.Submit([&](const score::cpp::stop_token&) {
            ++counter;
            second_done.notify();
        });
        ++counter;
        first_done.notify();
    });

    second_done.waitWithAbort({});
    first_done.waitWithAbort({});

    // Then both tasks must have gotten executed synchronously
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, SubmitTaskFromWithinAnotherTaskAfterShutdown)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool without any thread which got already shutdown
    ThreadPool unit{0U};
    unit.Shutdown();

    ASSERT_TRUE(unit.ShutdownRequested());

    // When submitting a task which itself submits another task
    unit.Submit([&unit, &counter](const score::cpp::stop_token&) noexcept {
        unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
            ++counter;
        });
        ++counter;
    });

    // Then both tasks must have gotten executed synchronously
    ASSERT_EQ(counter, 2U);
}

TEST(ThreadPool, SubmitTaskWhileThreadPoolWasAlreadyRequestedToShutDown)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool without any thread which got already shutdown
    ThreadPool unit{0U};
    unit.Shutdown();

    // When submitting one task
    ASSERT_TRUE(unit.ShutdownRequested());
    auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        ++counter;
    });

    // Then the task must have gotten executed nonetheless
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, PostTaskWhileThreadPoolWasAlreadyRequestedToShutDown)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool without any thread which got already shutdown
    ThreadPool unit{0U};
    unit.Shutdown();

    // When posting one task
    ASSERT_TRUE(unit.ShutdownRequested());
    unit.Post([&counter](const score::cpp::stop_token&) noexcept {
        ++counter;
    });

    // Then the task must have gotten executed nonetheless
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, SubmitTaskAndDestroyThreadPoolDirectlyAfterwards)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool with one thread
    std::optional<ThreadPool> unit{1U};

    // When submitting one task prior to destroying the ThreadPool
    auto f = unit->Submit([&counter](const score::cpp::stop_token&) noexcept {
        ++counter;
    });

    // And destroying the ThreadPool directly afterwards
    unit.reset();

    // Then the task must have gotten executed nonetheless
    f.Wait();
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, PostTaskAndDestroyThreadPoolDirectlyAfterwards)
{
    std::atomic<std::size_t> counter{0};

    // Given a ThreadPool with one thread
    std::optional<ThreadPool> unit{1U};

    // When posting one task prior to destroying the ThreadPool
    unit->Post([&counter](const score::cpp::stop_token&) noexcept {
        ++counter;
    });

    // And destroying the ThreadPool directly afterwards
    unit.reset();

    // Then the task must have gotten executed nonetheless
    ASSERT_EQ(counter, 1U);
}

TEST(ThreadPool, SubmitTasksWhileThreadPoolGetsShutDown)
{
    constexpr std::size_t kMaxNumRuns{1000};
    constexpr std::size_t kNumSubmits{10};
    std::atomic<std::size_t> counter{0};

    for (std::size_t num_run{1}; num_run <= kMaxNumRuns; ++num_run)
    {
        concurrency::Notification producer_is_running{};

        // Given a ThreadPool with one thread
        ThreadPool unit{1U};

        // When repeatedly submitting tasks
        auto producer_finished = std::async(std::launch::async, [&] {
            producer_is_running.notify();
            for (std::size_t num_submit{0}; num_submit < kNumSubmits; ++num_submit)
            {
                auto task_future = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
                    ++counter;
                });
                task_future.Wait();
            }
        });

        // At the same time while the ThreadPool gets requested to shut down
        auto thread_pool_got_shutdown = std::async([&] {
            producer_is_running.waitWithAbort({});
            unit.Shutdown();
            ASSERT_TRUE(unit.ShutdownRequested());
        });

        // Then all submitted tasks must have gotten executed
        thread_pool_got_shutdown.wait();
        producer_finished.wait();
        ASSERT_EQ(counter, num_run * kNumSubmits);
    };
}

TEST(ThreadPool, PostTasksWhileThreadPoolGetsShutDown)
{
    constexpr std::size_t kMaxNumRuns{1000};
    constexpr std::size_t kNumSubmits{10};
    std::atomic<std::size_t> counter{0};

    for (std::size_t num_run{1}; num_run <= kMaxNumRuns; ++num_run)
    {
        concurrency::Notification producer_is_running{};

        // Given a ThreadPool with one thread
        ThreadPool unit{1U};

        // When repeatedly posting tasks
        auto producer_finished = std::async(std::launch::async, [&] {
            producer_is_running.notify();
            for (std::size_t num_submit{0}; num_submit < kNumSubmits; ++num_submit)
            {
                unit.Post([&counter](const score::cpp::stop_token&) noexcept {
                    ++counter;
                });
            }
            const auto start = std::chrono::steady_clock::now();
            constexpr std::chrono::seconds kMaxWait{10};
            while (counter != (num_run * kNumSubmits))
            {
                if (std::chrono::steady_clock::now() > (start + kMaxWait))
                {
                    ASSERT_EQ(counter, num_run * kNumSubmits);
                    break;
                }
                std::this_thread::yield();
            }
        });

        // While at the same time shutting down the ThreadPool
        auto thread_pool_got_shutdown = std::async([&] {
            producer_is_running.waitWithAbort({});
            unit.Shutdown();
            ASSERT_TRUE(unit.ShutdownRequested());
        });

        // Then all posted tasks must have gotten executed
        thread_pool_got_shutdown.wait();
        producer_finished.wait();
        ASSERT_EQ(counter, num_run * kNumSubmits);
    };
}

class TaskMemoryResource : public score::cpp::pmr::memory_resource
{
  private:
    void* do_allocate(std::size_t bytes, std::size_t) override
    {
        allocated_memory_ += bytes;
        return malloc(bytes);
    }
    void do_deallocate(void* p, std::size_t bytes, std::size_t) override
    {
        allocated_memory_ -= bytes;
        free(p);
    }
    bool do_is_equal(const memory_resource&) const noexcept override
    {
        return false;
    };

  public:
    std::atomic<std::size_t> allocated_memory_{};
};

TEST(ThreadPool, UsingCustomMemoryResource)
{
    // Given a ThreadPool that uses a custom memory resource
    TaskMemoryResource memory_resource{};
    std::atomic<std::size_t> counter{0};
    ThreadPool unit{2U, &memory_resource};

    // When submitting tasks
    auto f = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });
    auto f2 = unit.Submit([&counter](const score::cpp::stop_token&) noexcept {
        counter++;
    });

    // That these tasks are allocated on the custom memory resource
    ASSERT_GT(memory_resource.allocated_memory_, 0);
    EXPECT_TRUE(f.Get());
    EXPECT_TRUE(f2.Get());
    ASSERT_EQ(counter, 2U);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
