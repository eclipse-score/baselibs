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
#include "score/concurrency/executor.h"

#include "gtest/gtest.h"

#include <vector>

namespace score
{
namespace concurrency
{
namespace
{

class DummyExecutor final : public Executor
{
  public:
    explicit DummyExecutor(
        score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource()) noexcept
        : Executor(memory_resource)
    {
    }

    std::size_t MaxConcurrencyLevel() const noexcept override
    {
        return 1U;
    }

    bool ShutdownRequested() const noexcept override
    {
        return false;
    }

    void Shutdown() noexcept override {}

    std::vector<score::cpp::pmr::unique_ptr<Task>> enqueuedTasks{};

    void Enqueue(score::cpp::pmr::unique_ptr<Task> task) override
    {
        enqueuedTasks.emplace_back(std::move(task));
    }
};

class ExecutorTest : public ::testing::Test
{
  public:
    DummyExecutor unit{};
};

TEST_F(ExecutorTest, postCallable)
{
    // Given an Executor implementation

    // When posting a callable
    bool isExecuted{false};
    unit.Post([&isExecuted](const score::cpp::stop_token&) noexcept {
        isExecuted = true;
    });

    // That the callable is stored and can be executed
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
    (*(unit.enqueuedTasks.front()))(score::cpp::stop_token{});
    ASSERT_TRUE(isExecuted);
}

TEST_F(ExecutorTest, postCallableWithArgs)
{
    // Given an Executor implementation

    // When posting a callable
    bool isExecuted{false};
    unit.Post(
        [&isExecuted](const score::cpp::stop_token&, int, float) noexcept {
            isExecuted = true;
        },
        42,
        42.0);

    // That the callable is stored and can be executed
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
    (*(unit.enqueuedTasks.front()))(score::cpp::stop_token{});
    ASSERT_TRUE(isExecuted);
}

void postSomeTask(Executor& executor)
{
    executor.Post([](const score::cpp::stop_token&) {});
}

TEST_F(ExecutorTest, postCallableUsingInterface)
{
    // Given an Executor

    // When posting a callable using the interface (not the impl)
    postSomeTask(unit);

    // That the callable is stored
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
}

TEST_F(ExecutorTest, submitCallable)
{
    // Given an Executor

    // When submitting a callable
    auto future = unit.Submit([](const score::cpp::stop_token&) {
        return 42;
    });

    // That the callable is stored and can be executed
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
    (*(unit.enqueuedTasks.front()))(score::cpp::stop_token{});
    ASSERT_EQ(future.Get().value(), 42);
}

TEST_F(ExecutorTest, submitCallableWithArgs)
{
    // Given an Executor

    // When submitting a callable
    auto future = unit.Submit(
        [](const score::cpp::stop_token&, int arg1, float) noexcept {
            return arg1;
        },
        42,
        42.0);

    // That the callable is stored and can be executed
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
    (*(unit.enqueuedTasks.front()))(score::cpp::stop_token{});
    ASSERT_EQ(future.Get().value(), 42);
}

auto submitSomeTask(Executor& executor)
{
    return executor.Submit([](const score::cpp::stop_token&) {
        return 42;
    });
}

TEST_F(ExecutorTest, submitCallableUsingInterface)
{
    // Given an Executor

    // When submitting a callable using the interface (not the impl)
    auto future = submitSomeTask(unit);

    // That the callable is stored and can be executed
    ASSERT_EQ(unit.enqueuedTasks.size(), 1);
    (*(unit.enqueuedTasks.front()))(score::cpp::stop_token{});
    ASSERT_EQ(future.Get().value(), 42);
}

class MyTask : public Task
{
  public:
    std::promise<void> promise{};
    score::cpp::stop_source stop_source_{};
    void operator()(const score::cpp::stop_token) noexcept override
    {
        promise.set_value();
    }
    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }
};

TEST_F(ExecutorTest, postCustomTask)
{

    auto task = score::cpp::pmr::make_unique<MyTask>(score::cpp::pmr::get_default_resource());
    auto future = task->promise.get_future();

    unit.Post(std::move(task));
}

void postCustomTask(Executor& executor)
{
    executor.Post(score::cpp::pmr::make_unique<MyTask>(score::cpp::pmr::get_default_resource()));
}

TEST_F(ExecutorTest, postCustomTaskUsingExecutorInterface)
{
    postCustomTask(unit);
}

TEST_F(ExecutorTest, GetMemoryResource)
{
    // With default memory resource
    EXPECT_EQ(unit.GetMemoryResource(), score::cpp::pmr::get_default_resource());

    // With custom memory resource
    score::cpp::pmr::monotonic_buffer_resource custom_memory_resource{};
    DummyExecutor custom_unit{&custom_memory_resource};
    EXPECT_EQ(custom_unit.GetMemoryResource(), &custom_memory_resource);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
