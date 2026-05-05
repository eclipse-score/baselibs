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
#include "score/containers/intrusive_list.h"
#include "score/quality/compiler_warnings/warnings.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

class ListElement : public score::containers::intrusive_list_element<>
{
  public:
    ListElement(std::size_t value = 0) noexcept : value_{value} {}
    const std::size_t& Get() const noexcept
    {
        return value_;
    }
    std::size_t& Get() noexcept
    {
        return value_;
    }
    void Set(std::size_t value) noexcept
    {
        value_ = value;
    }

    // here, the equivalence operator is used to check the identity of the instances, not of the values
    bool operator==(const ListElement& other) const noexcept
    {
        return this == &other;
    }

  private:
    std::size_t value_;
};

using List = score::containers::intrusive_list<ListElement>;

template <typename ListType>  // don't force ListType reference constness
void CheckEmpty(ListType& list)
{
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0U);
    EXPECT_TRUE(list.begin() == list.end());
    EXPECT_FALSE(list.begin() != list.end());
    EXPECT_TRUE(list.rbegin() == list.rend());
    EXPECT_FALSE(list.rbegin() != list.rend());
    EXPECT_TRUE(list.cbegin() == list.cend());
    EXPECT_FALSE(list.cbegin() != list.cend());
    EXPECT_TRUE(list.crbegin() == list.crend());
    EXPECT_FALSE(list.crbegin() != list.crend());
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters): Test helper
template <typename ListType>  // don't force ListType reference constness
void CheckNonEmpty(ListType& list,
                   const std::size_t expected_size,
                   const typename ListType::value_type& expected_front,
                   const typename ListType::value_type& expected_back)
// NOLINTEND(bugprone-easily-swappable-parameters): Test helper
{
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), expected_size);

    const auto expected_distance = static_cast<std::ptrdiff_t>(expected_size);

    EXPECT_TRUE(list.begin() != list.end());
    EXPECT_FALSE(list.begin() == list.end());
    EXPECT_TRUE(std::next(list.begin(), expected_distance) == list.end());
    EXPECT_FALSE(list.begin() != std::prev(list.end(), expected_distance));
    EXPECT_EQ(&expected_front, &*list.begin());
    EXPECT_EQ(&expected_back, &*std::prev(list.end()));

    EXPECT_TRUE(list.rbegin() != list.rend());
    EXPECT_FALSE(list.rbegin() == list.rend());
    EXPECT_TRUE(std::next(list.rbegin(), expected_distance) == list.rend());
    EXPECT_FALSE(list.rbegin() != std::prev(list.rend(), expected_distance));
    EXPECT_EQ(&expected_back, &*list.rbegin());
    EXPECT_EQ(&expected_front, &*std::prev(list.rend()));

    EXPECT_TRUE(list.cbegin() != list.cend());
    EXPECT_FALSE(list.cbegin() == list.cend());
    EXPECT_TRUE(std::next(list.cbegin(), expected_distance) == list.cend());
    EXPECT_FALSE(list.cbegin() != std::prev(list.cend(), expected_distance));
    EXPECT_EQ(&expected_front, &*list.cbegin());
    EXPECT_EQ(&expected_back, &*std::prev(list.cend()));

    EXPECT_TRUE(list.crbegin() != list.crend());
    EXPECT_FALSE(list.crbegin() == list.crend());
    EXPECT_TRUE(std::next(list.crbegin(), expected_distance) == list.crend());
    EXPECT_FALSE(list.crbegin() != std::prev(list.crend(), expected_distance));
    EXPECT_EQ(&expected_back, &*list.crbegin());
    EXPECT_EQ(&expected_front, &*std::prev(list.crend()));
}

TEST(IntrusiveList, NotLinkedListElement)
{
    ListElement element;
    ListElement copied_element{element};
    ListElement moved_element{std::move(element)};
    // will assert if any of them is linked
}

TEST(IntrusiveList, EmptyIterator)
{
    const List::iterator iterator;
    const List::const_iterator const_iterator;
    EXPECT_TRUE(iterator == iterator);
    EXPECT_FALSE(iterator != iterator);
    EXPECT_TRUE(const_iterator == const_iterator);
    EXPECT_FALSE(const_iterator != const_iterator);
    EXPECT_TRUE(iterator == const_iterator);
    EXPECT_FALSE(iterator != const_iterator);

    const List::reverse_iterator reverse_iterator;
    const List::const_reverse_iterator const_reverse_iterator;
    EXPECT_TRUE(reverse_iterator == reverse_iterator);
    EXPECT_FALSE(reverse_iterator != reverse_iterator);
    EXPECT_TRUE(const_reverse_iterator == const_reverse_iterator);
    EXPECT_FALSE(const_reverse_iterator != const_reverse_iterator);
    EXPECT_TRUE(reverse_iterator == const_reverse_iterator);
    EXPECT_FALSE(reverse_iterator != const_reverse_iterator);
}

TEST(IntrusiveList, EmptyList)
{
    List list;
    const List& const_list = list;

    CheckEmpty(list);
    CheckEmpty(const_list);

    List moved_list{std::move(list)};
    // NOLINTNEXTLINE(bugprone-use-after-move): testing correctness of implementation
    CheckEmpty(list);
    CheckEmpty(moved_list);
}

TEST(IntrusiveList, SingleElementMinimalChecks)
{
    std::array<ListElement, 1> elements;
    List list{elements.begin(), elements.end()};
    EXPECT_FALSE(list.empty());
    list.clear();
    CheckEmpty(list);
}

template <typename T>
inline constexpr bool refers_to_const_v = std::is_const_v<std::remove_reference_t<T>>;

static_assert(!refers_to_const_v<decltype(*std::declval<List::iterator>())>);
static_assert(!refers_to_const_v<decltype(*std::declval<List::reverse_iterator>())>);
static_assert(refers_to_const_v<decltype(*std::declval<List::const_iterator>())>);
static_assert(refers_to_const_v<decltype(*std::declval<List::const_reverse_iterator>())>);

static_assert(!refers_to_const_v<decltype(std::declval<List::iterator>()->Get())>);
static_assert(!refers_to_const_v<decltype(std::declval<List::reverse_iterator>()->Get())>);
static_assert(refers_to_const_v<decltype(std::declval<List::const_iterator>()->Get())>);
static_assert(refers_to_const_v<decltype(std::declval<List::const_reverse_iterator>()->Get())>);

TEST(IntrusiveList, SingleElementBasedIteratorChecks)
{
    constexpr std::size_t kMagic = 42;
    std::array<ListElement, 1> elements{kMagic};
    List list{elements.begin(), elements.end()};
    const auto begin_iterator = list.begin();
    const auto end_iterator = list.end();
    const auto rbegin_iterator = list.rbegin();
    const auto rend_iterator = list.rend();
    const auto cbegin_iterator = list.cbegin();
    const auto cend_iterator = list.cend();
    const auto crbegin_iterator = list.crbegin();
    const auto crend_iterator = list.crend();
    const auto to_iterator = list.iterator_to(elements[0]);
    EXPECT_TRUE(*to_iterator == elements[0]);
    EXPECT_TRUE(*begin_iterator == elements[0]);
    EXPECT_TRUE(*rbegin_iterator == elements[0]);
    EXPECT_EQ(to_iterator->Get(), kMagic);
    EXPECT_EQ(begin_iterator->Get(), kMagic);
    EXPECT_EQ(rbegin_iterator->Get(), kMagic);
    to_iterator->Set(kMagic + 1);
    EXPECT_EQ(elements[0].Get(), kMagic + 1);
    static_assert(std::is_same_v<const List::iterator, decltype(begin_iterator)>);
    static_assert(std::is_same_v<const List::iterator, decltype(end_iterator)>);
    static_assert(std::is_same_v<const List::reverse_iterator, decltype(rbegin_iterator)>);
    static_assert(std::is_same_v<const List::reverse_iterator, decltype(rend_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(cbegin_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(cend_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(crbegin_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(crend_iterator)>);
    static_assert(std::is_same_v<const List::iterator, decltype(to_iterator)>);

    const List& const_list = list;
    const auto const_begin_iterator = const_list.begin();
    const auto const_end_iterator = const_list.end();
    const auto const_rbegin_iterator = const_list.rbegin();
    const auto const_rend_iterator = const_list.rend();
    const auto const_cbegin_iterator = const_list.cbegin();
    const auto const_cend_iterator = const_list.cend();
    const auto const_crbegin_iterator = const_list.crbegin();
    const auto const_crend_iterator = const_list.crend();
    const auto const_to_iterator = const_list.iterator_to(elements[0]);
    EXPECT_TRUE(*const_to_iterator == elements[0]);
    EXPECT_TRUE(*const_begin_iterator == elements[0]);
    EXPECT_TRUE(*const_rbegin_iterator == elements[0]);
    EXPECT_EQ(const_to_iterator->Get(), kMagic + 1);
    EXPECT_EQ(const_begin_iterator->Get(), kMagic + 1);
    EXPECT_EQ(const_rbegin_iterator->Get(), kMagic + 1);
    static_assert(std::is_same_v<const List::const_iterator, decltype(const_begin_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(const_end_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(const_rbegin_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(const_rend_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(const_cbegin_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(const_cend_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(const_crbegin_iterator)>);
    static_assert(std::is_same_v<const List::const_reverse_iterator, decltype(const_crend_iterator)>);
    static_assert(std::is_same_v<const List::const_iterator, decltype(const_to_iterator)>);

    const List::iterator empty_iterator;
    const List::reverse_iterator empty_reverse_iterator;
    EXPECT_TRUE(begin_iterator != empty_iterator);
    EXPECT_FALSE(begin_iterator == empty_iterator);
    EXPECT_TRUE(end_iterator != empty_iterator);
    EXPECT_FALSE(end_iterator == empty_iterator);
    EXPECT_TRUE(rbegin_iterator != empty_reverse_iterator);
    EXPECT_FALSE(rbegin_iterator == empty_reverse_iterator);
    EXPECT_TRUE(rend_iterator != empty_reverse_iterator);
    EXPECT_FALSE(rend_iterator == empty_reverse_iterator);
    EXPECT_TRUE(cbegin_iterator != empty_iterator);
    EXPECT_FALSE(cbegin_iterator == empty_iterator);
    EXPECT_TRUE(cend_iterator != empty_iterator);
    EXPECT_FALSE(cend_iterator == empty_iterator);
    EXPECT_TRUE(crbegin_iterator != empty_reverse_iterator);
    EXPECT_FALSE(crbegin_iterator == empty_reverse_iterator);
    EXPECT_TRUE(crend_iterator != empty_reverse_iterator);
    EXPECT_FALSE(crend_iterator == empty_reverse_iterator);
    EXPECT_TRUE(to_iterator != empty_iterator);
    EXPECT_FALSE(to_iterator == empty_iterator);

    EXPECT_TRUE(begin_iterator == cbegin_iterator);
    EXPECT_TRUE(cbegin_iterator == begin_iterator);
    EXPECT_TRUE(end_iterator == cend_iterator);
    EXPECT_TRUE(cend_iterator == end_iterator);
    EXPECT_TRUE(rbegin_iterator == crbegin_iterator);
    EXPECT_TRUE(crbegin_iterator == rbegin_iterator);
    EXPECT_TRUE(rend_iterator == crend_iterator);
    EXPECT_TRUE(crend_iterator == rend_iterator);
    EXPECT_FALSE(begin_iterator != cbegin_iterator);
    EXPECT_FALSE(cbegin_iterator != begin_iterator);
    EXPECT_FALSE(end_iterator != cend_iterator);
    EXPECT_FALSE(cend_iterator != end_iterator);
    EXPECT_FALSE(rbegin_iterator != crbegin_iterator);
    EXPECT_FALSE(crbegin_iterator != rbegin_iterator);
    EXPECT_FALSE(rend_iterator != crend_iterator);
    EXPECT_FALSE(crend_iterator != rend_iterator);

    EXPECT_TRUE(const_begin_iterator == cbegin_iterator);
    EXPECT_TRUE(const_end_iterator == cend_iterator);
    EXPECT_TRUE(const_rbegin_iterator == crbegin_iterator);
    EXPECT_TRUE(const_rend_iterator == crend_iterator);
    EXPECT_TRUE(const_cbegin_iterator == cbegin_iterator);
    EXPECT_TRUE(const_cend_iterator == cend_iterator);
    EXPECT_TRUE(const_crbegin_iterator == crbegin_iterator);
    EXPECT_TRUE(const_crend_iterator == crend_iterator);

    EXPECT_TRUE(begin_iterator != end_iterator);
    EXPECT_TRUE(rbegin_iterator != rend_iterator);
    EXPECT_TRUE(cbegin_iterator != cend_iterator);
    EXPECT_TRUE(crbegin_iterator != crend_iterator);
    EXPECT_TRUE(to_iterator == begin_iterator);
    EXPECT_TRUE(to_iterator != end_iterator);

    List::iterator iterator;
    iterator = begin_iterator;
    EXPECT_TRUE(iterator++ == begin_iterator);
    EXPECT_TRUE(iterator == end_iterator);
    iterator = begin_iterator;
    EXPECT_TRUE(++iterator == end_iterator);
    EXPECT_TRUE(iterator == end_iterator);
    iterator = end_iterator;
    EXPECT_TRUE(iterator-- == end_iterator);
    EXPECT_TRUE(iterator == begin_iterator);
    iterator = end_iterator;
    EXPECT_TRUE(--iterator == begin_iterator);
    EXPECT_TRUE(iterator == begin_iterator);
    static_assert(std::is_same_v<List::iterator, decltype(iterator++)>);
    static_assert(std::is_same_v<List::iterator&, decltype(++iterator)>);
    static_assert(std::is_same_v<List::iterator, decltype(iterator--)>);
    static_assert(std::is_same_v<List::iterator&, decltype(--iterator)>);

    List::reverse_iterator reverse_iterator;
    reverse_iterator = rbegin_iterator;
    EXPECT_TRUE(reverse_iterator++ == rbegin_iterator);
    EXPECT_TRUE(reverse_iterator == rend_iterator);
    reverse_iterator = rbegin_iterator;
    EXPECT_TRUE(++reverse_iterator == rend_iterator);
    EXPECT_TRUE(reverse_iterator == rend_iterator);
    reverse_iterator = rend_iterator;
    EXPECT_TRUE(reverse_iterator-- == rend_iterator);
    EXPECT_TRUE(reverse_iterator == rbegin_iterator);
    reverse_iterator = rend_iterator;
    EXPECT_TRUE(--reverse_iterator == rbegin_iterator);
    EXPECT_TRUE(reverse_iterator == rbegin_iterator);
    static_assert(std::is_same_v<List::reverse_iterator, decltype(reverse_iterator++)>);
    static_assert(std::is_same_v<List::reverse_iterator&, decltype(++reverse_iterator)>);
    static_assert(std::is_same_v<List::reverse_iterator, decltype(reverse_iterator--)>);
    static_assert(std::is_same_v<List::reverse_iterator&, decltype(--reverse_iterator)>);

    List::const_iterator const_iterator;
    const_iterator = begin_iterator;
    EXPECT_TRUE(const_iterator++ == cbegin_iterator);
    EXPECT_TRUE(const_iterator == cend_iterator);
    const_iterator = cbegin_iterator;
    EXPECT_TRUE(++const_iterator == cend_iterator);
    EXPECT_TRUE(const_iterator == cend_iterator);
    const_iterator = cend_iterator;
    EXPECT_TRUE(const_iterator-- == cend_iterator);
    EXPECT_TRUE(const_iterator == cbegin_iterator);
    const_iterator = cend_iterator;
    EXPECT_TRUE(--const_iterator == cbegin_iterator);
    EXPECT_TRUE(const_iterator == cbegin_iterator);
    static_assert(std::is_same_v<List::const_iterator, decltype(const_iterator++)>);
    static_assert(std::is_same_v<List::const_iterator&, decltype(++const_iterator)>);
    static_assert(std::is_same_v<List::const_iterator, decltype(const_iterator--)>);
    static_assert(std::is_same_v<List::const_iterator&, decltype(--const_iterator)>);

    List::const_reverse_iterator const_reverse_iterator;
    const_reverse_iterator = crbegin_iterator;
    EXPECT_TRUE(const_reverse_iterator++ == crbegin_iterator);
    EXPECT_TRUE(const_reverse_iterator == crend_iterator);
    const_reverse_iterator = crbegin_iterator;
    EXPECT_TRUE(++const_reverse_iterator == crend_iterator);
    EXPECT_TRUE(const_reverse_iterator == crend_iterator);
    const_reverse_iterator = crend_iterator;
    EXPECT_TRUE(const_reverse_iterator-- == crend_iterator);
    EXPECT_TRUE(const_reverse_iterator == crbegin_iterator);
    const_reverse_iterator = crend_iterator;
    EXPECT_TRUE(--const_reverse_iterator == crbegin_iterator);
    EXPECT_TRUE(const_reverse_iterator == crbegin_iterator);
    static_assert(std::is_same_v<List::const_reverse_iterator, decltype(const_reverse_iterator++)>);
    static_assert(std::is_same_v<List::const_reverse_iterator&, decltype(++const_reverse_iterator)>);
    static_assert(std::is_same_v<List::const_reverse_iterator, decltype(const_reverse_iterator--)>);
    static_assert(std::is_same_v<List::const_reverse_iterator&, decltype(--const_reverse_iterator)>);

    list.clear();
}

TEST(IntrusiveList, SingleElementInsertRemoveChecks)
{
    List list;
    const List& const_list = list;
    ListElement front_back;

    list.push_back(front_back);
    CheckNonEmpty(list, 1, front_back, front_back);
    CheckNonEmpty(const_list, 1, front_back, front_back);
    list.pop_back();
    CheckEmpty(list);

    list.push_front(front_back);
    CheckNonEmpty(list, 1, front_back, front_back);
    CheckNonEmpty(const_list, 1, front_back, front_back);
    list.pop_front();
    CheckEmpty(const_list);

    list.push_front(front_back);
    List moved_list{std::move(list)};
    // NOLINTNEXTLINE(bugprone-use-after-move): testing correctness of implementation
    CheckEmpty(list);
    CheckNonEmpty(moved_list, 1, front_back, front_back);
    moved_list.clear();
    CheckEmpty(moved_list);

    list.insert(list.end(), front_back);
    CheckNonEmpty(list, 1, front_back, front_back);
    list.remove(front_back);
    CheckEmpty(list);

    list.insert(list.begin(), front_back);
    CheckNonEmpty(list, 1, front_back, front_back);
    list.remove_if([](auto&&) noexcept {
        return true;
    });
    CheckEmpty(list);

    list.insert(list.begin(), front_back);
    ListElement copied_front_back{front_back};
    CheckNonEmpty(list, 1, front_back, front_back);
    ListElement moved_front_back{std::move(front_back)};
    CheckNonEmpty(list, 1, moved_front_back, moved_front_back);
    list.remove(moved_front_back);
    CheckEmpty(list);
}

TEST(IntrusiveList, TwoElementsInsertRemoveChecks)
{
    List list;
    const List& const_list = list;
    ListElement front;
    ListElement back;

    list.push_back(front);
    list.push_back(back);
    CheckNonEmpty(list, 2, front, back);
    CheckNonEmpty(const_list, 2, front, back);
    list.pop_back();
    CheckNonEmpty(list, 1, front, front);
    list.pop_back();
    CheckEmpty(list);

    list.push_front(back);
    list.push_front(front);
    CheckNonEmpty(list, 2, front, back);
    CheckNonEmpty(const_list, 2, front, back);
    list.pop_front();
    CheckNonEmpty(list, 1, back, back);
    list.pop_front();
    CheckEmpty(const_list);

    list.push_back(back);
    list.push_front(front);
    CheckNonEmpty(list, 2, front, back);
    CheckNonEmpty(const_list, 2, front, back);
    list.pop_back();
    list.pop_front();
    CheckEmpty(list);

    list.push_front(front);
    list.push_back(back);
    CheckNonEmpty(list, 2, front, back);
    CheckNonEmpty(const_list, 2, front, back);
    List moved_list{std::move(list)};
    CheckEmpty(const_list);
    CheckNonEmpty(moved_list, 2, front, back);
    moved_list.pop_front();
    moved_list.pop_back();
    CheckEmpty(moved_list);

    // NOLINTNEXTLINE(bugprone-use-after-move): testing correctness of implementation
    list.insert(list.end(), front);
    list.insert(list.end(), back);
    CheckNonEmpty(list, 2, front, back);
    list.remove(front);
    list.remove(back);

    list.insert(list.begin(), back);
    list.insert(list.begin(), front);
    CheckNonEmpty(list, 2, front, back);
    list.remove_if([](auto&&) noexcept {
        return true;
    });

    list.insert(std::next(list.insert(list.end(), front)), back);
    CheckNonEmpty(list, 2, front, back);
    list.clear();

    list.insert(list.insert(list.begin(), back), front);
    CheckNonEmpty(list, 2, front, back);
    list.clear();
}

TEST(IntrusiveList, SixElementsInsertRemoveChecks)
{
    constexpr std::size_t num_elements = 6;
    std::array<ListElement, num_elements> elements;
    List list{elements.begin(), elements.end()};
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    list.assign(elements.rbegin(), elements.rend());
    CheckNonEmpty(list, num_elements, elements[num_elements - 1], elements[0]);

    // "no auto-unlink" is guaranteed
    CheckNonEmpty(list, num_elements, elements[num_elements - 1], elements[0]);
    list.clear();
    CheckEmpty(list);

    list.insert(list.begin(), elements.begin(), elements.end());
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    list.insert(list.end(), elements.begin(), elements.end());
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    auto half_point_from = std::next(elements.begin(), num_elements / 2);

    list.insert(list.begin(), elements.begin(), half_point_from);
    list.insert(list.end(), half_point_from, elements.end());
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    auto half_point_to = list.insert(list.end(), half_point_from, elements.end());
    list.insert(half_point_to, elements.begin(), half_point_from);
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    auto third_point_from = std::next(elements.begin(), num_elements / 3);
    auto two_third_point_from = std::next(elements.begin(), num_elements / 3 * 2);

    list.insert(list.begin(), elements.begin(), half_point_from);
    list.insert(list.end(), half_point_from, elements.end());
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();
    CheckEmpty(list);

    list.insert(list.begin(), elements.begin(), third_point_from);
    auto two_third_point_to = list.insert(list.end(), two_third_point_from, elements.end());
    list.insert(two_third_point_to, third_point_from, two_third_point_from);
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    EXPECT_TRUE(
        std::equal(elements.begin(), elements.end(), list.begin(), list.end(), [](const auto& l, const auto& r) {
            return &l == &r;
        }));
    list.remove(elements[0]);
    CheckNonEmpty(list, num_elements - 1, elements[1], elements[num_elements - 1]);

    List moved_list{std::move(list)};
    // NOLINTNEXTLINE(bugprone-use-after-move): testing correctness of implementation
    CheckEmpty(list);
    CheckNonEmpty(moved_list, num_elements - 1, elements[1], elements[num_elements - 1]);

    moved_list.remove_if([](auto&&) noexcept {
        return true;
    });
    CheckEmpty(moved_list);
}

DISABLE_WARNING_PUSH
DISABLE_WARNING_SELF_MOVE  // testing correctness of implementation

TEST(IntrusiveList, MoveAssignmentTest)
{
    // NOLINTBEGIN(bugprone-use-after-move): testing correctness of implementation

    List list;
    auto* list_ptr = &list;
    list = std::move(*list_ptr);
    CheckEmpty(list);

    List list_from;

    ListElement front_back;
    list_from.push_back(front_back);
    list = std::move(list_from);
    CheckEmpty(list_from);
    CheckNonEmpty(list, 1, front_back, front_back);
    list_ptr = &list;
    list = std::move(*list_ptr);
    CheckNonEmpty(list, 1, front_back, front_back);

    ListElement front;
    ListElement back;
    list_from.push_back(front);
    list_from.push_back(back);
    list = std::move(list_from);
    CheckEmpty(list_from);
    CheckNonEmpty(list, 2, front, back);
    list_ptr = &list;
    list = std::move(*list_ptr);
    CheckNonEmpty(list, 2, front, back);

    constexpr std::size_t num_elements = 6;
    std::array<ListElement, num_elements> elements;
    list_from.assign(elements.begin(), elements.end());
    list = std::move(list_from);
    CheckEmpty(list_from);
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list_ptr = &list;
    list = std::move(*list_ptr);
    CheckNonEmpty(list, num_elements, elements[0], elements[num_elements - 1]);
    list.clear();

    // will assert if any of the list elements is still linked

    // NOLINTEND(bugprone-use-after-move): testing correctness of implementation
}

DISABLE_WARNING_POP  // "-Wself-move"

TEST(IntrusiveList, EraseTest)
{
    constexpr std::size_t num_elements = 6;
    std::array<ListElement, num_elements> elements;
    List list{elements.begin(), elements.end()};

    auto iterator = list.erase(list.begin());
    EXPECT_EQ(iterator, list.begin());
    CheckNonEmpty(list, num_elements - 1, elements[1], elements[num_elements - 1]);

    iterator = list.erase(std::prev(list.end(), 2), list.end());
    EXPECT_EQ(iterator, list.end());
    CheckNonEmpty(list, num_elements - 3, elements[1], elements[num_elements - 3]);

    iterator = list.erase(list.begin(), list.begin());
    EXPECT_EQ(iterator, list.begin());
    CheckNonEmpty(list, num_elements - 3, elements[1], elements[num_elements - 3]);

    iterator = list.erase(list.iterator_to(elements[2]));
    const ListElement& last = elements[num_elements - 3];
    EXPECT_EQ(iterator, list.iterator_to(last));
    CheckNonEmpty(list, num_elements - 4, elements[1], elements[num_elements - 3]);

    iterator = list.erase(list.begin(), list.end());
    EXPECT_EQ(iterator, list.end());
    CheckEmpty(list);
}

TEST(IntrusiveList, SwapTest)
{
    List list1;

    list1.swap(list1);
    CheckEmpty(list1);
    swap(list1, list1);
    CheckEmpty(list1);

    List list2;

    list1.swap(list2);
    CheckEmpty(list1);
    CheckEmpty(list2);
    swap(list1, list2);
    CheckEmpty(list1);
    CheckEmpty(list2);
    swap(list2, list1);
    CheckEmpty(list1);
    CheckEmpty(list2);

    ListElement front1;

    list1.push_front(front1);
    list1.swap(list1);
    CheckNonEmpty(list1, 1, front1, front1);

    list1.swap(list2);
    CheckEmpty(list1);
    CheckNonEmpty(list2, 1, front1, front1);
    swap(list1, list2);
    CheckNonEmpty(list1, 1, front1, front1);
    CheckEmpty(list2);

    ListElement front2;

    list2.push_front(front2);
    swap(list1, list2);
    CheckNonEmpty(list1, 1, front2, front2);
    CheckNonEmpty(list2, 1, front1, front1);

    ListElement back1;
    list2.push_back(back1);
    ListElement back2;
    list1.push_back(back2);
    swap(list1, list2);
    CheckNonEmpty(list1, 2, front1, back1);
    CheckNonEmpty(list2, 2, front2, back2);

    list1.clear();
    list2.clear();
}

TEST(IntrusiveList, DisposeTest)
{
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers): "Magic numbers" here are the test labels by themselves

    constexpr std::size_t num_elements = 6;
    std::array<ListElement, num_elements> elements{0, 1, 2, 3, 4, 5};
    std::array<ListElement, num_elements> more_elements{10, 11, 12, 13, 14, 15};
    List list{elements.begin(), elements.end()};

    std::size_t index{};
    const auto disposer = [&](ListElement* e) noexcept {
        EXPECT_EQ(std::find(list.begin(), list.end(), *e), list.end());
        EXPECT_EQ(e->Get(), index++);
    };

    index = 5;
    list.pop_back_and_dispose(disposer);
    EXPECT_EQ(index, 6);

    index = 0;
    list.pop_front_and_dispose(disposer);
    EXPECT_EQ(index, 1);
    list.erase_and_dispose(list.begin(), disposer);
    EXPECT_EQ(index, 2);
    list.erase_and_dispose(list.begin(), std::next(list.begin(), 2), disposer);
    EXPECT_EQ(index, 4);

    list.dispose_and_assign(disposer, more_elements.begin(), more_elements.end());
    EXPECT_EQ(index, 5);

    index = 10;
    list.remove_and_dispose(more_elements[0], disposer);
    EXPECT_EQ(index, 11);

    list.remove_and_dispose_if(
        [&](auto&& element) noexcept {
            return element.Get() < 13;
        },
        disposer);
    EXPECT_EQ(index, 13);

    list.clear_and_dispose(disposer);
    EXPECT_EQ(index, 16);

    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers): "Magic numbers" here are the test labels by themselves
}

class tag1;
class tag2
{
};

// NOLINTNEXTLINE(fuchsia-multiple-inheritance): intended use by API
class multi_element : public score::containers::intrusive_list_element<>,
                      public score::containers::intrusive_list_element<tag1>,
                      public score::containers::intrusive_list_element<tag2>
{
};

TEST(IntrusiveList, MultiTagTest)
{
    constexpr std::size_t num_elements = 6;
    std::array<multi_element, num_elements> elements;
    score::containers::intrusive_list<multi_element> no_tag_list{elements.begin(), elements.end()};
    score::containers::intrusive_list<multi_element, tag1> tag1_list{no_tag_list.begin(), no_tag_list.end()};
    score::containers::intrusive_list<multi_element, tag2> tag2_list;

    CheckNonEmpty(no_tag_list, num_elements, elements[0], elements[num_elements - 1]);
    CheckNonEmpty(tag1_list, num_elements, elements[0], elements[num_elements - 1]);
    CheckEmpty(tag2_list);

    tag2_list.insert(tag2_list.begin(), tag1_list.rbegin(), tag1_list.rend());
    CheckNonEmpty(no_tag_list, num_elements, elements[0], elements[num_elements - 1]);
    CheckNonEmpty(tag1_list, num_elements, elements[0], elements[num_elements - 1]);
    CheckNonEmpty(tag2_list, num_elements, elements[num_elements - 1], elements[0]);

    no_tag_list.clear();
    CheckEmpty(no_tag_list);
    CheckNonEmpty(tag1_list, num_elements, elements[0], elements[num_elements - 1]);
    CheckNonEmpty(tag2_list, num_elements, elements[num_elements - 1], elements[0]);

    tag1_list.clear();
    CheckEmpty(no_tag_list);
    CheckEmpty(tag1_list);
    CheckNonEmpty(tag2_list, num_elements, elements[num_elements - 1], elements[0]);

    tag2_list.clear();
}

}  // namespace
