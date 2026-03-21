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

#include "score/shm/map.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace score::shm
{
namespace
{

using ::testing::Return;
using ::testing::_;

class MockMemoryResource final : public MemoryResource
{
  public:
    MOCK_METHOD(void*, allocate, (std::size_t, std::size_t), (noexcept, override));
    MOCK_METHOD(void, deallocate, (void*, std::size_t, std::size_t), (noexcept, override));
    MOCK_METHOD(bool, is_equal, (const MemoryResource&), (const, noexcept, override));
};

TEST(Map, CreateStartsEmpty)
{
    auto created = Map<int, int>::Create();
    ASSERT_TRUE(created.has_value());

    Map<int, int> map = std::move(created).value();
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0U);
    EXPECT_EQ(map.begin(), map.end());
}

TEST(Map, CreateFromInitializerListBuildsOrderedMap)
{
    auto created = Map<int, int>::Create({{3, 30}, {1, 10}, {2, 20}});
    ASSERT_TRUE(created.has_value());

    Map<int, int> map = std::move(created).value();
    ASSERT_EQ(map.size(), 3U);

    auto it = map.begin();
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, 10);
    ++it;
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, 20);
    ++it;
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 3);
    EXPECT_EQ(it->second, 30);
}

TEST(Map, InsertStoresAndFindRetrieves)
{
    Map<int, std::string> map = Map<int, std::string>::CreateOrAbort();

    auto inserted = map.Insert({7, "seven"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 7);
    EXPECT_EQ(inserted.value().first->second, "seven");

    auto found = map.find(7);
    ASSERT_NE(found, map.end());
    EXPECT_EQ(found->second, "seven");
    EXPECT_EQ(map.at(7), "seven");
    EXPECT_TRUE(map.contains(7));
}

TEST(Map, InsertDuplicateKeepsExistingValue)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({5, 10}).has_value());

    auto duplicate = map.Insert({5, 99});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 10);
    EXPECT_EQ(map.size(), 1U);
}

TEST(Map, EmplaceStoresValue)
{
    Map<int, std::string> map = Map<int, std::string>::CreateOrAbort();

    auto emplaced = map.Emplace(4, "four");
    ASSERT_TRUE(emplaced.has_value());
    EXPECT_TRUE(emplaced.value().second);
    EXPECT_EQ(map.at(4), "four");
}

TEST(Map, GetOrInsertDefaultInsertsAndReturnsReference)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();

    auto first = map.GetOrInsertDefault(11);
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(first.value().get(), 0);

    first.value().get() = 42;
    auto second = map.GetOrInsertDefault(11);
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(second.value().get(), 42);
    EXPECT_EQ(map.size(), 1U);
}

TEST(Map, EraseRemovesExistingKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    EXPECT_EQ(map.erase(2), 1U);
    EXPECT_EQ(map.size(), 2U);
    EXPECT_FALSE(map.contains(2));
    EXPECT_EQ(map.erase(2), 0U);
}

TEST(Map, ClearRemovesAllElements)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 0; key < 10; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key * 10}).has_value());
    }
    ASSERT_EQ(map.size(), 10U);

    map.clear();
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0U);
    EXPECT_EQ(map.begin(), map.end());
}

TEST(Map, CloneCreatesIndependentCopy)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto cloned = map.Clone();
    ASSERT_TRUE(cloned.has_value());
    Map<int, int> clone = std::move(cloned).value();

    EXPECT_EQ(clone.size(), 2U);
    EXPECT_EQ(clone.at(1), 10);
    EXPECT_EQ(clone.at(2), 20);

    ASSERT_TRUE(clone.Insert({3, 30}).has_value());
    EXPECT_FALSE(map.contains(3));
    EXPECT_TRUE(clone.contains(3));
}

TEST(Map, MoveConstructionTransfersOwnership)
{
    Map<int, int> source = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(source.Insert({1, 1}).has_value());
    ASSERT_TRUE(source.Insert({2, 2}).has_value());

    Map<int, int> moved{std::move(source)};
    EXPECT_EQ(moved.size(), 2U);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(moved.at(1), 1);
    EXPECT_EQ(moved.at(2), 2);
}

TEST(Map, IteratorTraversesInOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    const std::array<int, 7U> keys{{40, 10, 60, 50, 20, 70, 30}};
    for (const int key : keys)
    {
        ASSERT_TRUE(map.Insert({key, key}).has_value());
    }

    std::vector<int> iterated_keys{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        iterated_keys.push_back(it->first);
    }

    std::vector<int> expected{keys.begin(), keys.end()};
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(iterated_keys, expected);
}

TEST(Map, IteratorSupportsBidirectionalTraversal)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 1}).has_value());
    ASSERT_TRUE(map.Insert({2, 2}).has_value());
    ASSERT_TRUE(map.Insert({3, 3}).has_value());

    auto it = map.end();
    --it;
    EXPECT_EQ(it->first, 3);
    --it;
    EXPECT_EQ(it->first, 2);
    --it;
    EXPECT_EQ(it->first, 1);
}

TEST(Map, HandlesAscendingInsertionsAndRemovals)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 0; key < 200; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key * 2}).has_value());
    }

    for (int key = 0; key < 200; key += 2)
    {
        EXPECT_EQ(map.erase(key), 1U);
    }

    EXPECT_EQ(map.size(), 100U);
    for (int key = 1; key < 200; key += 2)
    {
        EXPECT_TRUE(map.contains(key));
        EXPECT_EQ(map.at(key), key * 2);
    }
}

TEST(Map, InsertPropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<std::pair<const int, int>> allocator{&mock_resource};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);

    EXPECT_CALL(mock_resource, allocate(_, _)).WillOnce(Return(nullptr));

    auto inserted = map.Insert({1, 1});
    ASSERT_FALSE(inserted.has_value());
    EXPECT_EQ(inserted.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Map, EmplacePropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<std::pair<const int, int>> allocator{&mock_resource};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);

    EXPECT_CALL(mock_resource, allocate(_, _)).WillOnce(Return(nullptr));

    auto emplaced = map.Emplace(1, 1);
    ASSERT_FALSE(emplaced.has_value());
    EXPECT_EQ(emplaced.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Map, GetOrInsertDefaultPropagatesOutOfMemoryFromAllocator)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<std::pair<const int, int>> allocator{&mock_resource};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);

    EXPECT_CALL(mock_resource, allocate(_, _)).WillOnce(Return(nullptr));

    auto result = map.GetOrInsertDefault(42);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Map, CreateFromRangePropagatesOutOfMemory)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<std::pair<const int, int>> allocator{&mock_resource};
    const std::array<std::pair<const int, int>, 2U> values{{{1, 1}, {2, 2}}};

    EXPECT_CALL(mock_resource, allocate(_, _)).WillOnce(Return(nullptr));

    auto created = Map<int, int>::Create(values.begin(), values.end(), std::less<int>{}, allocator);
    ASSERT_FALSE(created.has_value());
    EXPECT_EQ(created.error(), ContainerErrorCode::kOutOfMemory);
}

struct NonDefaultConstructible
{
    explicit NonDefaultConstructible(int value_in) noexcept : value{value_in} {}
    NonDefaultConstructible(const NonDefaultConstructible&) = default;
    NonDefaultConstructible(NonDefaultConstructible&&) noexcept = default;
    NonDefaultConstructible& operator=(const NonDefaultConstructible&) = default;
    NonDefaultConstructible& operator=(NonDefaultConstructible&&) noexcept = default;
    ~NonDefaultConstructible() = default;

    int value;
};

TEST(MapDeathTest, AtAbortsOnMissingKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    EXPECT_DEATH(map.at(1), "");
}

TEST(MapDeathTest, DecrementEndAbortsOnEmpty)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    EXPECT_DEATH(--map.end(), "");
}

TEST(MapDeathTest, InsertOrAbortAbortsOnOutOfMemory)
{
    PolymorphicAllocator<std::pair<const int, int>> allocator{GetNullMemoryResource()};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);
    EXPECT_DEATH(map.InsertOrAbort({1, 1}), "");
}

TEST(MapDeathTest, GetOrInsertDefaultOrAbortAbortsOnOutOfMemory)
{
    PolymorphicAllocator<std::pair<const int, int>> allocator{GetNullMemoryResource()};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);
    EXPECT_DEATH(map.GetOrInsertDefaultOrAbort(1), "");
}

TEST(MapDeathTest, GetOrInsertDefaultAbortsForNonDefaultConstructibleMappedType)
{
    Map<int, NonDefaultConstructible> map = Map<int, NonDefaultConstructible>::CreateOrAbort();
    EXPECT_DEATH(map.GetOrInsertDefault(1), "");
}

}  // namespace
}  // namespace score::shm
