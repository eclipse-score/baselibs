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

#include "score/nothrow/map.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace score::nothrow
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

template <typename T>
class TestNullableRawPtr
{
  public:
    TestNullableRawPtr(T const* const pointer) noexcept : pointer_{pointer} {}
    TestNullableRawPtr(const TestNullableRawPtr&) noexcept = default;
    TestNullableRawPtr& operator=(const TestNullableRawPtr&) noexcept = default;

    T* get() noexcept { return const_cast<T*>(pointer_); }
    const T* get() const noexcept { return pointer_; }

  private:
    const T* pointer_{nullptr};
};

struct TestRawPointerPolicy
{
    template <typename T>
    using Ptr = TestNullableRawPtr<T>;

    template <typename T>
    using NullablePtr = TestNullableRawPtr<T>;
};

using IntIntMap = Map<int, int>;
using StringMap = Map<int, std::string>;
using GreaterIntMap = Map<int, int, std::greater<int>>;
using PolicyIntMap = Map<int,
                         int,
                         std::less<int>,
                         PolymorphicAllocator<std::pair<const int, int>>,
                         TestRawPointerPolicy>;

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

TEST(Map, SupportsInjectedPointerPolicy)
{
    using PolicyMap = Map<int,
                          int,
                          std::less<int>,
                          PolymorphicAllocator<std::pair<const int, int>>,
                          TestRawPointerPolicy>;

    PolicyMap map = PolicyMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    EXPECT_EQ(map.size(), 3U);
    EXPECT_EQ(map.at(1), 10);
    EXPECT_EQ(map.at(2), 20);
    EXPECT_EQ(map.at(3), 30);
}

TEST(Map, PolicyMapDuplicateInsertKeepsExistingValue)
{
    PolicyIntMap map = PolicyIntMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({5, 10}).has_value());

    auto duplicate = map.Insert({5, 99});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 10);
}

TEST(Map, SupportsCustomComparatorOrderingAndLookup)
{
    using DescMap = Map<int, int, std::greater<int>>;
    DescMap map = DescMap::CreateOrAbort(std::greater<int>{});

    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    std::vector<int> iterated_keys{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        iterated_keys.push_back(it->first);
    }
    EXPECT_EQ(iterated_keys, (std::vector<int>{3, 2, 1}));

    const auto found = map.find(2);
    ASSERT_NE(found, map.end());
    EXPECT_EQ(found->second, 20);
    EXPECT_TRUE(map.contains(1));
    EXPECT_EQ(map.find(99), map.end());
}

TEST(Map, GreaterMapDuplicateInsertKeepsExistingValue)
{
    GreaterIntMap map = GreaterIntMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto duplicate = map.Insert({2, 99});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 20);
}

TEST(Map, StringMapDuplicateInsertKeepsExistingValue)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({2, "two"}).has_value());

    auto duplicate = map.Insert({2, "other"});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, "two");
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

TEST(Map, CreateFromRangeBuildsOrderedMap)
{
    const std::array<std::pair<const int, int>, 3U> values{{{3, 30}, {1, 10}, {2, 20}}};
    auto created = Map<int, int>::Create(values.begin(), values.end());
    ASSERT_TRUE(created.has_value());

    Map<int, int> map = std::move(created).value();
    ASSERT_EQ(map.size(), 3U);
    EXPECT_EQ(map.at(1), 10);
    EXPECT_EQ(map.at(2), 20);
    EXPECT_EQ(map.at(3), 30);
}

TEST(Map, MoveAssignmentTransfersOwnership)
{
    Map<int, int> source = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(source.Insert({1, 10}).has_value());
    ASSERT_TRUE(source.Insert({2, 20}).has_value());

    Map<int, int> target = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(target.Insert({99, 99}).has_value());

    target = std::move(source);
    EXPECT_EQ(target.size(), 2U);
    EXPECT_EQ(target.at(1), 10);
    EXPECT_EQ(target.at(2), 20);
    EXPECT_FALSE(target.contains(99));
    EXPECT_TRUE(source.empty());
}

TEST(Map, MoveAssignmentToSelfIsNoOp)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    Map<int, int>& ref = map;
    map = std::move(ref);
    EXPECT_EQ(map.size(), 1U);
    EXPECT_EQ(map.at(1), 10);
}

TEST(Map, SwapExchangesContents)
{
    Map<int, int> first = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(first.Insert({1, 10}).has_value());
    ASSERT_TRUE(first.Insert({2, 20}).has_value());

    Map<int, int> second = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(second.Insert({3, 30}).has_value());

    first.swap(second);

    EXPECT_EQ(first.size(), 1U);
    EXPECT_EQ(first.at(3), 30);
    EXPECT_EQ(second.size(), 2U);
    EXPECT_EQ(second.at(1), 10);
    EXPECT_EQ(second.at(2), 20);
}

TEST(Map, EmplaceOrAbortReturnsIteratorAndFlag)
{
    Map<int, std::string> map = Map<int, std::string>::CreateOrAbort();

    auto [it, inserted] = map.EmplaceOrAbort(5, "five");
    EXPECT_TRUE(inserted);
    EXPECT_EQ(it->first, 5);
    EXPECT_EQ(it->second, "five");

    auto [it2, inserted2] = map.EmplaceOrAbort(5, "other");
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(it2->second, "five");
    EXPECT_EQ(map.size(), 1U);
}

TEST(Map, HintInsertSupportsSortedFastPath)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    Map<int, int>::const_iterator hint = map.cend();
    for (int key = 0; key < 100; ++key)
    {
        auto inserted = map.Insert(hint, {key, key * 10});
        ASSERT_TRUE(inserted.has_value());
        EXPECT_TRUE(inserted.value().second);
        hint = inserted.value().first;
    }

    EXPECT_EQ(map.size(), 100U);
    EXPECT_EQ(map.begin()->first, 0);
    auto it = map.end();
    --it;
    EXPECT_EQ(it->first, 99);
}

TEST(Map, HintInsertDuplicateReturnsExisting)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());

    const auto hint = map.find(20);
    auto duplicate = map.Insert(hint, {10, 7});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 1);
}

TEST(Map, HintEmplaceSupportsSortedFastPath)
{
    Map<int, std::string> map = Map<int, std::string>::CreateOrAbort();
    Map<int, std::string>::const_iterator hint = map.cend();
    for (int key = 0; key < 32; ++key)
    {
        auto inserted = map.Emplace(hint, key, "value");
        ASSERT_TRUE(inserted.has_value());
        EXPECT_TRUE(inserted.value().second);
        hint = inserted.value().first;
    }

    EXPECT_EQ(map.size(), 32U);
    EXPECT_EQ(map.begin()->first, 0);
    auto it = map.end();
    --it;
    EXPECT_EQ(it->first, 31);
}

TEST(Map, StringMapHintInsertWithForeignHintFallsBackToLookup)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, "ten"}).has_value());
    ASSERT_TRUE(map.Insert({30, "thirty"}).has_value());

    StringMap other = StringMap::CreateOrAbort();
    ASSERT_TRUE(other.Insert({20, "twenty"}).has_value());

    auto inserted = map.Insert(other.cbegin(), {20, "twenty"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 20);
}

TEST(Map, StringMapHintInsertAtEndUsesRightmostFastPathForGreaterKey)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, "one"}).has_value());
    ASSERT_TRUE(map.Insert({3, "three"}).has_value());

    auto inserted = map.Insert(map.cend(), {4, "four"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 4);
}

TEST(Map, StringMapHintInsertAtEndFallsBackWhenKeyNotGreaterThanRightmost)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, "one"}).has_value());
    ASSERT_TRUE(map.Insert({3, "three"}).has_value());

    auto inserted = map.Insert(map.cend(), {2, "two"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 2);
}

TEST(Map, StringMapHintInsertFallsBackWhenHintLeftChildExists)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, "ten"}).has_value());
    ASSERT_TRUE(map.Insert({20, "twenty"}).has_value());
    ASSERT_TRUE(map.Insert({30, "thirty"}).has_value());
    ASSERT_TRUE(map.Insert({15, "fifteen"}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {17, "seventeen"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 17);
}

TEST(Map, StringMapHintInsertFallsBackWhenHintRightChildExists)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, "ten"}).has_value());
    ASSERT_TRUE(map.Insert({20, "twenty"}).has_value());
    ASSERT_TRUE(map.Insert({30, "thirty"}).has_value());
    ASSERT_TRUE(map.Insert({25, "twenty-five"}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {23, "twenty-three"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 23);
}

TEST(Map, StringMapHintInsertFallsBackWhenKeyIsNotLessThanSuccessor)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, "ten"}).has_value());
    ASSERT_TRUE(map.Insert({20, "twenty"}).has_value());
    ASSERT_TRUE(map.Insert({30, "thirty"}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {35, "thirty-five"});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 35);
}

TEST(Map, StringMapHintInsertReturnsExistingWhenHintMatchesKey)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, "ten"}).has_value());
    ASSERT_TRUE(map.Insert({20, "twenty"}).has_value());
    ASSERT_TRUE(map.Insert({30, "thirty"}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto duplicate = map.Insert(hint, {20, "duplicate"});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, "twenty");
}

TEST(Map, CloneOrAbortReturnsIndependentCopy)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    Map<int, int> clone = map.CloneOrAbort();
    EXPECT_EQ(clone.size(), 2U);
    EXPECT_EQ(clone.at(1), 10);
    EXPECT_EQ(clone.at(2), 20);

    ASSERT_TRUE(clone.Insert({3, 30}).has_value());
    EXPECT_FALSE(map.contains(3));
}

TEST(Map, ConstIteratorViaCbegin)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    std::vector<int> keys{};
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{1, 2, 3}));
}

TEST(Map, ConstMapIteratesViaBeginEnd)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    const Map<int, int>& cmap = map;
    std::vector<int> values{};
    for (auto it = cmap.begin(); it != cmap.end(); ++it)
    {
        values.push_back(it->second);
    }
    EXPECT_EQ(values, (std::vector<int>{10, 20}));
}

TEST(Map, ConstIteratorBidirectionalTraversal)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 1}).has_value());
    ASSERT_TRUE(map.Insert({2, 2}).has_value());
    ASSERT_TRUE(map.Insert({3, 3}).has_value());

    auto it = map.cend();
    --it;
    EXPECT_EQ(it->first, 3);
    --it;
    EXPECT_EQ(it->first, 2);
    --it;
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it, map.cbegin());
}

TEST(Map, ConstIteratorConstructsFromMutableIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    Map<int, int>::iterator mutable_it = map.begin();
    Map<int, int>::const_iterator const_it = mutable_it;
    EXPECT_EQ(const_it->first, 1);
    EXPECT_EQ(const_it->second, 10);
}

TEST(Map, ConstFindReturnsConstIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    const Map<int, int>& cmap = map;
    auto it = cmap.find(1);
    ASSERT_NE(it, cmap.end());
    EXPECT_EQ(it->second, 10);

    EXPECT_EQ(cmap.find(999), cmap.end());
}

TEST(Map, ConstAtReturnsConstReference)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    const Map<int, int>& cmap = map;
    EXPECT_EQ(cmap.at(1), 10);
}

TEST(Map, EraseAllElementsOneByOne)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 0; key < 50; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key}).has_value());
    }

    for (int key = 0; key < 50; ++key)
    {
        EXPECT_EQ(map.erase(key), 1U);
    }

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0U);
    EXPECT_EQ(map.begin(), map.end());
}

TEST(Map, EraseInReverseOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 0; key < 50; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key}).has_value());
    }

    for (int key = 49; key >= 0; --key)
    {
        EXPECT_EQ(map.erase(key), 1U);
    }

    EXPECT_TRUE(map.empty());
}

TEST(Map, InsertAfterClearReusesMap)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    map.clear();
    ASSERT_TRUE(map.empty());

    ASSERT_TRUE(map.Insert({3, 30}).has_value());
    EXPECT_EQ(map.size(), 1U);
    EXPECT_EQ(map.at(3), 30);
    EXPECT_FALSE(map.contains(1));
}

TEST(Map, PostIncrementIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto it = map.begin();
    auto prev = it++;
    EXPECT_EQ(prev->first, 1);
    EXPECT_EQ(it->first, 2);
}

TEST(Map, PostDecrementIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto it = map.end();
    --it;
    auto prev = it--;
    EXPECT_EQ(prev->first, 2);
    EXPECT_EQ(it->first, 1);
}

TEST(Map, GetAllocatorReturnsAllocator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    auto allocator = map.get_allocator();
    (void)allocator;
}

TEST(Map, InsertOrAbortMoveOverloadReturnsIteratorAndFlag)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();

    Map<int, int>::value_type value{7, 70};
    auto [it, inserted] = map.InsertOrAbort(std::move(value));
    EXPECT_TRUE(inserted);
    EXPECT_EQ(it->first, 7);
    EXPECT_EQ(it->second, 70);
    EXPECT_EQ(map.size(), 1U);
}

TEST(Map, GetOrInsertDefaultOrAbortReturnsReference)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();

    int& first = map.GetOrInsertDefaultOrAbort(42);
    EXPECT_EQ(first, 0);

    first = 1234;
    int& second = map.GetOrInsertDefaultOrAbort(42);
    EXPECT_EQ(second, 1234);
    EXPECT_EQ(map.size(), 1U);
}

TEST(Map, InsertDuplicateInLargerMapKeepsExistingValue)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    auto duplicate = map.Insert({2, 200});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 20);
    EXPECT_EQ(map.size(), 3U);
}

TEST(Map, HintInsertWithForeignHintFallsBackToLookup)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());

    Map<int, int> other = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(other.Insert({20, 200}).has_value());

    auto inserted = map.Insert(other.cbegin(), {20, 200});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 20);
    EXPECT_EQ(map.size(), 3U);
}

TEST(Map, HintInsertAtEndUsesRightmostFastPathForGreaterKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    auto inserted = map.Insert(map.cend(), {4, 40});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 4);
}

TEST(Map, HintInsertAtEndFallsBackWhenKeyNotGreaterThanRightmost)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    auto inserted = map.Insert(map.cend(), {2, 20});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 2);
}

TEST(Map, HintInsertFallsBackWhenHintLeftChildExists)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());
    ASSERT_TRUE(map.Insert({15, 150}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {17, 170});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 17);
}

TEST(Map, HintInsertFallsBackWhenHintRightChildExists)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());
    ASSERT_TRUE(map.Insert({25, 250}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {23, 230});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 23);
}

TEST(Map, HintInsertFallsBackWhenKeyIsNotLessThanSuccessor)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto inserted = map.Insert(hint, {35, 350});
    ASSERT_TRUE(inserted.has_value());
    EXPECT_TRUE(inserted.value().second);
    EXPECT_EQ(inserted.value().first->first, 35);
}

TEST(Map, HintInsertReturnsExistingWhenHintMatchesKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());

    auto hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto duplicate = map.Insert(hint, {20, 999});
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 200);
}

TEST(Map, HintEmplaceDuplicateReturnsExistingForKeyValueOverload)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());

    Map<int, int>::const_iterator hint = map.find(20);
    ASSERT_NE(hint, map.end());

    auto duplicate = map.Emplace(hint, 20, 999);
    ASSERT_TRUE(duplicate.has_value());
    EXPECT_FALSE(duplicate.value().second);
    EXPECT_EQ(duplicate.value().first->second, 200);
}

TEST(Map, PiecewiseEmplacePropagatesOutOfMemoryFromAllocator)
{
    PolymorphicAllocator<std::pair<const int, int>> allocator{GetNullMemoryResource()};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);

    auto emplaced =
        map.Emplace(std::piecewise_construct, std::forward_as_tuple(1), std::forward_as_tuple(10));
    ASSERT_FALSE(emplaced.has_value());
    EXPECT_EQ(emplaced.error(), ContainerErrorCode::kOutOfMemory);
}

TEST(Map, PiecewiseEmplaceInsertsUsingLeftChildPath)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());

    auto emplaced =
        map.Emplace(std::piecewise_construct, std::forward_as_tuple(5), std::forward_as_tuple(50));
    ASSERT_TRUE(emplaced.has_value());
    EXPECT_TRUE(emplaced.value().second);
    EXPECT_EQ(emplaced.value().first->first, 5);
    EXPECT_EQ(emplaced.value().first->second, 50);
}

TEST(Map, PreviousNodeUsesMaxOfLeftSubtree)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({50, 500}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());
    ASSERT_TRUE(map.Insert({40, 400}).has_value());
    ASSERT_TRUE(map.Insert({35, 350}).has_value());

    auto it = map.find(50);
    ASSERT_NE(it, map.end());
    --it;
    EXPECT_EQ(it->first, 40);
}

TEST(Map, RightLeftInsertionPatternRebalancesAndPreservesOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 10}).has_value());
    ASSERT_TRUE(map.Insert({30, 30}).has_value());
    ASSERT_TRUE(map.Insert({20, 20}).has_value());

    std::vector<int> keys{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{10, 20, 30}));
}

TEST(Map, GreaterMapInsertionAndIterationOrder)
{
    GreaterIntMap map = GreaterIntMap::CreateOrAbort(std::greater<int>{});
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());

    std::vector<int> keys{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{30, 20, 10}));

    auto tail = map.end();
    --tail;
    EXPECT_EQ(tail->first, 10);
}

TEST(Map, PolicyMapInsertionAndOrder)
{
    PolicyIntMap map = PolicyIntMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 100}).has_value());
    ASSERT_TRUE(map.Insert({30, 300}).has_value());
    ASSERT_TRUE(map.Insert({20, 200}).has_value());

    std::vector<int> keys{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{10, 20, 30}));
}

TEST(Map, NonRootLeftSubtreeRightRotationPreservesOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    const std::array<int, 8U> keys{{100, 50, 150, 25, 75, 10, 5, 60}};

    for (const int key : keys)
    {
        ASSERT_TRUE(map.Insert({key, key * 10}).has_value());
    }

    std::vector<int> iterated{};
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        iterated.push_back(it->first);
    }

    std::vector<int> expected{keys.begin(), keys.end()};
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(iterated, expected);
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

TEST(MapDeathTest, IteratorArrowAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.end();
            (void)it->first;
        },
        "");
}

TEST(MapDeathTest, IteratorIncrementAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.end();
            ++it;
        },
        "");
}

TEST(MapDeathTest, IteratorDecrementAbortsOnDefaultConstructedIterator)
{
    EXPECT_DEATH(
        {
            IntIntMap::iterator it{};
            --it;
        },
        "");
}

TEST(MapDeathTest, IteratorDecrementBeginAborts)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.begin();
            --it;
        },
        "");
}

TEST(MapDeathTest, ConstAtAbortsOnMissingKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    const Map<int, int>& cmap = map;
    EXPECT_DEATH(cmap.at(1), "");
}

TEST(MapDeathTest, ConstIteratorDereferenceAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.cend();
            (void)*it;
        },
        "");
}

TEST(MapDeathTest, ConstIteratorArrowAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.cend();
            (void)it->first;
        },
        "");
}

TEST(MapDeathTest, ConstIteratorIncrementAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.cend();
            ++it;
        },
        "");
}

TEST(MapDeathTest, ConstIteratorDecrementAbortsOnDefaultConstructedIterator)
{
    EXPECT_DEATH(
        {
            IntIntMap::const_iterator it{};
            --it;
        },
        "");
}

TEST(MapDeathTest, ConstIteratorDecrementEndAbortsOnEmpty)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    EXPECT_DEATH(
        {
            auto it = map.cend();
            --it;
        },
        "");
}

TEST(MapDeathTest, ConstIteratorDecrementBeginAborts)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.cbegin();
            --it;
        },
        "");
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

TEST(MapDeathTest, EmplaceOrAbortAbortsOnOutOfMemory)
{
    PolymorphicAllocator<std::pair<const int, int>> allocator{GetNullMemoryResource()};
    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);
    EXPECT_DEATH(map.EmplaceOrAbort(1, 1), "");
}

TEST(MapDeathTest, CloneOrAbortAbortsOnOutOfMemory)
{
    MockMemoryResource mock_resource{};
    PolymorphicAllocator<std::pair<const int, int>> allocator{&mock_resource};

    alignas(alignof(std::max_align_t)) std::uint8_t buffer[256]{};
    EXPECT_CALL(mock_resource, allocate(_, _))
        .WillOnce(Return(static_cast<void*>(buffer)))
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock_resource, deallocate(_, _, _)).Times(::testing::AnyNumber());

    Map<int, int> map = Map<int, int>::CreateOrAbort(std::less<int>{}, allocator);
    ASSERT_TRUE(map.Insert({1, 1}).has_value());

    EXPECT_DEATH(map.CloneOrAbort(), "");
}

TEST(MapDeathTest, StringMapDecrementBeginAborts)
{
    StringMap map = StringMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, "one"}).has_value());
    ASSERT_TRUE(map.Insert({2, "two"}).has_value());
    ASSERT_TRUE(map.Insert({3, "three"}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.begin();
            --it;
        },
        "");
}

TEST(MapDeathTest, StringMapEmplaceOrAbortAbortsOnOutOfMemory)
{
    PolymorphicAllocator<std::pair<const int, std::string>> allocator{GetNullMemoryResource()};
    StringMap map = StringMap::CreateOrAbort(std::less<int>{}, allocator);

    EXPECT_DEATH(map.EmplaceOrAbort(1, "one"), "");
}

TEST(MapDeathTest, GreaterMapIteratorArrowAbortsOnEndIterator)
{
    GreaterIntMap map = GreaterIntMap::CreateOrAbort(std::greater<int>{});
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.end();
            (void)it->first;
        },
        "");
}

TEST(MapDeathTest, PolicyMapIteratorIncrementAbortsOnEndIterator)
{
    PolicyIntMap map = PolicyIntMap::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    EXPECT_DEATH(
        {
            auto it = map.end();
            ++it;
        },
        "");
}

TEST(Map, ReverseIteratorTraversesInReverseOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    std::vector<int> keys{};
    for (auto it = map.rbegin(); it != map.rend(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{3, 2, 1}));
}

TEST(Map, ConstReverseIteratorTraversesInReverseOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    const Map<int, int>& cmap = map;
    std::vector<int> keys{};
    for (auto it = cmap.rbegin(); it != cmap.rend(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{3, 2, 1}));
}

TEST(Map, CRBeginCREndTraverseInReverseOrder)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    std::vector<int> keys{};
    for (auto it = map.crbegin(); it != map.crend(); ++it)
    {
        keys.push_back(it->first);
    }
    EXPECT_EQ(keys, (std::vector<int>{2, 1}));
}

TEST(Map, ReverseIteratorOnEmptyMapIsNoOp)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    EXPECT_EQ(map.rbegin(), map.rend());
    EXPECT_EQ(map.crbegin(), map.crend());
}

TEST(Map, CountReturnsOneForExistingKeyZeroOtherwise)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    EXPECT_EQ(map.count(1), 1U);
    EXPECT_EQ(map.count(2), 1U);
    EXPECT_EQ(map.count(3), 0U);
}

TEST(Map, LowerBoundFindsFirstNotLessThan)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    auto it = map.lower_bound(20);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 20);

    it = map.lower_bound(15);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 20);

    it = map.lower_bound(5);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 10);

    it = map.lower_bound(31);
    EXPECT_EQ(it, map.end());
}

TEST(Map, ConstLowerBoundFindsFirstNotLessThan)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    const Map<int, int>& cmap = map;
    auto it = cmap.lower_bound(20);
    ASSERT_NE(it, cmap.end());
    EXPECT_EQ(it->first, 20);

    it = cmap.lower_bound(25);
    ASSERT_NE(it, cmap.end());
    EXPECT_EQ(it->first, 30);
}

TEST(Map, UpperBoundFindsFirstGreaterThan)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    auto it = map.upper_bound(20);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 30);

    it = map.upper_bound(15);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 20);

    it = map.upper_bound(30);
    EXPECT_EQ(it, map.end());

    it = map.upper_bound(5);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 10);
}

TEST(Map, ConstUpperBoundFindsFirstGreaterThan)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    const Map<int, int>& cmap = map;
    auto it = cmap.upper_bound(20);
    ASSERT_NE(it, cmap.end());
    EXPECT_EQ(it->first, 30);

    it = cmap.upper_bound(30);
    EXPECT_EQ(it, cmap.end());
}

TEST(Map, EqualRangeForExistingKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    auto [first, last] = map.equal_range(20);
    ASSERT_NE(first, map.end());
    EXPECT_EQ(first->first, 20);
    ASSERT_NE(last, map.end());
    EXPECT_EQ(last->first, 30);
}

TEST(Map, EqualRangeForMissingKeyReturnsEmptyRange)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    auto [first, last] = map.equal_range(20);
    EXPECT_EQ(first, last);
    ASSERT_NE(first, map.end());
    EXPECT_EQ(first->first, 30);
}

TEST(Map, ConstEqualRangeForExistingKey)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({10, 1}).has_value());
    ASSERT_TRUE(map.Insert({20, 2}).has_value());
    ASSERT_TRUE(map.Insert({30, 3}).has_value());

    const Map<int, int>& cmap = map;
    auto [first, last] = cmap.equal_range(20);
    ASSERT_NE(first, cmap.end());
    EXPECT_EQ(first->first, 20);
    ASSERT_NE(last, cmap.end());
    EXPECT_EQ(last->first, 30);
}

TEST(Map, LowerBoundUpperBoundOnEmptyMap)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    EXPECT_EQ(map.lower_bound(1), map.end());
    EXPECT_EQ(map.upper_bound(1), map.end());

    auto [first, last] = map.equal_range(1);
    EXPECT_EQ(first, map.end());
    EXPECT_EQ(last, map.end());
}

TEST(Map, EraseByIteratorReturnsNextElement)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    auto it = map.find(2);
    ASSERT_NE(it, map.end());

    auto next = map.erase(it);
    EXPECT_EQ(map.size(), 2U);
    EXPECT_FALSE(map.contains(2));
    ASSERT_NE(next, map.end());
    EXPECT_EQ(next->first, 3);
}

TEST(Map, EraseByConstIteratorAtBegin)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto next = map.erase(map.cbegin());
    EXPECT_EQ(map.size(), 1U);
    EXPECT_FALSE(map.contains(1));
    ASSERT_NE(next, map.end());
    EXPECT_EQ(next->first, 2);
}

TEST(Map, EraseByIteratorAtLastElement)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto it = map.find(2);
    auto next = map.erase(it);
    EXPECT_EQ(map.size(), 1U);
    EXPECT_FALSE(map.contains(2));
    EXPECT_EQ(next, map.end());
}

TEST(Map, EraseByIteratorOnSingleElementMap)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());

    auto next = map.erase(map.cbegin());
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(next, map.end());
}

TEST(Map, EraseByIteratorSequentiallyRemovesAll)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 0; key < 20; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key}).has_value());
    }

    auto it = map.cbegin();
    while (it != map.cend())
    {
        it = map.erase(it);
    }
    EXPECT_TRUE(map.empty());
}

TEST(Map, EraseRangeRemovesSubset)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    for (int key = 1; key <= 5; ++key)
    {
        ASSERT_TRUE(map.Insert({key, key * 10}).has_value());
    }

    auto first = map.find(2);
    auto last = map.find(4);
    auto result = map.erase(first, last);

    EXPECT_EQ(map.size(), 3U);
    EXPECT_TRUE(map.contains(1));
    EXPECT_FALSE(map.contains(2));
    EXPECT_FALSE(map.contains(3));
    EXPECT_TRUE(map.contains(4));
    EXPECT_TRUE(map.contains(5));
    ASSERT_NE(result, map.end());
    EXPECT_EQ(result->first, 4);
}

TEST(Map, EraseRangeEntireMap)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());
    ASSERT_TRUE(map.Insert({3, 30}).has_value());

    auto result = map.erase(map.cbegin(), map.cend());
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(result, map.end());
}

TEST(Map, EraseEmptyRangeIsNoOp)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    auto it = map.find(2);
    auto result = map.erase(it, it);
    EXPECT_EQ(map.size(), 2U);
    EXPECT_EQ(result->first, 2);
}

TEST(Map, EqualityOperatorOnEqualMaps)
{
    Map<int, int> a = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(a.Insert({1, 10}).has_value());
    ASSERT_TRUE(a.Insert({2, 20}).has_value());

    Map<int, int> b = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(b.Insert({1, 10}).has_value());
    ASSERT_TRUE(b.Insert({2, 20}).has_value());

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(Map, EqualityOperatorOnDifferentValues)
{
    Map<int, int> a = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(a.Insert({1, 10}).has_value());
    ASSERT_TRUE(a.Insert({2, 20}).has_value());

    Map<int, int> b = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(b.Insert({1, 10}).has_value());
    ASSERT_TRUE(b.Insert({2, 99}).has_value());

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(Map, EqualityOperatorOnDifferentKeys)
{
    Map<int, int> a = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(a.Insert({1, 10}).has_value());

    Map<int, int> b = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(b.Insert({2, 10}).has_value());

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(Map, EqualityOperatorOnDifferentSizes)
{
    Map<int, int> a = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(a.Insert({1, 10}).has_value());
    ASSERT_TRUE(a.Insert({2, 20}).has_value());

    Map<int, int> b = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(b.Insert({1, 10}).has_value());

    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(Map, EqualityOperatorOnEmptyMaps)
{
    Map<int, int> a = Map<int, int>::CreateOrAbort();
    Map<int, int> b = Map<int, int>::CreateOrAbort();

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(Map, EqualityWithClone)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    ASSERT_TRUE(map.Insert({2, 20}).has_value());

    Map<int, int> clone = map.CloneOrAbort();
    EXPECT_TRUE(map == clone);
}

TEST(MapDeathTest, EraseByIteratorAbortsOnEndIterator)
{
    Map<int, int> map = Map<int, int>::CreateOrAbort();
    ASSERT_TRUE(map.Insert({1, 10}).has_value());
    EXPECT_DEATH(map.erase(map.cend()), "");
}

}  // namespace
}  // namespace score::nothrow
