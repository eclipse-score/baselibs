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
#include "score/mw/log/detail/wait_free_stack/wait_free_stack.h"
#include "score/concurrency/atomic/atomic_mock.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <thread>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

using ::testing::_;
using ::testing::Return;

TEST(WaitFreeStack, AtomicOperationFetchAddForWriteIndex)
{
    RecordProperty("Requirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check atomic fetch_add function for write_index_.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    using AtomicType = std::size_t;

    std::unique_ptr<score::concurrency::atomic::AtomicMock<AtomicType>> atomic_mock{
        std::make_unique<score::concurrency::atomic::AtomicMock<AtomicType>>()};

    score::concurrency::atomic::AtomicIndirectorMock<AtomicType>::SetMockObject(atomic_mock.get());

    constexpr auto kStackSize = 1UL;
    WaitFreeStack<std::string, score::concurrency::atomic::AtomicIndirectorMock> stack{kStackSize};

    EXPECT_CALL(*atomic_mock, fetch_add(_, _)).WillOnce(Return(1));
    EXPECT_CALL(*atomic_mock, load(_)).Times(0);
    stack.TryPush(std::to_string(1));
}

TEST(WaitFreeStack, AtomicShallBeLockFree)
{
    RecordProperty("Requirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Check atomic lock-free.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    ASSERT_TRUE(
        (WaitFreeStack<std::string, score::concurrency::atomic::AtomicIndirectorReal>::AtomicIndex{}.is_always_lock_free));
    ASSERT_TRUE(
        (WaitFreeStack<std::string, score::concurrency::atomic::AtomicIndirectorReal>::AtomicBool{}.is_always_lock_free));
}

TEST(WaitFreeStack, ConcurrentPushingAndReadingShouldReturnExpectedElements)
{
    RecordProperty("Requirement", "SCR-861550");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Ensures that WaitFreeStack shall be capable of performing multiple "
                   "concurrent write operations without endless loops and return the correct data.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr auto kStackSize = 10UL;
    WaitFreeStack<std::string, score::concurrency::atomic::AtomicIndirectorReal> stack{kStackSize};

    // Start writer threads
    constexpr auto kNumberOfPushThreads = 32UL;
    std::vector<std::thread> push_threads;
    for (auto i = 0UL; i < kNumberOfPushThreads; ++i)
    {
        push_threads.emplace_back(std::thread([&stack, i]() {
            const auto result = stack.TryPush(std::to_string(i));
            if (result.has_value() == false)
            {
                return;
            }

            // Expect we get the same value as pushed.
            if (result.value().get() != std::to_string(i))
            {
                std::abort();
            }

            // Expect we find the same value as pushed.
            if (stack
                    .Find([i](const auto& item) {
                        return item == std::to_string(i);
                    })
                    .value()
                    .get() != std::to_string(i))
            {
                std::abort();
            }
        }));
    }

    // Start reader threads
    constexpr auto kNumberOfReadThreads = 16UL;
    std::vector<std::vector<std::string>> found_strings(kNumberOfReadThreads);
    std::vector<std::thread> read_threads;
    for (auto thread_index = 0UL; thread_index < kNumberOfReadThreads; thread_index++)
    {
        read_threads.emplace_back(std::thread([&stack, &found_strings, thread_index]() {
            auto& thread_result = found_strings[thread_index];
            while (thread_result.size() < kStackSize)
            {
                for (auto i = 0UL; i < kNumberOfPushThreads; ++i)
                {
                    if (std::find(thread_result.begin(), thread_result.end(), std::to_string(i)) != thread_result.end())
                    {
                        continue;
                    }

                    const auto result = stack.Find([i](const std::string& item) {
                        return item == std::to_string(i);
                    });
                    if (result.has_value())
                    {
                        thread_result.emplace_back(result.value());
                    }
                }
            }
        }));
    }

    // Wait for all threads to terminate.
    for (auto& thread : push_threads)
    {
        thread.join();
    }
    for (auto& thread : read_threads)
    {
        thread.join();
    }

    // Check that all threads found the same elements.
    for (auto thread_index = 0UL; thread_index < kNumberOfReadThreads; thread_index++)
    {
        std::sort(found_strings[thread_index].begin(), found_strings[thread_index].end());
    }
    for (auto i = 0UL; i < kStackSize; ++i)
    {
        for (auto thread_index = 0UL; thread_index < kNumberOfReadThreads; thread_index++)
        {
            ASSERT_EQ(found_strings[0UL][i], found_strings[thread_index][i]);
        }
    }
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
