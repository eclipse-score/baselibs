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
#include "score/concurrency/synchronized_queue.h"
#include "gtest/gtest.h"

#if !defined(__QNX__)
#include <valgrind/valgrind.h>
#define TEST_RUNNING_ON_VALGRIND (RUNNING_ON_VALGRIND)
#else  // defined(__QNX__)
#define TEST_RUNNING_ON_VALGRIND (false)
#endif  // !defined(__QNX__)

#include <atomic>
#include <cstdint>
#include <optional>

namespace score::concurrency
{

using namespace std::literals::chrono_literals;

TEST(SynchronizedQueue, CheckFalseResponseOnPushWhenMaxQueueLengthReached)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that QueueSender::Push() would return false if queue is full and pushing into the "
                   "queue is impossible");

    const int maxQueueLength = 5;
    const int32_t push_value = 6;

    // Create synchonized queue with max queue length = 5
    SynchronizedQueue<int32_t> sync_queue(maxQueueLength);

    // Create two senders to queue
    auto sender1 = sync_queue.CreateSender();
    auto sender2 = sync_queue.CreateSender();

    // That push() method returns true if max queue length is not reached yet and push is successfull
    EXPECT_TRUE(sender1.push(1));
    EXPECT_TRUE(sender1.push(2));
    EXPECT_TRUE(sender1.push(3));
    EXPECT_TRUE(sender2.push(4));
    EXPECT_TRUE(sender2.push(5));
    // That push() method by rvalue reference returns false if queue length equals to max queue length
    // and push is failed
    EXPECT_FALSE(sender2.push(6));
    // That push() method by const reference returns false if queue length equals to max queue length
    // and push is failed
    EXPECT_FALSE(sender2.push(push_value));
}

TEST(SynchronizedQueue, CheckFalseResponseOnPushWhenQueueObjectDoesNotExist)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that QueueSender::Push() would return false "
                   "if queue object doesn't exist anymore");

    const int maxQueueLength = 5;
    const int32_t push_value = 1;

    // Create synchonized queue with long life cicle
    SynchronizedQueue<int32_t> sync_queue_long(maxQueueLength);

    // Create sender to queue
    auto sender = sync_queue_long.CreateSender();

    // Create new queue with limited life cycle
    {
        // Create synchonized queue with max queue length = 5
        SynchronizedQueue<int32_t> sync_queue_short(maxQueueLength);

        // Copy shared state from the queue with short life cicle
        sender = sync_queue_short.CreateSender();
    }
    // That push() method by rvalue reference returns false if queue doesn't exist anymore
    EXPECT_FALSE(sender.push(1));
    // That push() method by const reference returns false if queue doesn't exist anymore
    EXPECT_FALSE(sender.push(push_value));
}

TEST(SynchronizedQueue, CallPopForEmptyQueue)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return nullopt if queue is empty");

    const int max_queue_length = 5;
    const auto sleep_duration = 100ms;

    // Create synchonized queue with max queue length = 5
    SynchronizedQueue<int32_t> sync_queue(max_queue_length);

    auto pop_start_time = std::chrono::steady_clock::now();

    // Try to read the empty queue with provided timeout
    auto result = sync_queue.Pop(sleep_duration, score::cpp::stop_token{});

    auto pop_end_time = std::chrono::steady_clock::now();

    // That Pop() method is waiting for given timeout before response if queue is empty
    EXPECT_TRUE(pop_end_time >= pop_start_time + sleep_duration);

    // That result of Pop() for empty queue has no value
    EXPECT_FALSE(result.has_value());
}

TEST(SynchronizedQueue, CallPushWhenPopIsWaitingForTimeout)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return the value "
                   "if it's in waiting state and QueueSender pushes new value into the queue");

    const int max_queue_length = 5;
    const auto sleep_duration = 100ms;
    const auto postpone_push_duration = 10ms;

    // Create synchonized queue with max queue length = 5
    SynchronizedQueue<int32_t> sync_queue(max_queue_length);

    // Create ender to queue
    auto sender = sync_queue.CreateSender();

    score::cpp::stop_source st_source;
    const score::cpp::stop_token st_token = st_source.get_token();

    std::thread receiver_thread([&sync_queue, &sleep_duration, &st_token]() noexcept {
        auto result = sync_queue.Pop(sleep_duration, st_token);
        // That Pop() returns value from queue if this value was pushed to the queue
        // after Pop() was called
        EXPECT_TRUE(result.has_value());
    });

    std::thread sender_thread([&sender, &postpone_push_duration]() noexcept {
        std::this_thread::sleep_for(postpone_push_duration);
        sender.push(1);
    });

    receiver_thread.join();
    sender_thread.join();
}

TEST(SynchronizedQueue, MakeStressTestForPushingFromMultipleThreads)
{
#ifdef __SANITIZE_ADDRESS__
    constexpr std::size_t num_threads = 100;
#else
    const std::size_t num_threads = TEST_RUNNING_ON_VALGRIND ? 100 : 1000;
#endif

    constexpr std::size_t num_values_per_thread = 100;
    const std::size_t max_queue_length = num_threads * num_values_per_thread;

    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This is a smoke test where test 1000 threads are created and each of them pushes 100 values "
                   "into the queue. "
                   "Test checks that all the values are in the queue.");

    const auto sleep_duration = 100ms;
    // Create queue with size = 100000 elements
    SynchronizedQueue<std::int32_t> sync_queue(max_queue_length);

    score::cpp::stop_source st_source;
    const score::cpp::stop_token st_token = st_source.get_token();

    // Value that will be increased everytime new thread is running and pushed into the queue
    std::atomic<std::int32_t> value{1};

    std::vector<std::thread> sender_threads;

    // Create and run vector of threads (1000 threads)
    for (std::size_t i = 0; i < num_threads; i++)
    {
        sender_threads.emplace_back([&sync_queue, &value]() noexcept {
            auto sender = sync_queue.CreateSender();
            // Increase value every time when thread tries to push new element into the queue
            for (std::size_t a = 0; a < num_values_per_thread; a++)
            {
                std::int32_t val = value.fetch_add(1, std::memory_order_relaxed);
                sender.push(val);
            }
        });
    }

    // Create thread for queue reader
    std::thread reader_thread([&st_token, &sleep_duration, &sync_queue, max_queue_length]() noexcept {
        std::set<std::int32_t> set;
        std::optional<std::int32_t> result(0);
        result = 0;

        EXPECT_TRUE(result.has_value());

        while (result.has_value())
        {
            result = sync_queue.Pop(sleep_duration, st_token);

            if (result.has_value())
            {
                set.insert(result.value());
            }
        }
        // That set size is max_queue_length
        ASSERT_TRUE(set.size() == max_queue_length);

        std::int32_t previous_value = 0;
        std::int32_t tmp = 0;

        for (const auto& element : set)
        {
            // That there are all the elements pished to the queue
            EXPECT_TRUE(element > previous_value);
            tmp++;
            EXPECT_EQ(tmp, element);
            previous_value = element;
        }
    });

    for (auto& thr : sender_threads)
    {
        thr.join();
    }
    reader_thread.join();
}

}  // namespace score::concurrency
