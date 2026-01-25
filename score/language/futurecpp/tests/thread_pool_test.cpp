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

#include <score/private/execution/thread_pool.hpp>
#include <score/private/execution/thread_pool.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/latch.hpp>

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace detail
{
namespace
{

// NOTRACING
TEST(thread_pool_test, constructor_GivenNonPositiveWorkerCount_ExpectPreconditionViolated)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(thread_pool{thread_pool::worker_count{-10}});
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(thread_pool{thread_pool::worker_count{0}});
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(thread_pool{thread_pool::worker_count{10}});
}

// NOTRACING
TEST(thread_pool_test, max_concurrency_GivenNumberOfWorkers_ExpectReturnNumberOfWorkers)
{
    EXPECT_EQ(1, thread_pool{thread_pool::worker_count{1}}.max_concurrency());
    EXPECT_EQ(2, thread_pool{thread_pool::worker_count{2}}.max_concurrency());
    EXPECT_EQ(3, thread_pool{thread_pool::worker_count{3}}.max_concurrency());
}

class thread_pool_test_task final : public score::cpp::execution::detail::base_task
{
public:
    thread_pool_test_task(std::int32_t& n, score::cpp::latch& l) : n_{n}, l_{l} {}

    void start() override
    {
        n_ *= n_;
        l_.count_down();
    }

    void disable() override { FAIL() << "disable shall not be called"; }

private:
    std::int32_t& n_;
    score::cpp::latch& l_;
};

// NOTRACING
TEST(thread_pool_test, push_GivenTasksArePushed_ExpectAllTasksToBeExecuted)
{
    const std::size_t count{16};
    thread_pool pool{thread_pool::worker_count{2}};
    score::cpp::latch latch{count};

    std::vector<std::int32_t> numbers(count);
    std::vector<std::int32_t> expected(count);

    std::iota(numbers.begin(), numbers.end(), 0);

    for (std::size_t i{0U}; i < count; ++i)
    {
        expected[i] = numbers[i] * numbers[i];
    }

    std::vector<thread_pool_test_task> tasks{};
    tasks.reserve(numbers.size());

    for (auto& n : numbers)
    {
        tasks.emplace_back(n, latch);
        pool.push(tasks.back());
    }

    latch.wait();

    EXPECT_TRUE(std::all_of(tasks.cbegin(), tasks.cend(), [](const base_task& t) { return !t.is_linked(); }));
    EXPECT_EQ(expected, numbers);
}

class thread_pool_test_state_task final : public base_task
{
public:
    enum class state : std::int32_t
    {
        initial,
        start,
        disable,
    };

    explicit thread_pool_test_state_task(std::atomic<state>& s) : state_{&s} {}

    void start() override
    {
        EXPECT_EQ(state::initial, *state_);
        *state_ = state::start;
    }
    void disable() override
    {
        EXPECT_EQ(state::initial, *state_);
        *state_ = state::disable;
    }

private:
    std::atomic<state>* state_;
};

// NOTRACING
TEST(thread_pool_test, push_GivenTaskIsPushed_ExpectStartCalledOnTaskEventually)
{
    std::atomic<thread_pool_test_state_task::state> state{};
    thread_pool_test_state_task task{state};

    thread_pool pool{thread_pool::worker_count{1}};
    pool.push(task);

    while (state == thread_pool_test_state_task::state::initial)
    {
        // busy wait
    }

    EXPECT_TRUE(!task.is_linked());
    EXPECT_EQ(state, thread_pool_test_state_task::state::start);
}

// NOTRACING
TEST(thread_pool_test, push_GivenTaskIsPushed_ExpectTaskIsNotInStateInitialWhenThreadPoolStops)
{
    std::atomic<thread_pool_test_state_task::state> state{};
    thread_pool_test_state_task task{state};

    {
        thread_pool pool{thread_pool::worker_count{1}};
        pool.push(task);
    }

    EXPECT_TRUE(!task.is_linked());
    EXPECT_NE(state, thread_pool_test_state_task::state::initial);
}

} // namespace
} // namespace detail
} // namespace execution
} // namespace score::cpp
