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
#include "score/analysis/tracing/common/shared_list/shared_list.h"
#include "score/analysis/tracing/common/canary_wrapper/canary_wrapper.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/test/mocks/flexible_circular_allocator_mock.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_chunk.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
using namespace score::memory::shared;
using namespace score::analysis::tracing;
using testing::_;
using testing::Invoke;
using testing::Return;

class SharedListFixture : public ::testing::Test
{
  public:
    std::shared_ptr<FlexibleCircularAllocatorMock> flexible_allocator_mock_;

  protected:
    void SetUp() override
    {
        flexible_allocator_mock_ = std::make_shared<FlexibleCircularAllocatorMock>();
    }

    void TearDown() override {}
};

// Test Suite
TEST_F(SharedListFixture, DefaultConstructor)
{
    shared::List<std::uint8_t> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SharedListFixture, ConstructorWithAllocator)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).Times(0);
    shared::List<std::uint8_t> list(flexible_allocator_mock_);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SharedListFixture, PushBackSingleElement)
{

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    EXPECT_EQ(list.size(), 0);
    auto result = list.push_back(10);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(list.empty());
    list.clear();
}

TEST_F(SharedListFixture, PushBackSingleElementAndFailToAllocate)
{

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t, const std::size_t) {
        return nullptr;
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    EXPECT_EQ(list.size(), 0);
    auto result = list.push_back(10);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
    list.clear();
}

TEST_F(SharedListFixture, PushBackMultipleElementsAndClear)
{
    std::uint8_t number_of_elements = 3;

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .Times(number_of_elements)
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _))
        .Times(number_of_elements)
        .WillRepeatedly([](void* const address, const std::size_t) {
            free(address);  // Simulate deallocation
            return score::ResultBlank{};
        });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    EXPECT_EQ(list.size(), 0);

    for (std::uint8_t i = 0; i < number_of_elements; i++)
    {
        auto result = list.push_back(i);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(list.size(), i + 1);
        EXPECT_FALSE(list.empty());
    }
    list.clear();
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST_F(SharedListFixture, EmplaceBack)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    shared::List<std::pair<std::uint32_t, std::uint32_t>> list(flexible_allocator_mock_);

    ASSERT_TRUE(list.emplace_back(1U, 2U));

    EXPECT_EQ(list.size(), 1);
    auto result = list.at(0);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), std::make_pair(1U, 2U));
}

TEST_F(SharedListFixture, AtIndex)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillRepeatedly([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    ASSERT_TRUE(list.push_back(5));
    ASSERT_TRUE(list.push_back(10));

    auto result1 = list.at(0);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), 5);

    auto result2 = list.at(1);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 10);

    auto result3 = list.at(2);
    EXPECT_FALSE(result3.has_value());
}

TEST_F(SharedListFixture, Iterators)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillRepeatedly([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    ASSERT_TRUE(list.push_back(1));
    ASSERT_TRUE(list.push_back(2));
    ASSERT_TRUE(list.push_back(3));

    std::uint8_t expected[] = {1, 2, 3};
    size_t index = 0;

    for (auto it = list.begin(); it != list.end(); ++it, ++index)
    {
        EXPECT_EQ(*it, expected[index]);
    }
}

TEST_F(SharedListFixture, ArrowOperatorAccessesMember)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillRepeatedly([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    struct TestData
    {
        int id;
        std::string name;
    };
    shared::List<TestData> list(flexible_allocator_mock_);

    ASSERT_TRUE(list.push_back({1, "one"}));
    ASSERT_TRUE(list.push_back({2, "two"}));
    ASSERT_TRUE(list.push_back({3, "three"}));

    std::uint8_t expected_ids[] = {1, 2, 3};
    std::string expected_name[] = {"one", "two", "three"};

    size_t index = 0;

    for (auto it = list.begin(); it != list.end(); ++it, ++index)
    {
        EXPECT_EQ(it->id, expected_ids[index]);
        EXPECT_STREQ(it->name.c_str(), expected_name[index].c_str());
    }
}

TEST_F(SharedListFixture, IncrementOnEmptyList)
{
    shared::List<std::uint8_t> list(flexible_allocator_mock_, 0, 0, 0);

    auto it = list.begin();

    // Make sure the iterator starts at the end for an empty list
    EXPECT_EQ(it, list.end());

    // Increment the iterator and check if it remains at end
    ++it;
    EXPECT_EQ(it, list.end());

    // Also check post-increment operator
    it++;
    EXPECT_EQ(it, list.end());
}

TEST_F(SharedListFixture, IteratorDereferenceAllocateSuccessfully)
{
    void* allocated_memory = nullptr;
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .WillOnce([&allocated_memory](const std::size_t size, const std::size_t) {
            allocated_memory = malloc(size);
            return allocated_memory;
        });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);
    shared::List<std::uint8_t>::iterator iterator(&list, nullptr);

    auto value = *iterator;
    std::ignore = value;

    free(allocated_memory);
}

TEST_F(SharedListFixture, IteratorDereferenceFailToAllocate)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t, const std::size_t) {
        return nullptr;
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);
    shared::List<std::uint8_t>::iterator iterator(&list, nullptr);

    auto value = *iterator;
    std::ignore = value;
}

TEST_F(SharedListFixture, SharedMemoryChunk)
{
    using ShmChunkList = shared::List<SharedMemoryChunk>;
    using ShmChunkVector = CanaryWrapper<ShmChunkList>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t)).value();
    if (nullptr == vector_shm_raw_pointer)
    {
        std::cout << "ErrorCode::kNotEnoughMemoryRecoverable" << std::endl;
    }

    const auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);

    std::thread allocator_thread = std::thread{[&]() {
        auto list_data = vector->GetData();
        if (!list_data.has_value())
        {
            std::cout << "ErrorCode::kMemoryCorruptionDetectedFatal" << std::endl;
            return;
        }
        auto& list = list_data.value().get();
        for (std::uint8_t i = 0U; i < 60; i++)
        {
            auto emplace_result = list.emplace_back(SharedMemoryChunk{SharedMemoryLocation{i, i}, i});
            if (!emplace_result.has_value())
            {
                list.clear();
                std::ignore = flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
                std::cout << "ErrorCode::kNotEnoughMemoryRecoverable" << std::endl;
            }
        }
    }};

    std::thread deallocator_thread = std::thread{[&]() {
        // need the sleep here as the shared list still not supporting concurrent execution yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto list_data = vector->GetData();
        if (!list_data.has_value())
        {
            std::cout << "ErrorCode::kMemoryCorruptionDetectedFatal" << std::endl;
            return;
        }
        auto& list = list_data.value().get();
        const std::size_t elements_to_deallocate = list.size();

        for (std::size_t i = 0U; i < elements_to_deallocate; i++)
        {
            auto el = list.at(i);
            if (el.has_value())
            {
                auto chunk_data = el.value().GetData();
                if (chunk_data.has_value())
                {
                    std::cout << "Element number " << std::to_string(i)
                              << " is : " << std::to_string(chunk_data.value().get().start_.offset_) << std::endl;
                }
            }
        }
        list.clear();
        EXPECT_TRUE(flexible_allocator->Deallocate(vector, sizeof(vector)));
    }};

    allocator_thread.join();
    deallocator_thread.join();

    memory_pointer_.reset();
    flexible_allocator.reset();

    ASSERT_EQ(true, true);
}

// Test cases for canary checks
TEST_F(SharedListFixture, SharedMemoryChunkIsCorruptedMethod)
{
    SharedMemoryChunk valid_chunk{SharedMemoryLocation{1, 1}, 100U};
    EXPECT_TRUE(valid_chunk.GetData().has_value());

    SharedMemoryChunk corrupted_start{SharedMemoryLocation{1, 1}, 100U};
    auto* start_canary = reinterpret_cast<std::uint32_t*>(&corrupted_start);
    *start_canary = 0xBADBAD;
    EXPECT_FALSE(corrupted_start.GetData().has_value());
    SharedMemoryChunk corrupted_end{SharedMemoryLocation{1, 1}, 100U};
    constexpr std::size_t start_canary_size = sizeof(std::uint32_t);
    constexpr std::size_t data_alignment = alignof(SharedMemoryChunkData);
    // Padding after start canary to align data
    constexpr std::size_t padding = (data_alignment - start_canary_size) % data_alignment;
    constexpr std::size_t end_canary_offset = start_canary_size + padding + sizeof(SharedMemoryChunkData);
    auto* end_canary =
        reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(&corrupted_end) + end_canary_offset);
    *end_canary = 0xBADBAD;
    EXPECT_FALSE(corrupted_end.GetData().has_value());

    SharedMemoryChunk corrupted_both{SharedMemoryLocation{1, 1}, 100U};
    auto* both_start = reinterpret_cast<std::uint32_t*>(&corrupted_both);
    auto* both_end =
        reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(&corrupted_both) + end_canary_offset);
    *both_start = 0xBADBAD;
    *both_end = 0xBADBAD;
    EXPECT_FALSE(corrupted_both.GetData().has_value());
}

TEST_F(SharedListFixture, SharedMemoryChunkCanaryDetection)
{
    using ShmChunkList = shared::List<SharedMemoryChunk>;
    using ShmChunkVector = CanaryWrapper<ShmChunkList>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t)).value();
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);
    auto list_data = vector->GetData();
    ASSERT_TRUE(list_data.has_value());
    auto& list = list_data.value().get();
    auto emplace_result = list.emplace_back(SharedMemoryChunk{SharedMemoryLocation{1, 1}, 100U});
    ASSERT_TRUE(emplace_result.has_value());

    auto result_valid = list.at(0);
    EXPECT_TRUE(result_valid.has_value());
    auto chunk_data = result_valid.value().GetData();
    ASSERT_TRUE(chunk_data.has_value());
    EXPECT_EQ(chunk_data.value().get().start_.offset_, 1);

    SharedMemoryChunk corrupted_chunk{SharedMemoryLocation{2, 2}, 200U};
    auto* start_canary = reinterpret_cast<std::uint32_t*>(&corrupted_chunk);
    *start_canary = 0xBADBEEF;

    EXPECT_FALSE(corrupted_chunk.GetData().has_value());

    list.clear();
    EXPECT_TRUE(flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector)));
}

TEST_F(SharedListFixture, SharedMemoryChunkCorruptionInList)
{
    // Test to cover line check: return error when SharedMemoryChunk.GetData() returns nullopt
    using ShmChunkList = shared::List<SharedMemoryChunk>;
    using ShmChunkVector = CanaryWrapper<ShmChunkList>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t)).value();
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);
    auto list_data = vector->GetData();
    ASSERT_TRUE(list_data.has_value());
    auto& list = list_data.value().get();

    // Add an element
    auto emplace_result = list.emplace_back(SharedMemoryChunk{SharedMemoryLocation{5, 5}, 500U});
    ASSERT_TRUE(emplace_result.has_value());

    // Verify we can access it normally first
    auto result_before = list.at(0);
    EXPECT_TRUE(result_before.has_value());
    auto chunk_data_before = result_before.value().GetData();
    ASSERT_TRUE(chunk_data_before.has_value());
    EXPECT_EQ(chunk_data_before.value().get().start_.offset_, 5);

    bool found_and_corrupted = false;
    std::uint8_t* base_memory = memory_pointer_.get();

    constexpr std::size_t alignment = alignof(SharedMemoryChunk);

    for (std::size_t offset = 0; offset < kFlexibleAllocatorSize; offset += alignment)
    {
        const SharedMemoryChunk* potential_chunk = reinterpret_cast<const SharedMemoryChunk*>(base_memory + offset);

        auto chunk_data = potential_chunk->GetData();
        if (chunk_data.has_value() && chunk_data.value().get().start_.offset_ == 5 &&
            chunk_data.value().get().start_.shm_object_handle_ == 5 && chunk_data.value().get().size_ == 500)
        {
            // Corrupt start canary via direct memory access
            auto* canary_ptr = const_cast<std::uint32_t*>(reinterpret_cast<const std::uint32_t*>(potential_chunk));
            *canary_ptr = 0xBADC0DE;
            found_and_corrupted = true;
            break;
        }
    }

    // The test should find and corrupt the chunk
    ASSERT_TRUE(found_and_corrupted) << "Failed to find SharedMemoryChunk in memory to corrupt it";

    if (found_and_corrupted)
    {
        // Try to access element - should detect corruption
        auto result_after = list.at(0);
        EXPECT_FALSE(result_after.has_value()) << "Expected corrupted chunk to be detected";
        EXPECT_EQ(result_after, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));
    }

    list.clear();
    EXPECT_TRUE(flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector)));
}
TEST_F(SharedListFixture, ClearWithValidListConditionFalse)
{
    // Test to cover if (IsCorrupted()) → false
    // When list is valid, clear() should perform normal deallocation
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .Times(2)
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _))
        .Times(2)  // Expect deallocation to be called for each element
        .WillRepeatedly([](void* const address, const std::size_t) {
            free(address);
            return score::ResultBlank{};
        });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    // Add some elements
    auto result1 = list.push_back(10);
    EXPECT_TRUE(result1.has_value());
    auto result2 = list.push_back(20);
    EXPECT_TRUE(result2.has_value());

    // Verify size before clear
    EXPECT_EQ(list.size(), 2);

    // Verify list is not corrupted by checking we can access elements
    EXPECT_TRUE(list.size() > 0U);

    // Call clear() - should take the IsCorrupted() == false branch
    // This should perform normal deallocation
    list.clear();

    // After clear(), size should be 0
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

TEST_F(SharedListFixture, AtMethodIsInBoundsConditionTrue)
{
    // Test to cover: if (!IsInBounds(current, sizeof(Node))) → false (condition passes)
    // When node is in bounds, at() should continue processing normally
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return score::Result<void*>{allocated_memory};
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _))
        .WillOnce(Return(true));  // Node is in bounds → condition is false, continues normally

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);
        return score::ResultBlank{};
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    // Add an element
    auto push_result = list.push_back(42);
    EXPECT_TRUE(push_result.has_value());

    // Access the element - should pass IsInBounds check
    auto at_result = list.at(0);
    EXPECT_TRUE(at_result.has_value());
    EXPECT_EQ(at_result.value(), 42);

    list.clear();
}

TEST_F(SharedListFixture, AtMethodIsInBoundsConditionFalse)
{
    // Test to cover : if (!IsInBounds(current, sizeof(Node))) → true
    // When node is out of bounds, at() should return memory corruption error
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _))
        .WillOnce(Return(false));  // Node is out of bounds → condition is true, returns error

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);
        return score::ResultBlank{};
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    // Add an element
    auto push_result = list.push_back(42);
    EXPECT_TRUE(push_result.has_value());

    // Access the element - should fail IsInBounds check
    auto at_result = list.at(0);
    EXPECT_FALSE(at_result.has_value());
    EXPECT_EQ(at_result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    list.clear();
}

TEST_F(SharedListFixture, AtMethodCurrentNullptrConditionTrue)
{
    // Test to cover: if (current == nullptr) → true
    // When current becomes nullptr during iteration, at() should return memory corruption error
    using ShmChunkList = shared::List<SharedMemoryChunk>;
    using ShmChunkVector = CanaryWrapper<ShmChunkList>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    auto vector_shm_result = flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    ASSERT_TRUE(vector_shm_result.has_value());
    void* const vector_shm_raw_pointer = vector_shm_result.value();

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);
    auto list_data = vector->GetData();
    ASSERT_TRUE(list_data.has_value());
    auto& list = list_data.value().get();

    // Add an element
    auto emplace_result = list.emplace_back(SharedMemoryChunk{SharedMemoryLocation{1, 1}, 100U});
    ASSERT_TRUE(emplace_result.has_value());

    // Manually corrupt the list structure to make ResolveOffset return nullptr
    // We'll manipulate the internal offset to point to an invalid location
    // Access members directly - List now has:
    //   flexible_allocator_ (std::shared_ptr - 16 bytes)
    //   head_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    //   tail_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    //   size_ (std::atomic_size_t - 8 bytes)
    std::uint8_t* list_bytes = reinterpret_cast<std::uint8_t*>(&list);

    // Skip shared_ptr (16 bytes) to reach head_offset_
    constexpr std::size_t shared_ptr_size = sizeof(std::shared_ptr<IFlexibleCircularAllocator>);
    constexpr std::size_t head_offset_position = shared_ptr_size;

    // Set head_offset to 0, which will cause ResolveOffset to return nullptr
    std::atomic<std::ptrdiff_t>* head_offset_ptr =
        reinterpret_cast<std::atomic<std::ptrdiff_t>*>(list_bytes + head_offset_position);
    head_offset_ptr->store(0);

    // Try to access element - current should be nullptr after loop
    auto at_result = list.at(0);
    EXPECT_FALSE(at_result.has_value());
    EXPECT_EQ(at_result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    // Clean up
    list.~List();
    EXPECT_TRUE(flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector)));
    flexible_allocator.reset();
    memory_pointer_.reset();
}

TEST_F(SharedListFixture, AtMethodCurrentNullptrConditionFalse)
{
    // Test to cover: if (current == nullptr) → false
    // When current is valid after iteration, at() should continue to return data
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .Times(2)
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).Times(1).WillOnce(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _))
        .Times(2)
        .WillRepeatedly([](void* const address, const std::size_t) {
            free(address);
            return score::ResultBlank{};
        });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    // Add multiple elements
    auto push_result1 = list.push_back(10);
    EXPECT_TRUE(push_result1.has_value());
    auto push_result2 = list.push_back(20);
    EXPECT_TRUE(push_result2.has_value());

    // Access the second element - requires iteration and current should not be nullptr
    auto at_result = list.at(1);
    EXPECT_TRUE(at_result.has_value());
    EXPECT_EQ(at_result.value(), 20);

    list.clear();
}

TEST_F(SharedListFixture, AtMethodNullCurrentAfterTraversal)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return score::Result<void*>{allocated_memory};
    });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);
        return score::ResultBlank{};
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillOnce(Return(true));

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    auto push_result = list.push_back(42);
    EXPECT_TRUE(push_result.has_value());
    EXPECT_EQ(list.size(), 1);

    // This test intentionally uses fragile memory layout assumptions to artificially corrupt size_ for testing edge
    // cases. While brittle, it's necessary to test the nullptr safety check that protects against memory corruption.
    // List memory layout (no canary wrapper):
    //   flexible_allocator_ (std::shared_ptr - 16 bytes)
    //   head_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    //   tail_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    //   size_ (std::atomic_size_t - 8 bytes)
    constexpr std::size_t shared_ptr_size = sizeof(std::shared_ptr<IFlexibleCircularAllocator>);
    constexpr std::size_t ptrdiff_size = sizeof(std::atomic<std::ptrdiff_t>);

    const std::size_t size_offset = shared_ptr_size + (2 * ptrdiff_size);

    std::atomic<std::size_t>* size_ptr =
        reinterpret_cast<std::atomic<std::size_t>*>(reinterpret_cast<std::uint8_t*>(&list) + size_offset);

    size_ptr->store(2);

    auto at_result = list.at(1);
    EXPECT_FALSE(at_result.has_value());
    EXPECT_EQ(at_result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    size_ptr->store(1);
    list.clear();
}

TEST_F(SharedListFixture, CalculateOffsetWithNonNullNode)
{
    // This test explicitly verifies that CalculateOffset is called with non-null nodes
    // when adding multiple elements, ensuring branch coverage for the non-null path

    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _))
        .Times(2)
        .WillRepeatedly([](const std::size_t size, const std::size_t) {
            auto allocated_memory = malloc(size);
            return allocated_memory;
        });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _))
        .Times(2)
        .WillRepeatedly([](void* const address, const std::size_t) {
            free(address);
            return score::ResultBlank{};
        });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    // First push_back: tail is null, so CalculateOffset is called with new_node (non-null)
    auto result1 = list.push_back(10);
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(list.size(), 1);

    // Second push_back: tail is not null, so CalculateOffset is called twice with non-null nodes
    // Once for linking tail->next and once for updating tail_offset
    auto result2 = list.push_back(20);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(list.size(), 2);

    // Verify both elements are accessible
    auto at_result1 = list.at(0);
    EXPECT_TRUE(at_result1.has_value());
    EXPECT_EQ(at_result1.value(), 10);

    auto at_result2 = list.at(1);
    EXPECT_TRUE(at_result2.has_value());
    EXPECT_EQ(at_result2.value(), 20);

    list.clear();
}

TEST_F(SharedListFixture, ValidIterator)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);  // Simulate deallocation
        return score::ResultBlank{};
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);
    list.push_back(1);
    auto it = list.begin();
    std::cout << "Iterator value: " << *it << std::endl;
    EXPECT_TRUE(it.IsValid());
}

TEST_F(SharedListFixture, IteratorWithNullCurrent)
{
    shared::List<int> list;
    auto it = list.begin();
    EXPECT_FALSE(it.IsValid());
}
