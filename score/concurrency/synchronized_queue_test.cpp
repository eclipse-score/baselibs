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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <atomic>
#include <cstdint>
#include <optional>

namespace score::concurrency
{

using namespace std::literals::chrono_literals;

// Mock class specializing the template
class MockNotification
{
  public:
    // Mocked method for Notification
    virtual ~MockNotification() = default;
    MOCK_METHOD(bool, waitForWithAbort, ((std::chrono::duration<int64_t, std::milli>), score::cpp::stop_token));
    MOCK_METHOD(bool, waitWithAbort, (score::cpp::stop_token token));
    MOCK_METHOD(void, notify, ());
    MOCK_METHOD(void, reset, ());
};

using ::testing::_;
using ::testing::Return;

TEST(SynchronizedQueue, CheckFalseResponseOnPushWhenMaxQueueLengthReached)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that QueueSender::Push() would return false if queue is full and pushing into the "
                   "queue is impossible");

    const std::int32_t maxQueueLength = 5;
    const std::int32_t push_value = 6;

    // Create synchonized queue with max queue length = 5
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue(maxQueueLength);

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
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that QueueSender::Push() would return false "
                   "if queue object doesn't exist anymore");

    const std::int32_t maxQueueLength = 5;
    const std::int32_t push_value = 1;

    // Given synchonized queue with long life cicle
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue_long(maxQueueLength);
    // Create sender to queue
    auto sender = sync_queue_long.CreateSender();

    // When new queue with limited life cycle
    {
        // When synchonized queue with max queue length = 5
        SynchronizedQueue<std::int32_t, MockNotification> sync_queue_short(maxQueueLength);
        sender = sync_queue_short.CreateSender();
    }
    // Then push() method by rvalue reference returns false if queue doesn't exist anymore
    EXPECT_FALSE(sender.push(1));
    // Then push() method by const reference returns false if queue doesn't exist anymore
    EXPECT_FALSE(sender.push(push_value));
}

TEST(SynchronizedQueue, CallPopForEmptyQueue)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return nullopt if queue is empty");

    const std::int32_t max_queue_length = 5;
    const auto sleep_duration = 100ms;

    auto shared_state = std::make_shared<details::SharedState<std::int32_t, MockNotification>>(max_queue_length);

    EXPECT_CALL(shared_state->notification_, waitForWithAbort(::testing::_, ::testing::_)).WillOnce([]() {
        return false;
    });

    // Given synchonized queue with max queue length = 5
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue(std::move(shared_state));

    // When calling the pop method with this expecation
    auto result = sync_queue.Pop(sleep_duration, score::cpp::stop_token{});

    // Then result of Pop() for empty queue has no value
    EXPECT_FALSE(result.has_value());
}

TEST(SynchronizedQueue, CallPushWhenPopIsWaitingForTimeout)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return the value "
                   "if it's in waiting state and QueueSender pushes new value into the queue");

    const std::int32_t max_queue_length = 5;
    const auto sleep_duration = 100ms;

    auto shared_state = std::make_shared<details::SharedState<std::int32_t, MockNotification>>(max_queue_length);

    auto& shared_state_ref = *shared_state;

    // Given synchonized queue with max queue length = 5
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue(std::move(shared_state));

    // Create sender to queue
    auto sender = sync_queue.CreateSender();

    score::cpp::stop_source st_source;
    const score::cpp::stop_token st_token = st_source.get_token();
    std::int32_t push_value = 1;

    EXPECT_CALL(shared_state_ref.notification_, waitForWithAbort(::testing::_, ::testing::_))
        .WillOnce([&sender, push_value]() {
            sender.push(push_value);
            return true;
        });

    EXPECT_CALL(shared_state_ref.notification_, notify());

    // When calling the pop method with this expecation
    auto result = sync_queue.Pop(sleep_duration, st_token);
    // Then Pop() returns value from queue if this value was pushed to the queue
    // after Pop() was called
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(push_value, result.value());
}

TEST(SynchronizedQueue, CallPushWithConstValueWhenPopIsWaitingForTimeout)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return the value "
                   "if it's in waiting state and QueueSender pushes new const value into the queue");

    const std::int32_t max_queue_length = 5;
    const auto sleep_duration = 100ms;

    auto shared_state = std::make_shared<details::SharedState<std::int32_t, MockNotification>>(max_queue_length);

    auto& shared_state_ref = *shared_state;

    // Given synchonized queue with max queue length = 5
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue(std::move(shared_state));

    // Create sender to queue
    auto sender = sync_queue.CreateSender();

    score::cpp::stop_source st_source;
    const score::cpp::stop_token st_token = st_source.get_token();
    const std::int32_t push_value = 1;

    EXPECT_CALL(shared_state_ref.notification_, waitForWithAbort(::testing::_, ::testing::_))
        .WillOnce([&sender, push_value]() {
            sender.push(push_value);
            return true;
        });

    EXPECT_CALL(shared_state_ref.notification_, notify());

    // When calling the pop method with this expecation
    auto result = sync_queue.Pop(sleep_duration, st_token);

    // Then the returned result will not be empty , and the value is 1
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(push_value, result.value());
}

TEST(SynchronizedQueue, CallPushAndPopAtForNoneEmptyQueue)
{
    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This test check that SynchronizedQueue::Pop() would return the value "
                   "in the case of none empty queue");

    const std::int32_t max_queue_length = 5;
    const auto sleep_duration = 100ms;

    auto shared_state = std::make_shared<details::SharedState<std::int32_t, MockNotification>>(max_queue_length);

    auto& shared_state_ref = *shared_state;

    // Create synchonized queue with max queue length = 5
    SynchronizedQueue<std::int32_t, MockNotification> sync_queue(std::move(shared_state));

    // Create sender to queue
    auto sender = sync_queue.CreateSender();

    score::cpp::stop_source st_source;
    const score::cpp::stop_token st_token = st_source.get_token();
    const std::int32_t push_value_1 = 1;
    const std::int32_t push_value_2 = 2;

    EXPECT_CALL(shared_state_ref.notification_, waitForWithAbort(::testing::_, ::testing::_))
        .WillOnce([&sender, push_value_1, push_value_2]() {
            sender.push(push_value_1);
            sender.push(push_value_2);
            return true;
        });

    EXPECT_CALL(shared_state_ref.notification_, notify()).Times(2);

    // When calling the pop method with this expecation
    auto first_push_result = sync_queue.Pop(sleep_duration, st_token);

    // Then the value returned will not be empty , and the value will be equal to 1
    EXPECT_TRUE(first_push_result.has_value());
    EXPECT_EQ(push_value_1, first_push_result.value());

    // When calling the pop method with this expecation
    auto second_push_result = sync_queue.Pop(sleep_duration, st_token);
    // Then the value returned will not be empty , and the value will be equal to 2
    EXPECT_TRUE(second_push_result.has_value());
    EXPECT_EQ(push_value_2, second_push_result.value());
}

TEST(SynchronizedQueue, MakeStressTestForPushingFromMultipleThreads)
{
    constexpr std::size_t num_threads = 3;
    constexpr std::size_t num_values_per_thread = 100;
    const std::size_t max_queue_length = num_threads * num_values_per_thread;

    RecordProperty("ASIL", "QM");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "Analysis of boundary values");
    RecordProperty("Verifies", "::score::platform::aas::lib::concurrency::SynchronizedQueue");
    RecordProperty("Description",
                   "This is a smoke test where test 3 threads are created and each of them pushes 100 values "
                   "into the queue. "
                   "Test checks that all the values are in the queue.");

    const auto sleep_duration = 100ms;
    // Given queue with size = 300 elements
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
                EXPECT_TRUE(sender.push(val));
            }
        });
    }

    // Create thread for queue reader
    std::thread reader_thread([&st_token, &sleep_duration, &sync_queue]() noexcept {
        std::set<std::int32_t> set;
        std::optional<std::int32_t> result(0);
        result = 0;

        EXPECT_TRUE(result.has_value());

        while (set.size() < max_queue_length)
        {
            result = sync_queue.Pop(sleep_duration, st_token);
            if (result.has_value())
            {
                set.insert(result.value());
            }
        }

        std::int32_t previous_value = 0;
        std::int32_t tmp = 0;

        for (const auto& element : set)
        {
            // Then there are all the elements pished to the queue
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
