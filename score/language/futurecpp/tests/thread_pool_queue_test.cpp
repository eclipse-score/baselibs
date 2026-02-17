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

#include <score/private/execution/thread_pool_queue.hpp>
#include <score/private/execution/thread_pool_queue.hpp> // test include guard

#include <score/assert.hpp>
#include <score/jthread.hpp>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace execution
{
namespace detail
{
namespace
{

struct element : score::cpp::detail::intrusive_forward_list_node
{
    element() = default;
    explicit element(const std::int32_t value) : score::cpp::detail::intrusive_forward_list_node{}, v{value} {}
    element(const element&) = default;
    element& operator=(const element&) = default;
    element(element&&) = default;
    element& operator=(element&&) = default;
    ~element() { SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(!is_linked()); }

    std::int32_t v;
};

// NOTRACING
TEST(thread_pool_queue_test, push_GivenNonEmptyQueue_ExpectValuePopped)
{
    thread_pool_queue<element> queue{};

    element v{42};
    queue.push(v);

    const element* const popped_value{queue.pop()};

    ASSERT_NE(popped_value, nullptr);
    EXPECT_EQ(popped_value->v, 42);
    EXPECT_EQ(&v, popped_value);
}

// NOTRACING
TEST(thread_pool_queue_test, try_to_push_GivenNonEmptyQueue_ExpectValuePopped)
{
    thread_pool_queue<element> queue{};

    element v{42};
    queue.try_to_push(v);

    const element* const popped_value{queue.try_to_pop()};

    ASSERT_NE(popped_value, nullptr);
    EXPECT_EQ(popped_value->v, 42);
    EXPECT_EQ(&v, popped_value);
}

// NOTRACING
TEST(thread_pool_queue_test, pop_GivenPopMayBlock_ExpectUnblockedWhenPush)
{
    element v{42};

    thread_pool_queue<element> queue{};

    score::cpp::jthread popper{[&queue, &v]() {
        const element* const popped_value{queue.pop()};
        EXPECT_TRUE((popped_value == nullptr) || (popped_value == &v));
    }};

    queue.push(v);
}

// NOTRACING
TEST(thread_pool_queue_test, pop_GivenPopMayBlock_ExpectUnblockedWhenTryToPush)
{
    element v{42};

    thread_pool_queue<element> queue{};

    score::cpp::jthread popper{[&queue, &v]() {
        const element* const popped_value{queue.pop()};
        EXPECT_TRUE((popped_value == nullptr) || (popped_value == &v));
    }};

    while (!queue.try_to_push(v))
    {
        // do nothing but ensure element is pushed eventually
    }
}

// NOTRACING
TEST(thread_pool_queue_test, pop_GivenPopMayBlock_ExpectUnblockedWhenAbort)
{
    thread_pool_queue<element> queue{};

    score::cpp::jthread popper{[&queue]() { EXPECT_EQ(queue.pop(), nullptr); }};

    queue.abort();
}

// NOTRACING
TEST(thread_pool_queue_test, try_to_pop_GivenEmptyQueue_ExpectNullptr)
{
    thread_pool_queue<element> queue{};

    EXPECT_EQ(queue.try_to_pop(), nullptr);
}

// NOTRACING
TEST(thread_pool_queue_test, abort_GivenAbortOnEmptyQueueWasCalled_ExpectPopToNotBlockThread)
{
    thread_pool_queue<element> queue{};

    queue.abort();

    EXPECT_EQ(queue.pop(), nullptr);
}

// NOTRACING
TEST(thread_pool_queue_test, abort_GivenAbortOnNonEmptyQueueWasCalled_ExpectPopToNotBlockThread)
{
    element v{42};

    thread_pool_queue<element> queue{};
    queue.push(v);

    queue.abort();

    EXPECT_EQ(queue.pop(), nullptr);
}

} // namespace
} // namespace detail
} // namespace execution
} // namespace score::cpp
