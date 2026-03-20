/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#include "score/shm/vector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

namespace score::shm
{
namespace
{

using ::testing::Return;

class MockMemoryResource final : public MemoryResource
{
  public:
    MOCK_METHOD(void*, allocate, (std::size_t, std::size_t), (noexcept, override));
    MOCK_METHOD(void, deallocate, (void*, std::size_t, std::size_t), (noexcept, override));
    MOCK_METHOD(bool, is_equal, (const MemoryResource&), (const, noexcept, override));
};

TEST(Vector, CreateWithZeroCapacityStartsEmpty)
{
    auto created = Vector<int>::CreateWithCapacity(0U);
    ASSERT_TRUE(created.has_value());

    Vector<int> vector = std::move(created).value();
    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_EQ(vector.capacity(), 0U);
}

TEST(Vector, CreateWithCountDefaultConstructsElements)
{
    auto created = Vector<int>::Create(3U);
    ASSERT_TRUE(created.has_value());

    Vector<int> vector = std::move(created).value();
    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 0);
    EXPECT_EQ(vector.at(1U), 0);
    EXPECT_EQ(vector.at(2U), 0);
}

TEST(Vector, CreateWithCountAndValueFillsElements)
{
    auto created = Vector<int>::Create(3U, 42);
    ASSERT_TRUE(created.has_value());

    Vector<int> vector = std::move(created).value();
    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 42);
    EXPECT_EQ(vector.at(1U), 42);
    EXPECT_EQ(vector.at(2U), 42);
}

TEST(Vector, CreateFromIteratorRange)
{
    const int source[] = {10, 20, 30};
    auto created = Vector<int>::Create(std::begin(source), std::end(source));
    ASSERT_TRUE(created.has_value());

    Vector<int> vector = std::move(created).value();
    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 10);
    EXPECT_EQ(vector.at(1U), 20);
    EXPECT_EQ(vector.at(2U), 30);
}

TEST(Vector, CreateFromInitializerList)
{
    auto created = Vector<int>::Create({5, 6, 7, 8});
    ASSERT_TRUE(created.has_value());

    Vector<int> vector = std::move(created).value();
    ASSERT_EQ(vector.size(), 4U);
    EXPECT_EQ(vector.at(0U), 5);
    EXPECT_EQ(vector.at(1U), 6);
    EXPECT_EQ(vector.at(2U), 7);
    EXPECT_EQ(vector.at(3U), 8);
}

TEST(Vector, PushBackAndElementAccessWork)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);

    EXPECT_TRUE(vector.PushBack(7).has_value());
    EXPECT_TRUE(vector.PushBack(11).has_value());

    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 7);
    EXPECT_EQ(vector.at(1U), 11);
    EXPECT_EQ(vector.front(), 7);
    EXPECT_EQ(vector.back(), 11);
}

TEST(VectorDeathTest, AtAbortsOnOutOfRange)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.at(0U), "");
}

TEST(VectorDeathTest, FrontAbortsOnEmpty)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.front(), "");
}

TEST(VectorDeathTest, BackAbortsOnEmpty)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.back(), "");
}

TEST(Vector, InsertAddsElementAtPosition)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, 2);
    ASSERT_TRUE(insert_result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(Vector, BeginEndProvideContiguousIteration)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    int sum = 0;
    for (auto it = vector.begin(); it != vector.end(); ++it)
    {
        sum += *it;
    }

    EXPECT_EQ(sum, 6);
}

TEST(Vector, CloneCreatesIndependentCopy)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(5).has_value());
    EXPECT_TRUE(vector.PushBack(6).has_value());

    auto clone_result = vector.Clone();
    ASSERT_TRUE(clone_result.has_value());

    Vector<int> clone = std::move(clone_result).value();
    EXPECT_EQ(clone.size(), 2U);
    EXPECT_EQ(clone.at(0U), 5);
    EXPECT_EQ(clone.at(1U), 6);

    EXPECT_TRUE(clone.PushBack(7).has_value());
    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(clone.size(), 3U);
}

TEST(Vector, SwapExchangesContents)
{
    Vector<int> first = Vector<int>::CreateWithCapacityOrAbort(1U);
    Vector<int> second = Vector<int>::CreateWithCapacityOrAbort(1U);

    EXPECT_TRUE(first.PushBack(10).has_value());
    EXPECT_TRUE(second.PushBack(20).has_value());

    first.swap(second);

    EXPECT_EQ(first.front(), 20);
    EXPECT_EQ(second.front(), 10);
}

TEST(Vector, CreatePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto created = Vector<int, PolymorphicAllocator<int>>::CreateWithCapacity(4U, allocator);
    ASSERT_FALSE(created.has_value());
    EXPECT_EQ(created.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Vector, ReservePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};

    Vector<int, PolymorphicAllocator<int>> vector =
        Vector<int, PolymorphicAllocator<int>>::CreateWithCapacityOrAbort(0U, allocator);

    EXPECT_CALL(mock_resource, allocate(10U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto reserve_result = vector.Reserve(10U);
    ASSERT_FALSE(reserve_result.has_value());
    EXPECT_EQ(reserve_result.error(), ContainerErrorCode::kOutOfMemory);
}

struct CountingLifecycleType
{
    static inline std::size_t constructor_count{0U};
    static inline std::size_t destructor_count{0U};

    explicit CountingLifecycleType(int value_in) noexcept : value{value_in}
    {
        ++constructor_count;
    }

    CountingLifecycleType(const CountingLifecycleType& other) noexcept : value{other.value}
    {
        ++constructor_count;
    }

    CountingLifecycleType(CountingLifecycleType&& other) noexcept : value{other.value}
    {
        ++constructor_count;
        other.value = -1;
    }

    CountingLifecycleType& operator=(const CountingLifecycleType&) = default;
    CountingLifecycleType& operator=(CountingLifecycleType&&) = default;

    ~CountingLifecycleType() noexcept
    {
        ++destructor_count;
    }

    int value{0};
};

TEST(Vector, NonTrivialTypeElementsAreDestroyed)
{
    CountingLifecycleType::constructor_count = 0U;
    CountingLifecycleType::destructor_count = 0U;

    {
        Vector<CountingLifecycleType> vector = Vector<CountingLifecycleType>::CreateWithCapacityOrAbort(1U);
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{1}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{2}).has_value());
        EXPECT_TRUE(vector.Insert(1U, CountingLifecycleType{3}).has_value());
        EXPECT_EQ(vector.size(), 3U);
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, MoveOnlyTypeCanBePushedAndInserted)
{
    using MoveOnlyInt = std::unique_ptr<int>;
    Vector<MoveOnlyInt> vector = Vector<MoveOnlyInt>::CreateWithCapacityOrAbort(0U);

    EXPECT_TRUE(vector.PushBack(std::make_unique<int>(10)).has_value());
    EXPECT_TRUE(vector.PushBack(std::make_unique<int>(30)).has_value());
    EXPECT_TRUE(vector.Insert(1U, std::make_unique<int>(20)).has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(*vector.at(0U), 10);
    EXPECT_EQ(*vector.at(1U), 20);
    EXPECT_EQ(*vector.at(2U), 30);
}

TEST(Vector, EmplaceBackConstructsInPlace)
{
    Vector<std::pair<int, int>> vector = Vector<std::pair<int, int>>::CreateWithCapacityOrAbort(2U);

    EXPECT_TRUE(vector.EmplaceBack(1, 2).has_value());
    EXPECT_TRUE(vector.EmplaceBack(3, 4).has_value());

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U).first, 1);
    EXPECT_EQ(vector.at(0U).second, 2);
    EXPECT_EQ(vector.at(1U).first, 3);
    EXPECT_EQ(vector.at(1U).second, 4);
}

TEST(Vector, EmplaceBackGrowsWhenFull)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    EXPECT_TRUE(vector.EmplaceBack(1).has_value());
    EXPECT_TRUE(vector.EmplaceBack(2).has_value());

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_GE(vector.capacity(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
}

TEST(Vector, PopBackRemovesLastElement)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    vector.pop_back();
    ASSERT_EQ(vector.size(), 1U);
    EXPECT_EQ(vector.at(0U), 1);
}

TEST(VectorDeathTest, PopBackAbortsOnEmpty)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.pop_back(), "");
}

TEST(Vector, PopBackDestroysElement)
{
    CountingLifecycleType::constructor_count = 0U;
    CountingLifecycleType::destructor_count = 0U;

    {
        Vector<CountingLifecycleType> vector = Vector<CountingLifecycleType>::CreateWithCapacityOrAbort(2U);
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{1}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{2}).has_value());
        vector.pop_back();
        ASSERT_EQ(vector.size(), 1U);
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, EraseRemovesElementAtIndex)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    vector.erase(1U);
    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 3);
}

TEST(VectorDeathTest, EraseAbortsOnOutOfRange)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_DEATH(vector.erase(1U), "");
}

TEST(Vector, EraseDestroysElements)
{
    CountingLifecycleType::constructor_count = 0U;
    CountingLifecycleType::destructor_count = 0U;

    {
        Vector<CountingLifecycleType> vector = Vector<CountingLifecycleType>::CreateWithCapacityOrAbort(3U);
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{1}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{2}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{3}).has_value());
        vector.erase(1U);
        ASSERT_EQ(vector.size(), 2U);
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, ClearRemovesAllElements)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    const auto capacity_before = vector.capacity();
    vector.clear();

    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_EQ(vector.capacity(), capacity_before);
}

TEST(Vector, ClearDestroysElements)
{
    CountingLifecycleType::constructor_count = 0U;
    CountingLifecycleType::destructor_count = 0U;

    {
        Vector<CountingLifecycleType> vector = Vector<CountingLifecycleType>::CreateWithCapacityOrAbort(2U);
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{1}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{2}).has_value());
        vector.clear();
        EXPECT_TRUE(vector.empty());
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, ResizeGrowsWithDefaultConstructedElements)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);

    auto result = vector.Resize(3U);
    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 0);
    EXPECT_EQ(vector.at(1U), 0);
    EXPECT_EQ(vector.at(2U), 0);
}

TEST(Vector, ResizeGrowsWithFillValue)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);

    auto result = vector.Resize(3U, 42);
    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 42);
    EXPECT_EQ(vector.at(1U), 42);
    EXPECT_EQ(vector.at(2U), 42);
}

TEST(Vector, ResizeShrinksAndDestroysElements)
{
    CountingLifecycleType::constructor_count = 0U;
    CountingLifecycleType::destructor_count = 0U;

    {
        Vector<CountingLifecycleType> vector = Vector<CountingLifecycleType>::CreateWithCapacityOrAbort(3U);
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{1}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{2}).has_value());
        EXPECT_TRUE(vector.PushBack(CountingLifecycleType{3}).has_value());

        EXPECT_TRUE(vector.Resize(1U).has_value());
        ASSERT_EQ(vector.size(), 1U);
        EXPECT_EQ(vector.at(0U).value, 1);
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, ResizeToSameSizeIsNoop)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    EXPECT_TRUE(vector.Resize(2U).has_value());
    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
}

TEST(Vector, ShrinkToFitReducesCapacity)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(10U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_EQ(vector.capacity(), 10U);

    auto result = vector.ShrinkToFit();
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(vector.capacity(), 2U);
    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
}

TEST(Vector, ShrinkToFitOnEmptyDeallocates)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(10U);
    EXPECT_EQ(vector.capacity(), 10U);

    EXPECT_TRUE(vector.ShrinkToFit().has_value());
    EXPECT_EQ(vector.capacity(), 0U);
    EXPECT_EQ(vector.size(), 0U);
}

TEST(Vector, ShrinkToFitWhenFullIsNoop)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    EXPECT_TRUE(vector.ShrinkToFit().has_value());
    EXPECT_EQ(vector.capacity(), 2U);
    EXPECT_EQ(vector.size(), 2U);
}

TEST(Vector, CreateWithBufferUsesExternalStorage)
{
    alignas(int) std::byte buffer[sizeof(int) * 4U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    EXPECT_TRUE(vector.has_fixed_capacity());
    EXPECT_EQ(vector.capacity(), 4U);
    EXPECT_EQ(vector.size(), 0U);

    EXPECT_TRUE(vector.PushBack(10).has_value());
    EXPECT_TRUE(vector.PushBack(20).has_value());
    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 10);
    EXPECT_EQ(vector.at(1U), 20);
}

TEST(Vector, CreateWithBufferRejectsGrowthBeyondCapacity)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    auto result = vector.PushBack(3);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
    EXPECT_EQ(vector.size(), 2U);
}

TEST(Vector, CreateWithBufferReserveRejectsGrowth)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    EXPECT_TRUE(vector.Reserve(2U).has_value());

    auto result = vector.Reserve(3U);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Vector, CreateWithBufferShrinkToFitIsNoop)
{
    alignas(int) std::byte buffer[sizeof(int) * 4U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    EXPECT_TRUE(vector.PushBack(1).has_value());

    EXPECT_TRUE(vector.ShrinkToFit().has_value());
    EXPECT_EQ(vector.capacity(), 4U);
}

TEST(Vector, CreateWithBufferDoesNotDeallocateOnDestruction)
{
    alignas(int) std::byte buffer[sizeof(int) * 4U]{};
    {
        Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
        EXPECT_TRUE(vector.PushBack(42).has_value());
    }
    // If deallocate were called on the stack buffer, this would crash.
    // Reaching here means the destructor correctly skipped deallocation.
}

}  // namespace
}  // namespace score::shm
