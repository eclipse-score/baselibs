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
#include "score/analysis/tracing/common/flexible_circular_allocator/lockless_flexible_circular_allocator.h"
#include "score/analysis/tracing/common/flexible_circular_allocator/test/mocks/flexible_circular_allocator_mock.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_chunk.h"
#include "score/analysis/tracing/common/interface_types/shared_memory_location.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string>
#include <thread>
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
        return true;
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
            return true;
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
        return true;
    });

    shared::List<std::pair<std::uint32_t, std::uint32_t>> list(flexible_allocator_mock_);

    list.emplace_back(1U, 2U);

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
        return true;
    });

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    list.push_back(5);
    list.push_back(10);

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
        return true;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

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
        return true;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillRepeatedly(Return(true));

    struct TestData
    {
        int id;
        std::string name;
    };
    shared::List<TestData> list(flexible_allocator_mock_);

    list.push_back({1, "one"});
    list.push_back({2, "two"});
    list.push_back({3, "three"});

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
    score::cpp::ignore = value;

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
    score::cpp::ignore = value;
}

TEST_F(SharedListFixture, SharedMemoryChunk)
{
    using ShmChunkVector = shared::List<SharedMemoryChunk>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    if (nullptr == vector_shm_raw_pointer)
    {
        std::cout << "ErrorCode::kNotEnoughMemoryRecoverable" << std::endl;
    }

    const auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);

    std::thread allocator_thread = std::thread{[&]() {
        for (std::uint8_t i = 0U; i < 60; i++)
        {
            auto emplace_result = vector->emplace_back(SharedMemoryChunk{SharedMemoryLocation{i, i}, i});
            if (!emplace_result.has_value())
            {
                vector->clear();
                score::cpp::ignore = flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
                std::cout << "ErrorCode::kNotEnoughMemoryRecoverable" << std::endl;
            }
        }
    }};

    std::thread deallocator_thread = std::thread{[&]() {
        // need the sleep here as the shared list still not supporting concurrent execution yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        const std::size_t elements_to_deallocate = vector->size();

        for (std::size_t i = 0U; i < elements_to_deallocate; i++)
        {
            auto el = vector->at(i);
            if (el.has_value())
            {
                std::cout << "Element number " << std::to_string(i)
                          << " is : " << std::to_string(el.value().start_.offset_) << std::endl;
            }
        }
        vector->clear();
        flexible_allocator->Deallocate(vector, sizeof(vector));
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
    SharedMemoryChunk valid_chunk{SharedMemoryLocation{1, 1}, 100};
    EXPECT_FALSE(valid_chunk.IsCorrupted());

    SharedMemoryChunk corrupted_start{SharedMemoryLocation{1, 1}, 100};
    corrupted_start.canary_start_ = 0xBADBAD;
    EXPECT_TRUE(corrupted_start.IsCorrupted());

    SharedMemoryChunk corrupted_end{SharedMemoryLocation{1, 1}, 100};
    corrupted_end.canary_end_ = 0xBADBAD;
    EXPECT_TRUE(corrupted_end.IsCorrupted());

    SharedMemoryChunk corrupted_both{SharedMemoryLocation{1, 1}, 100};
    corrupted_both.canary_start_ = 0xBADBAD;
    corrupted_both.canary_end_ = 0xBADBAD;
    EXPECT_TRUE(corrupted_both.IsCorrupted());
}

TEST_F(SharedListFixture, ListMagicNumberCorruptionSimulated)
{
    using ShmChunkVector = shared::List<SharedMemoryChunk>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);

    auto emplace_result = vector->emplace_back(SharedMemoryChunk{SharedMemoryLocation{1, 1}, 100});
    ASSERT_TRUE(emplace_result.has_value());

    std::uint64_t* start_canary_ptr = reinterpret_cast<std::uint64_t*>(vector);
    const std::uint64_t original_start = *start_canary_ptr;
    *start_canary_ptr = 0xBADBADBADBADBAD;

    auto result = vector->at(0);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    *start_canary_ptr = original_start;
    vector->clear();
    flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
}

TEST_F(SharedListFixture, SharedMemoryChunkCanaryDetection)
{
    using ShmChunkVector = shared::List<SharedMemoryChunk>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);
    auto emplace_result = vector->emplace_back(SharedMemoryChunk{SharedMemoryLocation{1, 1}, 100});
    ASSERT_TRUE(emplace_result.has_value());

    auto result_valid = vector->at(0);
    EXPECT_TRUE(result_valid.has_value());
    EXPECT_EQ(result_valid.value().start_.offset_, 1);

    std::uint64_t* list_start_canary_ptr = reinterpret_cast<std::uint64_t*>(vector);
    const std::uint64_t original_list_canary = *list_start_canary_ptr;
    *list_start_canary_ptr = 0xBADBADBADBADBAD;

    vector->clear();

    *list_start_canary_ptr = original_list_canary;

    SharedMemoryChunk corrupted_chunk{SharedMemoryLocation{2, 2}, 200};
    corrupted_chunk.canary_start_ = 0xBADBEEF;
    EXPECT_TRUE(corrupted_chunk.IsCorrupted());

    vector->clear();
    flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
}

TEST_F(SharedListFixture, ListEndCanaryCorruption)
{
    using ShmChunkVector = shared::List<SharedMemoryChunk>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);

    // Add an element
    auto emplace_result = vector->emplace_back(SharedMemoryChunk{SharedMemoryLocation{1, 1}, 100});
    ASSERT_TRUE(emplace_result.has_value());

    // The List structure in memory order is:
    // canary_start_ (8 bytes)
    // flexible_allocator_ (std::shared_ptr - 16 bytes)
    // head_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    // tail_offset_ (std::atomic<std::ptrdiff_t> - 8 bytes)
    // size_ (std::atomic_size_t - 8 bytes)
    // canary_end_ (8 bytes)
    // Total: 56 bytes, but aligned to std::max_align_t

    constexpr std::size_t canary_size = sizeof(std::uint64_t);
    constexpr std::size_t atomic_size = sizeof(std::atomic_size_t);
    constexpr std::size_t shared_ptr_size = sizeof(std::shared_ptr<IFlexibleCircularAllocator>);
    constexpr std::size_t ptrdiff_size = sizeof(std::atomic<std::ptrdiff_t>);

    // Calculate offset to canary_end_: skip start canary, shared_ptr, two ptrdiff_t, and size_t
    const std::size_t end_canary_offset = canary_size + shared_ptr_size + (2 * ptrdiff_size) + atomic_size;

    std::uint64_t* end_canary_ptr =
        reinterpret_cast<std::uint64_t*>(reinterpret_cast<std::uint8_t*>(vector) + end_canary_offset);
    const std::uint64_t original_end = *end_canary_ptr;
    *end_canary_ptr = 0xDEADDEADDEADDEAD;

    // Try to access element - should detect end canary corruption (line 170)
    auto result = vector->at(0);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    // Restore canary for cleanup
    *end_canary_ptr = original_end;
    vector->clear();
    flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
}

TEST_F(SharedListFixture, SharedMemoryChunkCorruptionInList)
{
    // Test to cover line 478 and 481: return error when SharedMemoryChunk.IsCorrupted() is true
    using ShmChunkVector = shared::List<SharedMemoryChunk>;
    static constexpr std::size_t kFlexibleAllocatorSize = 10000U;
    std::shared_ptr<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>> flexible_allocator;
    std::unique_ptr<std::uint8_t[]> memory_pointer_;

    memory_pointer_ = std::make_unique<std::uint8_t[]>(2 * kFlexibleAllocatorSize);
    flexible_allocator = std::make_shared<LocklessFlexibleCircularAllocator<AtomicIndirectorReal>>(
        memory_pointer_.get(), kFlexibleAllocatorSize);

    void* const vector_shm_raw_pointer =
        flexible_allocator->Allocate(sizeof(ShmChunkVector), alignof(std::max_align_t));
    ASSERT_NE(nullptr, vector_shm_raw_pointer);

    auto vector = new (vector_shm_raw_pointer) ShmChunkVector(flexible_allocator);

    // Add an element
    auto emplace_result = vector->emplace_back(SharedMemoryChunk{SharedMemoryLocation{5, 5}, 500});
    ASSERT_TRUE(emplace_result.has_value());

    // Verify we can access it normally first
    auto result_before = vector->at(0);
    EXPECT_TRUE(result_before.has_value());
    EXPECT_EQ(result_before.value().start_.offset_, 5);

    bool found_and_corrupted = false;
    std::uint8_t* base_memory = memory_pointer_.get();

    constexpr std::size_t alignment = alignof(SharedMemoryChunk);

    for (std::size_t offset = 0; offset < kFlexibleAllocatorSize; offset += alignment)
    {
        SharedMemoryChunk* potential_chunk = reinterpret_cast<SharedMemoryChunk*>(base_memory + offset);

        if (potential_chunk->canary_start_ == SharedMemoryChunk::kCanaryStart &&
            potential_chunk->canary_end_ == SharedMemoryChunk::kCanaryEnd && potential_chunk->start_.offset_ == 5 &&
            potential_chunk->start_.shm_object_handle_ == 5 && potential_chunk->size_ == 500)
        {
            potential_chunk->canary_start_ = 0xBADC0DE;
            found_and_corrupted = true;
            break;
        }
    }

    // The test should find and corrupt the chunk
    ASSERT_TRUE(found_and_corrupted) << "Failed to find SharedMemoryChunk in memory to corrupt it";

    if (found_and_corrupted)
    {
        // Try to access element - should detect corruption (line 478 and 481)
        auto result_after = vector->at(0);
        EXPECT_FALSE(result_after.has_value()) << "Expected corrupted chunk to be detected";
        EXPECT_EQ(result_after, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));
    }

    vector->clear();
    flexible_allocator->Deallocate(vector_shm_raw_pointer, sizeof(ShmChunkVector));
}

TEST_F(SharedListFixture, AtMethodOutOfBoundsDetection)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);
        return true;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillOnce(Return(false));
    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    auto push_result = list.push_back(42);
    EXPECT_TRUE(push_result.has_value());
    EXPECT_EQ(list.size(), 1);

    auto at_result = list.at(0);
    EXPECT_FALSE(at_result.has_value());
    EXPECT_EQ(at_result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    list.clear();
}

TEST_F(SharedListFixture, AtMethodNullCurrentAfterTraversal)
{
    EXPECT_CALL(*flexible_allocator_mock_, Allocate(_, _)).WillOnce([](const std::size_t size, const std::size_t) {
        auto allocated_memory = malloc(size);
        return allocated_memory;
    });

    EXPECT_CALL(*flexible_allocator_mock_, Deallocate(_, _)).WillOnce([](void* const address, const std::size_t) {
        free(address);
        return true;
    });

    EXPECT_CALL(*flexible_allocator_mock_, IsInBounds(_, _)).WillOnce(Return(true));

    shared::List<std::uint8_t> list(flexible_allocator_mock_);

    auto push_result = list.push_back(42);
    EXPECT_TRUE(push_result.has_value());
    EXPECT_EQ(list.size(), 1);

    // This test intentionally uses fragile memory layout assumptions to artificially corrupt size_ for testing edge
    // cases While brittle, it's necessary to test the nullptr safety check that protects against memory corruption
    constexpr std::size_t canary_size = sizeof(std::uint64_t);
    constexpr std::size_t shared_ptr_size = sizeof(std::shared_ptr<IFlexibleCircularAllocator>);
    constexpr std::size_t ptrdiff_size = sizeof(std::atomic<std::ptrdiff_t>);

    const std::size_t size_offset = canary_size + shared_ptr_size + (2 * ptrdiff_size);

    std::atomic<std::size_t>* size_ptr =
        reinterpret_cast<std::atomic<std::size_t>*>(reinterpret_cast<std::uint8_t*>(&list) + size_offset);

    size_ptr->store(2);

    auto at_result = list.at(1);
    EXPECT_FALSE(at_result.has_value());
    EXPECT_EQ(at_result, score::MakeUnexpected(ErrorCode::kMemoryCorruptionDetectedFatal));

    size_ptr->store(1);
    list.clear();
}
