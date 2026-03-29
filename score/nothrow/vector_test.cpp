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

#include "score/nothrow/vector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

namespace score::nothrow
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

class FailAfterFirstAllocationResource final : public MemoryResource
{
  public:
    void* allocate(std::size_t bytes, std::size_t alignment) noexcept override
    {
        if (allocation_count_++ > 0U)
        {
            return nullptr;
        }
        return GetNewDeleteResource()->allocate(bytes, alignment);
    }

    void deallocate(void* pointer, std::size_t bytes, std::size_t alignment) noexcept override
    {
        GetNewDeleteResource()->deallocate(pointer, bytes, alignment);
    }

    bool is_equal(const MemoryResource& other) const noexcept override { return this == &other; }

  private:
    std::size_t allocation_count_{0U};
};

template <typename T>
class TestRawPtr
{
  public:
    TestRawPtr(T const* const pointer) noexcept : pointer_{pointer} {}
    TestRawPtr(const TestRawPtr&) noexcept = default;
    TestRawPtr& operator=(const TestRawPtr&) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }
    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

struct TestRawPointerPolicy
{
    template <typename T>
    using Ptr = TestRawPtr<T>;

    template <typename T>
    using NullablePtr = TestRawPtr<T>;
};

struct NonDefaultConstructible
{
    explicit NonDefaultConstructible(int value_in) noexcept : value{value_in} {}

    int value;
};

template <typename T, typename = void>
struct HasCountCreate : std::false_type
{
};

template <typename T>
struct HasCountCreate<T, std::void_t<decltype(Vector<T>::Create(std::size_t{0U}))>> : std::true_type
{
};

template <typename T, typename = void>
struct HasCountCreateOrAbort : std::false_type
{
};

template <typename T>
struct HasCountCreateOrAbort<T, std::void_t<decltype(Vector<T>::CreateOrAbort(std::size_t{0U}))>> : std::true_type
{
};

template <typename T, typename = void>
struct HasCountResize : std::false_type
{
};

template <typename T>
struct HasCountResize<T, std::void_t<decltype(std::declval<Vector<T>&>().Resize(std::size_t{0U}))>> : std::true_type
{
};

template <typename T, typename = void>
struct HasCountResizeOrAbort : std::false_type
{
};

template <typename T>
struct HasCountResizeOrAbort<T, std::void_t<decltype(std::declval<Vector<T>&>().ResizeOrAbort(std::size_t{0U}))>>
    : std::true_type
{
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

TEST(Vector, CreateOrAbortWithCountDefaultConstructsElements)
{
    Vector<int> vector = Vector<int>::CreateOrAbort(3U);

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

TEST(Vector, CreateOrAbortWithCountAndValueFillsElements)
{
    Vector<int> vector = Vector<int>::CreateOrAbort(3U, 42);

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 42);
    EXPECT_EQ(vector.at(1U), 42);
    EXPECT_EQ(vector.at(2U), 42);
}

TEST(Vector, CountCreateIsUnavailableForNonDefaultConstructibleTypes)
{
    EXPECT_FALSE(HasCountCreate<NonDefaultConstructible>::value);
    EXPECT_FALSE(HasCountCreateOrAbort<NonDefaultConstructible>::value);
    EXPECT_FALSE(HasCountResize<NonDefaultConstructible>::value);
    EXPECT_FALSE(HasCountResizeOrAbort<NonDefaultConstructible>::value);
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

TEST(Vector, CreateOrAbortFromIteratorRange)
{
    const int source[] = {10, 20, 30};
    Vector<int> vector = Vector<int>::CreateOrAbort(std::begin(source), std::end(source));

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

TEST(Vector, CreateOrAbortFromInitializerList)
{
    Vector<int> vector = Vector<int>::CreateOrAbort({5, 6, 7, 8});

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

TEST(Vector, PushBackLvalueGrowsWhenFull)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    const int first = 7;
    const int second = 11;

    EXPECT_TRUE(vector.PushBack(first).has_value());
    EXPECT_TRUE(vector.PushBack(second).has_value());

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_GE(vector.capacity(), 2U);
    EXPECT_EQ(vector.at(0U), 7);
    EXPECT_EQ(vector.at(1U), 11);
}

TEST(Vector, ConstElementAccessWork)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);

    EXPECT_TRUE(vector.PushBack(7).has_value());
    EXPECT_TRUE(vector.PushBack(11).has_value());

    const Vector<int>& cvector = vector;
    EXPECT_EQ(cvector.size(), 2U);
    EXPECT_EQ(cvector.at(0U), 7);
    EXPECT_EQ(cvector.at(1U), 11);
    EXPECT_EQ(cvector.front(), 7);
    EXPECT_EQ(cvector.back(), 11);
}

TEST(Vector, PushBackLvaluePropagatesOutOfMemoryOnGrowth)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    const int first = 7;
    const int second = 11;

    EXPECT_TRUE(vector.PushBack(first).has_value());

    auto push_result = vector.PushBack(second);
    ASSERT_FALSE(push_result.has_value());
    EXPECT_EQ(push_result.error(), ContainerErrorCode::kOutOfMemory);
    ASSERT_EQ(vector.size(), 1U);
    EXPECT_EQ(vector.at(0U), 7);
}

TEST(Vector, PushBackOrAbortLvalueGrowsWhenFull)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    const int first = 7;
    const int second = 11;

    vector.PushBackOrAbort(first);
    vector.PushBackOrAbort(second);

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_GE(vector.capacity(), 2U);
    EXPECT_EQ(vector.at(0U), 7);
    EXPECT_EQ(vector.at(1U), 11);
}

TEST(Vector, PushBackOrAbortRvalueGrowsWhenFull)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);

    vector.PushBackOrAbort(7);
    vector.PushBackOrAbort(11);

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_GE(vector.capacity(), 2U);
    EXPECT_EQ(vector.at(0U), 7);
    EXPECT_EQ(vector.at(1U), 11);
}

TEST(VectorDeathTest, AtAbortsOnOutOfRange)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.at(0U), "");
}

TEST(VectorDeathTest, ConstAtAbortsOnOutOfRange)
{
    const Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.at(0U), "");
}

TEST(VectorDeathTest, FrontAbortsOnEmpty)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.front(), "");
}

TEST(VectorDeathTest, ConstFrontAbortsOnEmpty)
{
    const Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.front(), "");
}

TEST(VectorDeathTest, BackAbortsOnEmpty)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.back(), "");
}

TEST(VectorDeathTest, ConstBackAbortsOnEmpty)
{
    const Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.back(), "");
}

TEST(VectorDeathTest, PushBackOrAbortLvalueAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    const int first = 7;
    const int second = 11;

    ASSERT_TRUE(vector.PushBack(first).has_value());
    EXPECT_DEATH(vector.PushBackOrAbort(second), "");
}

TEST(VectorDeathTest, PushBackOrAbortRvalueAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    ASSERT_TRUE(vector.PushBack(7).has_value());
    EXPECT_DEATH(vector.PushBackOrAbort(11), "");
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

TEST(Vector, InsertRvalueAddsElementWithoutGrowth)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, 2);
    ASSERT_TRUE(insert_result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.capacity(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(Vector, InsertLvalueAddsElementAtPosition)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    const int inserted = 2;
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, inserted);
    ASSERT_TRUE(insert_result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_GE(vector.capacity(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(Vector, InsertLvalueAddsElementWithoutGrowth)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    const int inserted = 2;
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, inserted);
    ASSERT_TRUE(insert_result.has_value());

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.capacity(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(Vector, InsertLvaluePropagatesOutOfMemoryOnGrowth)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    const int inserted = 2;
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, inserted);
    ASSERT_FALSE(insert_result.has_value());
    EXPECT_EQ(insert_result.error(), ContainerErrorCode::kOutOfMemory);
    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 3);
}

TEST(Vector, InsertRvaluePropagatesOutOfMemoryOnGrowth)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    auto insert_result = vector.Insert(1U, 2);
    ASSERT_FALSE(insert_result.has_value());
    EXPECT_EQ(insert_result.error(), ContainerErrorCode::kOutOfMemory);
    ASSERT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 3);
}

TEST(Vector, InsertOrAbortLvalueAddsElement)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    const int inserted = 2;
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    vector.InsertOrAbort(1U, inserted);

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(Vector, InsertOrAbortRvalueAddsElement)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    vector.InsertOrAbort(1U, 2);

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
}

TEST(VectorDeathTest, InsertLvalueAbortsOnOutOfRangeIndex)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    const int inserted = 42;
    EXPECT_DEATH(vector.Insert(1U, inserted), "");
}

TEST(VectorDeathTest, InsertOrAbortLvalueAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    const int inserted = 2;
    ASSERT_TRUE(vector.PushBack(1).has_value());
    ASSERT_TRUE(vector.PushBack(3).has_value());

    EXPECT_DEATH(vector.InsertOrAbort(1U, inserted), "");
}

TEST(VectorDeathTest, InsertOrAbortRvalueAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int) * 2U]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    ASSERT_TRUE(vector.PushBack(1).has_value());
    ASSERT_TRUE(vector.PushBack(3).has_value());

    EXPECT_DEATH(vector.InsertOrAbort(1U, 2), "");
}

TEST(VectorDeathTest, InsertAbortsOnOutOfRangeIndex)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    EXPECT_DEATH(vector.Insert(1U, 42), "");
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

TEST(Vector, ConstBeginEndProvideContiguousIteration)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    const Vector<int>& cvector = vector;

    int sum = 0;
    for (auto it = cvector.begin(); it != cvector.end(); ++it)
    {
        sum += *it;
    }

    int csum = 0;
    for (auto it = cvector.cbegin(); it != cvector.cend(); ++it)
    {
        csum += *it;
    }

    EXPECT_EQ(sum, 6);
    EXPECT_EQ(csum, 6);
}

TEST(Vector, SubscriptProvidesAccess)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    vector[1U] = 22;

    EXPECT_EQ(vector[0U], 1);
    EXPECT_EQ(vector[1U], 22);
    EXPECT_EQ(vector[2U], 3);
}

TEST(Vector, ConstSubscriptProvidesAccess)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(3U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());
    EXPECT_TRUE(vector.PushBack(3).has_value());

    const Vector<int>& cvector = vector;

    EXPECT_EQ(cvector[0U], 1);
    EXPECT_EQ(cvector[1U], 2);
    EXPECT_EQ(cvector[2U], 3);
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

TEST(Vector, ClonePropagatesOutOfMemoryFromAllocator)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    FailAfterFirstAllocationResource resource{};
    PolymorphicAllocator<int> allocator{&resource};
    AllocVector vector = AllocVector::CreateWithCapacityOrAbort(2U, allocator);
    EXPECT_TRUE(vector.PushBack(5).has_value());
    EXPECT_TRUE(vector.PushBack(6).has_value());

    auto clone_result = vector.Clone();
    ASSERT_FALSE(clone_result.has_value());
    EXPECT_EQ(clone_result.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(VectorDeathTest, CloneOrAbortAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    FailAfterFirstAllocationResource resource{};
    PolymorphicAllocator<int> allocator{&resource};
    AllocVector vector = AllocVector::CreateWithCapacityOrAbort(2U, allocator);
    ASSERT_TRUE(vector.PushBack(5).has_value());
    ASSERT_TRUE(vector.PushBack(6).has_value());

    EXPECT_DEATH(
        {
            (void)vector.CloneOrAbort();
        },
        "");
}

TEST(Vector, CloneOrAbortCreatesIndependentCopy)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(5).has_value());
    EXPECT_TRUE(vector.PushBack(6).has_value());

    Vector<int> clone = vector.CloneOrAbort();
    EXPECT_EQ(clone.size(), 2U);
    EXPECT_EQ(clone.at(0U), 5);
    EXPECT_EQ(clone.at(1U), 6);

    EXPECT_TRUE(clone.PushBack(7).has_value());
    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(clone.size(), 3U);
}

TEST(Vector, MoveAssignmentTransfersContents)
{
    Vector<int> source = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(source.PushBack(5).has_value());
    EXPECT_TRUE(source.PushBack(6).has_value());

    Vector<int> target = Vector<int>::CreateWithCapacityOrAbort(1U);
    EXPECT_TRUE(target.PushBack(99).has_value());

    target = std::move(source);

    EXPECT_EQ(target.size(), 2U);
    EXPECT_EQ(target.at(0U), 5);
    EXPECT_EQ(target.at(1U), 6);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.capacity(), 0U);
}

TEST(Vector, MoveAssignmentToSelfIsNoOp)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(2U);
    EXPECT_TRUE(vector.PushBack(5).has_value());
    EXPECT_TRUE(vector.PushBack(6).has_value());

    Vector<int>& alias = vector;
    vector = std::move(alias);

    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 5);
    EXPECT_EQ(vector.at(1U), 6);
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

TEST(Vector, SupportsInjectedPointerPolicy)
{
    using PolicyVector = Vector<int, PolymorphicAllocator<int>, TestRawPointerPolicy>;
    PolicyVector vector = PolicyVector::CreateWithCapacityOrAbort(0U);

    ASSERT_TRUE(vector.PushBack(1).has_value());
    ASSERT_TRUE(vector.PushBack(2).has_value());
    ASSERT_TRUE(vector.PushBack(3).has_value());

    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
    EXPECT_EQ(vector.at(2U), 3);
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

TEST(Vector, CreateCountPropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto created = Vector<int, PolymorphicAllocator<int>>::Create(4U, allocator);
    ASSERT_FALSE(created.has_value());
    EXPECT_EQ(created.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Vector, CreateCountAndValuePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto created = Vector<int, PolymorphicAllocator<int>>::Create(4U, 42, allocator);
    ASSERT_FALSE(created.has_value());
    EXPECT_EQ(created.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Vector, CreateRangePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};
    const int source[] = {1, 2, 3, 4};

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto created = Vector<int, PolymorphicAllocator<int>>::Create(std::begin(source), std::end(source), allocator);
    ASSERT_FALSE(created.has_value());
    EXPECT_EQ(created.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(VectorDeathTest, CreateWithCapacityAbortsOnCapacityOverflow)
{
    EXPECT_DEATH(
        (void)Vector<int>::CreateWithCapacity((std::numeric_limits<std::size_t>::max() / sizeof(int)) + 1U),
        "");
}

TEST(VectorDeathTest, CreateWithCapacityOrAbortAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    PolymorphicAllocator<int> allocator{GetNullMemoryResource()};
    EXPECT_DEATH(
        {
            (void)AllocVector::CreateWithCapacityOrAbort(4U, allocator);
        },
        "");
}

TEST(VectorDeathTest, CreateOrAbortCountAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    PolymorphicAllocator<int> allocator{GetNullMemoryResource()};
    EXPECT_DEATH(
        {
            (void)AllocVector::CreateOrAbort(4U, allocator);
        },
        "");
}

TEST(VectorDeathTest, CreateOrAbortCountAndValueAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    PolymorphicAllocator<int> allocator{GetNullMemoryResource()};
    EXPECT_DEATH(
        {
            (void)AllocVector::CreateOrAbort(4U, 42, allocator);
        },
        "");
}

TEST(VectorDeathTest, CreateOrAbortRangeAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    const int source[] = {1, 2, 3, 4};
    PolymorphicAllocator<int> allocator{GetNullMemoryResource()};
    EXPECT_DEATH(
        {
            (void)AllocVector::CreateOrAbort(std::begin(source), std::end(source), allocator);
        },
        "");
}

TEST(VectorDeathTest, CreateOrAbortInitializerListAbortsOnOutOfMemory)
{
    using AllocVector = Vector<int, PolymorphicAllocator<int>>;
    PolymorphicAllocator<int> allocator{GetNullMemoryResource()};
    EXPECT_DEATH(
        {
            (void)AllocVector::CreateOrAbort({1, 2, 3, 4}, allocator);
        },
        "");
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

TEST(VectorDeathTest, ReserveAbortsOnCapacityOverflow)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);
    EXPECT_DEATH(vector.Reserve((std::numeric_limits<std::size_t>::max() / sizeof(int)) + 1U), "");
}

TEST(Vector, ReserveOrAbortGrowsCapacity)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(1U);
    EXPECT_TRUE(vector.PushBack(7).has_value());

    vector.ReserveOrAbort(4U);

    EXPECT_EQ(vector.size(), 1U);
    EXPECT_GE(vector.capacity(), 4U);
    EXPECT_EQ(vector.at(0U), 7);
}

TEST(VectorDeathTest, ReserveOrAbortAbortsOnAllocationFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    EXPECT_DEATH(vector.ReserveOrAbort(2U), "");
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

struct DefaultCountingLifecycleType
{
    static inline std::size_t constructor_count{0U};
    static inline std::size_t destructor_count{0U};

    DefaultCountingLifecycleType() noexcept : value{0}
    {
        ++constructor_count;
    }

    explicit DefaultCountingLifecycleType(int value_in) noexcept : value{value_in}
    {
        ++constructor_count;
    }

    DefaultCountingLifecycleType(const DefaultCountingLifecycleType& other) noexcept : value{other.value}
    {
        ++constructor_count;
    }

    DefaultCountingLifecycleType(DefaultCountingLifecycleType&& other) noexcept : value{other.value}
    {
        ++constructor_count;
        other.value = -1;
    }

    DefaultCountingLifecycleType& operator=(const DefaultCountingLifecycleType&) = default;
    DefaultCountingLifecycleType& operator=(DefaultCountingLifecycleType&&) = default;

    ~DefaultCountingLifecycleType() noexcept
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

TEST(Vector, EmplaceBackPropagatesOutOfMemoryOnGrowth)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    EXPECT_TRUE(vector.EmplaceBack(7).has_value());

    auto emplace_result = vector.EmplaceBack(11);
    ASSERT_FALSE(emplace_result.has_value());
    EXPECT_EQ(emplace_result.error(), ContainerErrorCode::kOutOfMemory);
    ASSERT_EQ(vector.size(), 1U);
    EXPECT_EQ(vector.at(0U), 7);
}

TEST(Vector, EmplaceBackOrAbortGrowsWhenFull)
{
    Vector<std::pair<int, int>> vector = Vector<std::pair<int, int>>::CreateWithCapacityOrAbort(1U);

    vector.EmplaceBackOrAbort(1, 2);
    vector.EmplaceBackOrAbort(3, 4);

    ASSERT_EQ(vector.size(), 2U);
    EXPECT_GE(vector.capacity(), 2U);
    EXPECT_EQ(vector.at(0U).first, 1);
    EXPECT_EQ(vector.at(0U).second, 2);
    EXPECT_EQ(vector.at(1U).first, 3);
    EXPECT_EQ(vector.at(1U).second, 4);
}

TEST(VectorDeathTest, EmplaceBackOrAbortAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));

    ASSERT_TRUE(vector.EmplaceBack(7).has_value());
    EXPECT_DEATH(vector.EmplaceBackOrAbort(11), "");
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

TEST(Vector, ResizePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};
    Vector<int, PolymorphicAllocator<int>> vector =
        Vector<int, PolymorphicAllocator<int>>::CreateWithCapacityOrAbort(0U, allocator);

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto result = vector.Resize(4U);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
    EXPECT_EQ(vector.size(), 0U);
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

TEST(Vector, ResizeWithFillValuePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<int> allocator{&mock_resource};
    Vector<int, PolymorphicAllocator<int>> vector =
        Vector<int, PolymorphicAllocator<int>>::CreateWithCapacityOrAbort(0U, allocator);

    EXPECT_CALL(mock_resource, allocate(4U * sizeof(int), alignof(int))).WillOnce(Return(nullptr));

    auto result = vector.Resize(4U, 42);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
    EXPECT_EQ(vector.size(), 0U);
}

TEST(Vector, ResizeOrAbortDefaultConstructedGrows)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);

    vector.ResizeOrAbort(3U);

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 0);
    EXPECT_EQ(vector.at(1U), 0);
    EXPECT_EQ(vector.at(2U), 0);
}

TEST(VectorDeathTest, ResizeOrAbortDefaultConstructedAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    ASSERT_TRUE(vector.PushBack(7).has_value());

    EXPECT_DEATH(vector.ResizeOrAbort(2U), "");
}

TEST(Vector, ResizeOrAbortFillValueGrows)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(0U);

    vector.ResizeOrAbort(3U, 42);

    ASSERT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.at(0U), 42);
    EXPECT_EQ(vector.at(1U), 42);
    EXPECT_EQ(vector.at(2U), 42);
}

TEST(VectorDeathTest, ResizeOrAbortFillValueAbortsOnGrowthFailure)
{
    alignas(int) std::byte buffer[sizeof(int)]{};
    Vector<int> vector = Vector<int>::CreateWithBuffer(buffer, sizeof(buffer));
    ASSERT_TRUE(vector.PushBack(7).has_value());

    EXPECT_DEATH(vector.ResizeOrAbort(2U, 42), "");
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

        EXPECT_TRUE(vector.Resize(1U, CountingLifecycleType{0}).has_value());
        ASSERT_EQ(vector.size(), 1U);
        EXPECT_EQ(vector.at(0U).value, 1);
    }

    EXPECT_EQ(CountingLifecycleType::constructor_count, CountingLifecycleType::destructor_count);
}

TEST(Vector, ResizeDefaultConstructedShrinkDestroysElements)
{
    DefaultCountingLifecycleType::constructor_count = 0U;
    DefaultCountingLifecycleType::destructor_count = 0U;

    {
        Vector<DefaultCountingLifecycleType> vector = Vector<DefaultCountingLifecycleType>::CreateOrAbort(3U);
        ASSERT_EQ(vector.size(), 3U);

        EXPECT_TRUE(vector.Resize(1U).has_value());
        ASSERT_EQ(vector.size(), 1U);
        EXPECT_EQ(vector.at(0U).value, 0);
    }

    EXPECT_EQ(DefaultCountingLifecycleType::constructor_count, DefaultCountingLifecycleType::destructor_count);
}

TEST(Vector, ReserveGrowsCapacityForDefaultCountingLifecycleType)
{
    Vector<DefaultCountingLifecycleType> vector = Vector<DefaultCountingLifecycleType>::CreateOrAbort(1U);
    ASSERT_EQ(vector.size(), 1U);
    EXPECT_EQ(vector.capacity(), 1U);

    auto result = vector.Reserve(3U);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(vector.size(), 1U);
    EXPECT_GE(vector.capacity(), 3U);
    EXPECT_EQ(vector.at(0U).value, 0);
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

TEST(Vector, ShrinkToFitPropagatesOutOfMemoryFromAllocator)
{
    FailAfterFirstAllocationResource resource{};
    PolymorphicAllocator<int> allocator{&resource};
    Vector<int, PolymorphicAllocator<int>> vector =
        Vector<int, PolymorphicAllocator<int>>::CreateWithCapacityOrAbort(4U, allocator);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    auto result = vector.ShrinkToFit();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
    EXPECT_EQ(vector.capacity(), 4U);
    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
}

TEST(Vector, ShrinkToFitOrAbortReducesCapacity)
{
    Vector<int> vector = Vector<int>::CreateWithCapacityOrAbort(10U);
    EXPECT_TRUE(vector.PushBack(1).has_value());
    EXPECT_TRUE(vector.PushBack(2).has_value());

    vector.ShrinkToFitOrAbort();

    EXPECT_EQ(vector.capacity(), 2U);
    EXPECT_EQ(vector.size(), 2U);
    EXPECT_EQ(vector.at(0U), 1);
    EXPECT_EQ(vector.at(1U), 2);
}

TEST(VectorDeathTest, ShrinkToFitOrAbortAbortsOnAllocationFailure)
{
    FailAfterFirstAllocationResource resource{};
    PolymorphicAllocator<int> allocator{&resource};
    Vector<int, PolymorphicAllocator<int>> vector =
        Vector<int, PolymorphicAllocator<int>>::CreateWithCapacityOrAbort(4U, allocator);
    ASSERT_TRUE(vector.PushBack(1).has_value());
    ASSERT_TRUE(vector.PushBack(2).has_value());

    EXPECT_DEATH(vector.ShrinkToFitOrAbort(), "");
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
}  // namespace score::nothrow
