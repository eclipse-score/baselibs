/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///

#include <score/circular_buffer.hpp>
#include <score/circular_buffer.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <array>
#include <cstdint>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{
struct data
{
public:
    explicit data(std::int32_t* destructor_tracker) : destructor_tracker_{destructor_tracker} {}

    data(const data& other)
        : destructor_tracker_{other.destructor_tracker_}
        , copy_constructs_{other.copy_constructs_ + 1}
        , copy_assignments_{other.copy_assignments_}
        , move_constructs_{other.move_constructs_}
        , move_assignments_{other.move_assignments_}
    {
    }

    data& operator=(const data& other)
    {
        destructor_tracker_ = other.destructor_tracker_;
        copy_constructs_ = other.copy_constructs_;
        copy_assignments_ = other.copy_assignments_ + 1;
        move_constructs_ = other.move_constructs_;
        move_assignments_ = other.move_assignments_;
        return *this;
    }
    data(data&& other) noexcept
        : destructor_tracker_{other.destructor_tracker_}
        , copy_constructs_{other.copy_constructs_}
        , copy_assignments_{other.copy_assignments_}
        , move_constructs_{other.move_constructs_ + 1}
        , move_assignments_{other.move_assignments_}
    {
    }
    data& operator=(const data&& other)
    {
        destructor_tracker_ = other.destructor_tracker_;
        copy_constructs_ = other.copy_constructs_;
        copy_assignments_ = other.copy_assignments_;
        move_constructs_ = other.move_constructs_;
        move_assignments_ = other.move_assignments_ + 1;
        return *this;
    }

    ~data()
    {
        if (destructor_tracker_)
        {
            ++*destructor_tracker_;
        }
    }

    std::int32_t move_constructs() const { return move_constructs_; }
    std::int32_t move_assignments() const { return move_assignments_; }
    std::int32_t copy_constructs() const { return copy_constructs_; }
    std::int32_t copy_assignments() const { return copy_assignments_; }

private:
    std::int32_t* destructor_tracker_;
    std::int32_t copy_constructs_{0};
    std::int32_t copy_assignments_{0};
    std::int32_t move_constructs_{0};
    std::int32_t move_assignments_{0};
};

struct copy_bomb
{
    copy_bomb() = default;
    copy_bomb(const copy_bomb& other) : shall_throw{other.shall_throw}
    {
        if (shall_throw)
        {
            throw std::exception{};
        }
    }
    copy_bomb(copy_bomb&&) = default;
    copy_bomb& operator=(const copy_bomb&) = default;
    copy_bomb& operator=(copy_bomb&&) = default;
    ~copy_bomb() noexcept = default;

    bool shall_throw{false};
};

struct move_bomb
{
    move_bomb() = default;
    move_bomb(const move_bomb&) = default;
    move_bomb(move_bomb&& other) : shall_throw{other.shall_throw}
    {
        if (shall_throw)
        {
            throw std::exception{};
        }
    }
    move_bomb& operator=(const move_bomb&) = default;
    move_bomb& operator=(move_bomb&&) = default;
    ~move_bomb() noexcept = default;

    bool shall_throw{false};
};

const std::size_t test_max_size{5U};

/// \test The copy constructor shall correctly handle the copied-to, uninintialized memory by calling the copy
/// constructor of the circular_buffer::value type.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, copy_constructor)
{
    const std::size_t vector_size{10U};
    circular_buffer<data, vector_size> vector;

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        vector.emplace_back(nullptr);
    }

    circular_buffer<data, vector_size> second_vector{vector};

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        EXPECT_EQ(1, second_vector[i].copy_constructs());
        EXPECT_EQ(0, second_vector[i].move_constructs());
        EXPECT_EQ(0, second_vector[i].copy_assignments());
        EXPECT_EQ(0, second_vector[i].move_assignments());
    }

    EXPECT_EQ(vector_size, vector.size());
    EXPECT_EQ(vector_size, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, copy_constructor_while_element_throws)
{
    score::cpp::circular_buffer<copy_bomb, 3U> buffer{};
    buffer.emplace_back();
    buffer.emplace_back();
    buffer.emplace_back();
    score::cpp::at(buffer, 1).shall_throw = true;
    score::cpp::circular_buffer<copy_bomb, 3U> second_buffer(buffer);

    EXPECT_EQ(3U, buffer.size());
    EXPECT_EQ(0U, second_buffer.size());
}

/// \test The move constructor shall correctly handle the moved-to, uninintialized memory by calling the move
/// constructor of the circular_buffer::value type.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, move_constructor)
{
    const std::size_t vector_size{10U};
    circular_buffer<data, vector_size> vector;

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        vector.emplace_back(nullptr);
    }

    circular_buffer<data, vector_size> second_vector{std::move(vector)};

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        EXPECT_EQ(0, second_vector[i].copy_constructs());
        EXPECT_EQ(1, second_vector[i].move_constructs());
        EXPECT_EQ(0, second_vector[i].copy_assignments());
        EXPECT_EQ(0, second_vector[i].move_assignments());
    }

    EXPECT_EQ(vector_size, vector.size());
    EXPECT_EQ(vector_size, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, move_constructor_while_element_throws)
{
    score::cpp::circular_buffer<move_bomb, 3U> buffer{};
    buffer.emplace_back();
    buffer.emplace_back();
    buffer.emplace_back();
    score::cpp::at(buffer, 1).shall_throw = true;
    score::cpp::circular_buffer<move_bomb, 3U> second_buffer(std::move(buffer));

    EXPECT_EQ(3U, buffer.size());
    EXPECT_EQ(0U, second_buffer.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, copy_assignment)
{
    std::array<std::int32_t, 4U> destructor_tracker{{0, 0, 0, 0}};

    circular_buffer<data, 4U> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    source_vector.emplace_back(&destructor_tracker[1]);

    circular_buffer<data, 4U> target_vector;
    target_vector.emplace_back(nullptr);
    target_vector.emplace_back(&destructor_tracker[2]);
    target_vector.emplace_back(&destructor_tracker[3]);
    target_vector.pop_front();

    target_vector = source_vector;

    EXPECT_EQ(2, target_vector.size());
    EXPECT_EQ(2, source_vector.size());

    EXPECT_EQ(1, target_vector[0].copy_constructs());
    EXPECT_EQ(0, target_vector[0].move_constructs());
    EXPECT_EQ(0, target_vector[0].copy_assignments());
    EXPECT_EQ(0, target_vector[0].move_assignments());

    EXPECT_EQ(1, target_vector[1].copy_constructs());
    EXPECT_EQ(0, target_vector[1].move_constructs());
    EXPECT_EQ(0, target_vector[1].copy_assignments());
    EXPECT_EQ(0, target_vector[1].move_assignments());

    std::array<std::int32_t, 4U> expected_destructor_calls{{0, 0, 1, 1}};
    EXPECT_EQ(expected_destructor_calls, destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, copy_assignment_on_identity)
{
    std::array<std::int32_t, 1U> destructor_tracker{{0}};

    circular_buffer<data, 1U> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    source_vector = static_cast<const circular_buffer<data, 1U>&>(
        source_vector); // `static_cast` to silence self-assign compiler warning
    EXPECT_EQ(0, source_vector[0].copy_constructs());
    EXPECT_EQ(0, source_vector[0].move_constructs());
    EXPECT_EQ(0, source_vector[0].copy_assignments());
    EXPECT_EQ(0, source_vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, copy_assignment_while_element_throws)
{
    score::cpp::circular_buffer<copy_bomb, 3U> buffer{};
    buffer.emplace_back();
    buffer.emplace_back();
    buffer.emplace_back();
    score::cpp::at(buffer, 1).shall_throw = true;
    score::cpp::circular_buffer<copy_bomb, 3U> second_buffer;
    second_buffer = buffer;

    EXPECT_EQ(3U, buffer.size());
    EXPECT_EQ(0U, second_buffer.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, move_assignment)
{
    std::array<std::int32_t, 4U> destructor_tracker{{0, 0, 0, 0}};

    circular_buffer<data, 4U> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    source_vector.emplace_back(&destructor_tracker[1]);

    circular_buffer<data, 4U> target_vector;
    target_vector.emplace_back(nullptr);
    target_vector.emplace_back(&destructor_tracker[2]);
    target_vector.emplace_back(&destructor_tracker[3]);
    target_vector.pop_front();

    target_vector = std::move(source_vector);

    EXPECT_EQ(2, target_vector.size());
    EXPECT_EQ(2, source_vector.size());

    EXPECT_EQ(0, target_vector[0].copy_constructs());
    EXPECT_EQ(1, target_vector[0].move_constructs());
    EXPECT_EQ(0, target_vector[0].copy_assignments());
    EXPECT_EQ(0, target_vector[0].move_assignments());

    EXPECT_EQ(0, target_vector[1].copy_constructs());
    EXPECT_EQ(1, target_vector[1].move_constructs());
    EXPECT_EQ(0, target_vector[1].copy_assignments());
    EXPECT_EQ(0, target_vector[1].move_assignments());

    std::array<std::int32_t, 4> expected_destructor_calls{{0, 0, 1, 1}};
    EXPECT_EQ(expected_destructor_calls, destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, move_assignment_while_element_throws)
{
    score::cpp::circular_buffer<move_bomb, 3U> buffer{};
    buffer.emplace_back();
    buffer.emplace_back();
    buffer.emplace_back();
    score::cpp::at(buffer, 1).shall_throw = true;
    score::cpp::circular_buffer<move_bomb, 3U> second_buffer;
    second_buffer = std::move(buffer);

    EXPECT_EQ(3U, buffer.size());
    EXPECT_EQ(0U, second_buffer.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, move_assignment_on_identity)
{
    std::array<std::int32_t, 1> destructor_tracker{{0}};

    circular_buffer<data, 1U> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    auto& source_vector_reference =
        source_vector; // using an additional reference to silence warning for intentional self-assign
    source_vector = std::move(source_vector_reference);
    EXPECT_EQ(0, source_vector[0].copy_constructs());
    EXPECT_EQ(0, source_vector[0].move_constructs());
    EXPECT_EQ(0, source_vector[0].copy_assignments());
    EXPECT_EQ(0, source_vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, push)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    EXPECT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);
    EXPECT_EQ(buffer.back(), 3);

    buffer.push_back(4);
    buffer.push_back(5);
    buffer.push_back(6);
    EXPECT_EQ(buffer.size(), 5U);
    EXPECT_EQ(buffer[0], 2);
    EXPECT_EQ(buffer[1], 3);
    EXPECT_EQ(buffer[2], 4);
    EXPECT_EQ(buffer[3], 5);
    EXPECT_EQ(buffer[4], 6);
    EXPECT_EQ(buffer.back(), 6);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, push_when_lvalue)
{
    std::int32_t destructor_tracker{0};
    const data lvalue_data{&destructor_tracker};
    circular_buffer<data, test_max_size> buffer{};
    buffer.push_back(lvalue_data);

    EXPECT_EQ(1U, buffer.size());
    EXPECT_EQ(1, buffer.front().copy_constructs());
    EXPECT_EQ(0, buffer.front().move_constructs());
    EXPECT_EQ(0, buffer.front().copy_assignments());
    EXPECT_EQ(0, buffer.front().move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, push_when_rvalue)
{
    std::int32_t destructor_tracker{0};
    circular_buffer<data, test_max_size> buffer{};
    buffer.push_back(data{&destructor_tracker});

    EXPECT_EQ(1U, buffer.size());
    EXPECT_EQ(0, buffer.front().copy_constructs());
    EXPECT_EQ(1, buffer.front().move_constructs());
    EXPECT_EQ(0, buffer.front().copy_assignments());
    EXPECT_EQ(0, buffer.front().move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, emplace_back_WhenDefaultConstructed)
{
    circular_buffer<std::int32_t, test_max_size> buffer;
    buffer.emplace_back();

    ASSERT_EQ(buffer.size(), 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, emplace_back_WhenTypical)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.emplace_back(1);
    buffer.emplace_back(2);
    buffer.emplace_back(3);
    EXPECT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);
    EXPECT_EQ(buffer.back(), 3);

    buffer.emplace_back(4);
    buffer.emplace_back(5);
    buffer.emplace_back(6);
    EXPECT_EQ(buffer.size(), 5U);
    EXPECT_EQ(buffer[0], 2);
    EXPECT_EQ(buffer[1], 3);
    EXPECT_EQ(buffer[2], 4);
    EXPECT_EQ(buffer[3], 5);
    EXPECT_EQ(buffer[4], 6);
    EXPECT_EQ(buffer.back(), 6);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, pop_front)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    EXPECT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);

    EXPECT_EQ(buffer.front(), 1);
    EXPECT_EQ(buffer.back(), 3);

    buffer.pop_front();
    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 3);

    buffer.pop_front();
    EXPECT_EQ(buffer.front(), 3);
    EXPECT_EQ(buffer.back(), 3);

    buffer.pop_front();
    EXPECT_TRUE(buffer.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, pop_front_GivenEmptyContainer_ExpectRemainsEmpty)
{
    circular_buffer<std::int32_t, test_max_size> buffer;
    EXPECT_TRUE(buffer.empty());
    buffer.pop_front();
    EXPECT_TRUE(buffer.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, pop_back)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);
    buffer.push_back(6);
    EXPECT_EQ(buffer.size(), 5U);

    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 6);

    buffer.pop_back();
    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 5);

    buffer.pop_back();
    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 4);

    buffer.pop_back();
    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 3);

    buffer.pop_back();
    EXPECT_EQ(buffer.front(), 2);
    EXPECT_EQ(buffer.back(), 2);

    buffer.pop_back();
    EXPECT_TRUE(buffer.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, pop_back_GivenEmptyContainer_ExpectRemainsEmpty)
{
    circular_buffer<std::int32_t, test_max_size> buffer;
    EXPECT_TRUE(buffer.empty());
    buffer.pop_back();
    EXPECT_TRUE(buffer.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, iterator)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.cbegin(), buffer.cend());
    EXPECT_EQ(buffer.begin(), buffer.end());

    for (std::int32_t i{0}; i < 6; ++i)
    {
        buffer.push_back(i + 1);
        const std::ptrdiff_t size{std::distance(buffer.cbegin(), buffer.cend())};
        EXPECT_EQ(size, std::min(i + 1, static_cast<std::int32_t>(test_max_size)));
        EXPECT_GE(size, 0);
        EXPECT_EQ(buffer.size(), static_cast<std::size_t>(size));
    }

    std::size_t count{0U};
    for (auto it = buffer.cbegin(); it != buffer.cend(); ++it)
    {
        const std::ptrdiff_t index{std::distance(buffer.cbegin(), it)};
        EXPECT_EQ(*it, index + 2) << "Index: " << index;
        EXPECT_EQ(*(it.operator->()), index + 2) << "Index: " << index;
        ++count;
    }
    EXPECT_EQ(count, test_max_size);

    count = 0U;
    for (auto it = buffer.begin(); it != buffer.end(); ++it)
    {
        const std::ptrdiff_t index{std::distance(buffer.begin(), it)};
        EXPECT_EQ(*it, index + 2) << "Index: " << index;
        EXPECT_EQ(*(it.operator->()), index + 2) << "Index: " << index;
        ++count;
    }
    EXPECT_EQ(count, test_max_size);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, reverse_iterator)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.crbegin(), buffer.crend());
    EXPECT_EQ(buffer.rbegin(), buffer.rend());
    EXPECT_EQ(score::cpp::as_const(buffer).rbegin(), score::cpp::as_const(buffer).rend());

    // buffer setup and buffer size check
    const auto number_of_iterations = static_cast<std::int32_t>(test_max_size);
    for (std::int32_t i{0}; i < number_of_iterations; ++i)
    {
        buffer.push_back(i + 1);
        const std::ptrdiff_t size{std::distance(buffer.crbegin(), buffer.crend())};
        EXPECT_EQ(size, std::min(i + 1, number_of_iterations));
        EXPECT_GE(size, 0);
        EXPECT_EQ(buffer.size(), static_cast<std::size_t>(size));
    }

    std::size_t count{0U};
    for (auto it = buffer.crbegin(); it != buffer.crend(); ++it)
    {
        const std::ptrdiff_t index{std::distance(it, buffer.crend())};
        EXPECT_EQ(*it, index) << "Index: " << index;
        EXPECT_EQ(*(it.operator->()), index) << "Index: " << index;
        ++count;
    }
    EXPECT_EQ(count, test_max_size);

    count = 0U;
    for (auto it = score::cpp::as_const(buffer).rbegin(); it != score::cpp::as_const(buffer).rend(); ++it)
    {
        const std::ptrdiff_t index{std::distance(it, score::cpp::as_const(buffer).rend())};
        EXPECT_EQ(*it, index) << "Index: " << index;
        EXPECT_EQ(*(it.operator->()), index) << "Index: " << index;
        ++count;
    }
    EXPECT_EQ(count, test_max_size);

    count = 0U;
    for (auto it = buffer.rbegin(); it != buffer.rend(); ++it)
    {
        const std::ptrdiff_t index{std::distance(it, buffer.rend())};
        EXPECT_EQ(*it, index) << "Index: " << index;
        EXPECT_EQ(*(it.operator->()), index) << "Index: " << index;
        ++count;
    }
    EXPECT_EQ(count, test_max_size);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, difference_type_matches_with_iterator)
{
    using type = score::cpp::circular_buffer<std::int32_t, test_max_size>;

    static_assert(std::is_same_v<type::difference_type, std::iterator_traits<type::iterator>::difference_type>);
    static_assert(std::is_same_v<type::difference_type, std::iterator_traits<type::const_iterator>::difference_type>);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, const_correctness_when_non_const_container)
{
    circular_buffer<std::int32_t, test_max_size> buffer;
    static_assert(std::is_same<std::int32_t&, decltype(buffer[0U])>::value, "failure");
    static_assert(std::is_same<std::int32_t&, decltype(*buffer.begin())>::value, "failure");
    static_assert(std::is_same<std::int32_t&, decltype(*buffer.end())>::value, "failure");
    static_assert(std::is_same<std::int32_t&, decltype(*buffer.rbegin())>::value, "failure");
    static_assert(std::is_same<std::int32_t&, decltype(*buffer.rend())>::value, "failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, const_correctness_when_const_container)
{
    const circular_buffer<std::int32_t, test_max_size> buffer;
    static_assert(std::is_same<const std::int32_t&, decltype(buffer[0U])>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.begin())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.end())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.rbegin())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.rend())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.cbegin())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.cend())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.crbegin())>::value, "failure");
    static_assert(std::is_same<const std::int32_t&, decltype(*buffer.crend())>::value, "failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, erase_from_begin)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);

    EXPECT_EQ(buffer.size(), 5U);

    const auto res = buffer.erase(buffer.begin(), buffer.begin() + 2);

    EXPECT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 3);
    EXPECT_EQ(buffer[1], 4);
    EXPECT_EQ(buffer[2], 5);
    EXPECT_EQ(*res, 3);
    EXPECT_EQ(res, buffer.begin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, erase_to_end)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);

    EXPECT_EQ(buffer.size(), 5U);

    const auto res = buffer.erase(buffer.end() - 2, buffer.end());

    EXPECT_EQ(buffer.size(), 3U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer[2], 3);
    EXPECT_EQ(res, buffer.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, erase_in_between)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);

    EXPECT_EQ(buffer.size(), 5U);

    const auto res = buffer.erase(buffer.begin() + 1, buffer.end() - 1);

    EXPECT_EQ(buffer.size(), 2U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 5);
    EXPECT_EQ(*res, 5);
    EXPECT_EQ(res, buffer.end() - 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, erase_single_argument)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);

    EXPECT_EQ(buffer.size(), 5U);

    const auto res = buffer.erase(buffer.begin() + 1);

    EXPECT_EQ(buffer.size(), 4U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(buffer[1], 3);
    EXPECT_EQ(buffer[2], 4);
    EXPECT_EQ(buffer[3], 5);
    EXPECT_EQ(*res, 3);
    EXPECT_EQ(res, buffer.begin() + 1);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, erase_nothing)
{
    circular_buffer<std::int32_t, test_max_size> buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);

    EXPECT_EQ(buffer.size(), 1U);

    const auto res = buffer.erase(buffer.begin(), buffer.begin());

    EXPECT_EQ(buffer.size(), 1U);
    EXPECT_EQ(buffer[0], 1);
    EXPECT_EQ(res, buffer.begin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circula_buffer_test, erase_when_precondition_violated)
{
    circular_buffer<std::int32_t, test_max_size> buffer;
    circular_buffer<std::int32_t, test_max_size> other_buffer;

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_TRUE(buffer.empty());

    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);

    EXPECT_EQ(buffer.size(), 5U);

    {
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(buffer.erase(buffer.begin(), other_buffer.end()));
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(buffer.erase(other_buffer.begin(), buffer.end()));
    }

    {
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(buffer.erase(buffer.begin(), buffer.end() + 1));
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(buffer.erase(buffer.begin() - 1, buffer.end()));
    }

    {
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(buffer.erase(buffer.end(), buffer.begin()));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, capacity)
{
    {
        using capacity_one_buffer = circular_buffer<std::int32_t, 1U>;
        static_assert(1U == capacity_one_buffer::capacity(), "");
    }
    {
        circular_buffer<std::int32_t, 10U> buffer;
        EXPECT_EQ(10, buffer.capacity());
        buffer.push_back(1);
        buffer.push_back(2);
        EXPECT_EQ(10, buffer.capacity());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17014595
TEST(circular_buffer_test, max_size)
{
    {
        using max_size_one_buffer = circular_buffer<std::int32_t, 1U>;
        static_assert(1U == max_size_one_buffer::max_size(), "");
    }
    {
        circular_buffer<std::int32_t, 10U> buffer;
        EXPECT_EQ(10, buffer.max_size());
        buffer.push_back(1);
        buffer.push_back(2);
        EXPECT_EQ(10, buffer.max_size());
    }
}

} // namespace
} // namespace score::cpp
