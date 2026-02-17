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

#include <score/inplace_vector.hpp>
#include <score/inplace_vector.hpp> // test include guard

#include <score/assert_support.hpp>
#include <score/type_traits.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct data
{
public:
    data() : data{nullptr} {}

    explicit data(std::int32_t* destructor_tracker) : destructor_tracker_{destructor_tracker}
    {
        if (destructor_tracker_ != nullptr)
        {
            *destructor_tracker_ = 0;
        }
    }

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
        if (destructor_tracker_ != nullptr)
        {
            *destructor_tracker_ += 1;
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

struct no_copy_construct
{
    no_copy_construct() = default;
    no_copy_construct(const no_copy_construct&) = delete;
    no_copy_construct(no_copy_construct&& other) = delete;
    no_copy_construct& operator=(const no_copy_construct&) = delete;
    no_copy_construct& operator=(no_copy_construct&&) = delete;
    ~no_copy_construct() noexcept = default;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_default_constructed)
{
    const std::size_t size{1U};
    const score::cpp::inplace_vector<std::int32_t, size> vector{};
    EXPECT_EQ(0U, vector.size());
    EXPECT_TRUE(vector.empty());
    EXPECT_FALSE(vector.full());
}

struct default_construction_count
{
    default_construction_count() { ++count; }
    static std::int32_t count;
};
std::int32_t default_construction_count::count{0};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_default_constructed_expect_that_elements_are_not_constructed)
{
    default_construction_count::count = 0;
    const score::cpp::inplace_vector<default_construction_count, 1U> vector{};
    EXPECT_EQ(0, default_construction_count::count);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor)
{
    const std::size_t size{1U};
    const score::cpp::inplace_vector<std::int32_t, size> vector(size, 23);
    EXPECT_EQ(23, vector[0]);
    EXPECT_EQ(size, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_n_default_elements)
{
    const std::size_t size{2U};
    const score::cpp::inplace_vector<std::int32_t, size> vector(size);
    EXPECT_EQ(0, vector[0]);
    EXPECT_EQ(0, vector[1]);
    EXPECT_EQ(size, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_n_default_elements_no_copy_construction)
{
    const std::size_t size{1U};
    const score::cpp::inplace_vector<no_copy_construct, size> vector(size);
    EXPECT_EQ(size, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_n_default_elements_and_precondition_violated)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::inplace_vector<std::int32_t, 1>(2)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, constructor_when_precondition_violated)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::inplace_vector<std::int32_t, 1>(2, 23)));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_constructor)
{
    const std::size_t vector_size{10U};
    score::cpp::inplace_vector<data, vector_size> vector;

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        vector.emplace_back(nullptr);
    }

    score::cpp::inplace_vector<data, vector_size> second_vector(vector);

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
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_constructor_while_element_throws)
{
    score::cpp::inplace_vector<copy_bomb, 3U> vector(3);
    score::cpp::at(vector, 1).shall_throw = true;
    score::cpp::inplace_vector<copy_bomb, 3U> second_vector(vector);

    EXPECT_EQ(3U, vector.size());
    EXPECT_EQ(0U, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_constructor)
{
    const std::size_t vector_size{10U};
    score::cpp::inplace_vector<data, vector_size> vector;

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        vector.emplace_back(nullptr);
    }

    const score::cpp::inplace_vector<data, vector_size> second_vector(std::move(vector));

    for (std::size_t i{0U}; i < vector_size; ++i)
    {
        EXPECT_EQ(0, second_vector[i].copy_constructs());
        EXPECT_EQ(1, second_vector[i].move_constructs());
        EXPECT_EQ(0, second_vector[i].copy_assignments());
        EXPECT_EQ(0, second_vector[i].move_assignments());
    }

    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(0U, vector.size());
    EXPECT_EQ(vector_size, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_constructor_while_element_throws)
{
    score::cpp::inplace_vector<move_bomb, 3U> vector(3);
    score::cpp::at(vector, 1).shall_throw = true;
    score::cpp::inplace_vector<move_bomb, 3U> second_vector(std::move(vector));

    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(0U, vector.size());
    EXPECT_EQ(0U, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_assignment_target_larger)
{
    const std::size_t vector_size{2U};
    std::array<std::int32_t, 2 * vector_size> destructor_tracker{{0, 0, 0, 0}};

    score::cpp::inplace_vector<data, vector_size> target_vector;
    target_vector.emplace_back(&destructor_tracker[0]);
    target_vector.emplace_back(&destructor_tracker[1]);

    score::cpp::inplace_vector<data, vector_size> source_vector;
    source_vector.emplace_back(&destructor_tracker[2]);

    target_vector = source_vector;

    EXPECT_EQ(1U, source_vector.size());
    EXPECT_EQ(1U, target_vector.size());

    EXPECT_EQ(1, target_vector[0].copy_constructs());

    std::array<std::int32_t, 2 * vector_size> expected_destructor_tracker{{1, 1, 0, 0}};
    EXPECT_EQ(destructor_tracker, expected_destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_assignment_source_larger)
{
    const std::size_t vector_size{2U};
    std::array<std::int32_t, 2 * vector_size> destructor_tracker{{0, 0, 0, 0}};

    score::cpp::inplace_vector<data, vector_size> target_vector;
    target_vector.emplace_back(&destructor_tracker[2]);

    score::cpp::inplace_vector<data, vector_size> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    source_vector.emplace_back(&destructor_tracker[1]);

    target_vector = source_vector;

    EXPECT_EQ(2U, source_vector.size());
    EXPECT_EQ(2U, target_vector.size());

    EXPECT_EQ(1, target_vector[0].copy_constructs());
    EXPECT_EQ(1, target_vector[1].copy_constructs());

    std::array<std::int32_t, 2 * vector_size> expected_destructor_tracker{{0, 0, 1, 0}};
    EXPECT_EQ(destructor_tracker, expected_destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_assignment_while_element_throws)
{
    score::cpp::inplace_vector<copy_bomb, 3U> vector(3);
    score::cpp::at(vector, 1).shall_throw = true;
    score::cpp::inplace_vector<copy_bomb, 3U> second_vector;
    second_vector = vector;

    EXPECT_EQ(3U, vector.size());
    EXPECT_EQ(0U, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, copy_self_assignment)
{
    std::array<std::int32_t, 1> destructor_tracker{{0}};
    score::cpp::inplace_vector<data, 2> vector{};
    vector.emplace_back(&destructor_tracker[0]);
    vector = static_cast<const score::cpp::inplace_vector<data, 2>&>(
        vector); // `static_cast` to silence self-assign compiler warning
    EXPECT_EQ(0, vector[0].copy_constructs());
    EXPECT_EQ(0, vector[0].move_constructs());
    EXPECT_EQ(0, vector[0].copy_assignments());
    EXPECT_EQ(0, vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_assignment_target_larger)
{
    const std::size_t vector_size{2U};
    std::array<std::int32_t, 2 * vector_size> destructor_tracker{{0, 0, 0, 0}};

    score::cpp::inplace_vector<data, vector_size> target_vector;
    target_vector.emplace_back(&(destructor_tracker[0]));
    target_vector.emplace_back(&(destructor_tracker[1]));

    score::cpp::inplace_vector<data, vector_size> source_vector;
    source_vector.emplace_back(&(destructor_tracker[2]));

    target_vector = std::move(source_vector);

    EXPECT_TRUE(source_vector.empty());
    EXPECT_EQ(0U, source_vector.size());
    EXPECT_EQ(1U, target_vector.size());

    EXPECT_EQ(1, target_vector[0].move_constructs());

    std::array<std::int32_t, 2 * vector_size> expected_destructor_tracker{{1, 1, 1, 0}};
    EXPECT_EQ(destructor_tracker, expected_destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_assignment_source_larger)
{
    const std::size_t vector_size{2U};
    std::array<std::int32_t, 2 * vector_size> destructor_tracker{{0, 0, 0, 0}};

    score::cpp::inplace_vector<data, vector_size> target_vector;
    target_vector.emplace_back(&destructor_tracker[2]);

    score::cpp::inplace_vector<data, vector_size> source_vector;
    source_vector.emplace_back(&destructor_tracker[0]);
    source_vector.emplace_back(&destructor_tracker[1]);

    target_vector = std::move(source_vector);

    EXPECT_TRUE(source_vector.empty());
    EXPECT_EQ(0U, source_vector.size());
    EXPECT_EQ(2U, target_vector.size());

    EXPECT_EQ(1, target_vector[0].move_constructs());
    EXPECT_EQ(1, target_vector[1].move_constructs());

    std::array<std::int32_t, 2 * vector_size> expected_destructor_tracker{{1, 1, 1, 0}};
    EXPECT_EQ(destructor_tracker, expected_destructor_tracker);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_assignment_while_element_throws)
{
    score::cpp::inplace_vector<move_bomb, 3U> vector(3);
    score::cpp::at(vector, 1).shall_throw = true;
    score::cpp::inplace_vector<move_bomb, 3U> second_vector;
    second_vector = std::move(vector);

    EXPECT_TRUE(vector.empty());
    EXPECT_EQ(0U, vector.size());
    EXPECT_EQ(0U, second_vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_self_assignment)
{
    std::array<std::int32_t, 1> destructor_tracker{{0}};
    score::cpp::inplace_vector<data, 2> vector{};
    vector.emplace_back(&destructor_tracker[0]);
    auto& vector_reference = vector; // using an additional reference to silence warning for intentional self-assign
    vector = std::move(vector_reference);
    EXPECT_EQ(0, vector[0].copy_constructs());
    EXPECT_EQ(0, vector[0].move_constructs());
    EXPECT_EQ(0, vector[0].copy_assignments());
    EXPECT_EQ(0, vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, capacity)
{
    {
        using capacity_one_vector = score::cpp::inplace_vector<std::int32_t, 1U>;
        static_assert(1U == capacity_one_vector::capacity(), "");
    }
    {
        score::cpp::inplace_vector<std::int32_t, 10> vector{};
        EXPECT_EQ(10, vector.capacity());
        vector.push_back(1);
        vector.push_back(2);
        EXPECT_EQ(10, vector.capacity());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, max_size)
{
    {
        using max_size_one_vector = score::cpp::inplace_vector<std::int32_t, 1U>;
        static_assert(1U == max_size_one_vector::max_size(), "");
    }
    {
        score::cpp::inplace_vector<std::int32_t, 10> vector{};
        EXPECT_EQ(10, vector.max_size());
        vector.push_back(1);
        vector.push_back(2);
        EXPECT_EQ(10, vector.max_size());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, assign)
{
    {
        score::cpp::inplace_vector<std::int32_t, 1> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.assign(0, 1));
        EXPECT_EQ(0, vector.size());
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.assign(1, 1));
        EXPECT_EQ(1, vector.size());
        EXPECT_EQ(1, vector[0]);
    }
    {
        score::cpp::inplace_vector<std::int32_t, 3> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.assign(2, 1));
        EXPECT_EQ(2, vector.size());
        EXPECT_EQ(1, vector[0]);
        EXPECT_EQ(1, vector[1]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, assign_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 1> vector{};
    EXPECT_EQ(0, vector.size());
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.assign(2, 1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, access_when_precondition_violated)
{
    {
        score::cpp::inplace_vector<std::int32_t, 2> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector[0]);
        vector.push_back(0);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector[0]);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector[1]);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector[2]);
    }
    {
        const score::cpp::inplace_vector<std::int32_t, 2> vector{0};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector[0]);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector[1]);
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector[2]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_default_value_when_size_unchanged)
{
    score::cpp::inplace_vector<std::int32_t, 2> vector{1, 2};
    vector.resize(2);
    EXPECT_EQ(2, vector.size());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_copy_of_value_when_size_unchanged)
{
    score::cpp::inplace_vector<std::int32_t, 2> vector{1, 2};
    vector.resize(2, 23);
    EXPECT_EQ(2, vector.size());
    EXPECT_EQ(1, vector[0]);
    EXPECT_EQ(2, vector[1]);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_default_value_when_shrink)
{
    {
        std::array<std::int32_t, 2> destructor_tracker{};
        score::cpp::inplace_vector<data, 4> vector{data{&destructor_tracker[0]}, data{&destructor_tracker[1]}};
        vector.resize(0);
        EXPECT_EQ(0, vector.size());
        EXPECT_EQ(2, destructor_tracker[0]); // 1 addtional dtor call for temporary after copied into vector
        EXPECT_EQ(2, destructor_tracker[1]); // 1 addtional dtor call for temporary after copied into vector
    }
    {
        std::array<std::int32_t, 2> destructor_tracker{};
        score::cpp::inplace_vector<data, 4> vector{data{&destructor_tracker[0]}, data{&destructor_tracker[1]}};
        vector.resize(1);
        EXPECT_EQ(1, vector.size());
        EXPECT_EQ(1, destructor_tracker[0]); // dtor call for temporary after copied into vector
        EXPECT_EQ(2, destructor_tracker[1]); // 1 addtional dtor call for temporary after copied into vector
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_copy_of_value_when_shrink)
{
    {
        std::array<std::int32_t, 3> destructor_tracker{};
        score::cpp::inplace_vector<data, 4> vector{data{&destructor_tracker[0]}, data{&destructor_tracker[1]}};
        vector.resize(0, data{&destructor_tracker[2]});
        EXPECT_EQ(0, vector.size());
        EXPECT_EQ(2, destructor_tracker[0]); // 1 addtional dtor call for temporary after copied into vector
        EXPECT_EQ(2, destructor_tracker[1]); // 1 addtional dtor call for temporary after copied into vector
    }
    {
        std::array<std::int32_t, 3> destructor_tracker{};
        score::cpp::inplace_vector<data, 4> vector{data{&destructor_tracker[0]}, data{&destructor_tracker[1]}};
        vector.resize(1, data{&destructor_tracker[2]});
        EXPECT_EQ(1, vector.size());
        EXPECT_EQ(1, destructor_tracker[0]); // dtor call for temporary after copied into vector
        EXPECT_EQ(2, destructor_tracker[1]); // 1 addtional dtor call for temporary after copied into vector
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_default_value_when_append)
{
    {
        score::cpp::inplace_vector<std::int32_t, 4> vector{1, 2};
        vector.resize(3);
        EXPECT_EQ(3, vector.size());
        EXPECT_EQ(1, vector[0]);
        EXPECT_EQ(2, vector[1]);
        EXPECT_EQ(0, vector[2]);
    }
    {
        score::cpp::inplace_vector<std::int32_t, 4> vector{1, 2};
        vector.resize(4);
        EXPECT_EQ(4, vector.size());
        EXPECT_EQ(1, vector[0]);
        EXPECT_EQ(2, vector[1]);
        EXPECT_EQ(0, vector[2]);
        EXPECT_EQ(0, vector[3]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_with_copy_of_value_when_append)
{
    {
        score::cpp::inplace_vector<std::int32_t, 4> vector{1, 2};
        vector.resize(3, 23);
        EXPECT_EQ(3, vector.size());
        EXPECT_EQ(1, vector[0]);
        EXPECT_EQ(2, vector[1]);
        EXPECT_EQ(23, vector[2]);
    }
    {
        score::cpp::inplace_vector<std::int32_t, 4> vector{1, 2};
        vector.resize(4, 23);
        EXPECT_EQ(4, vector.size());
        EXPECT_EQ(1, vector[0]);
        EXPECT_EQ(2, vector[1]);
        EXPECT_EQ(23, vector[2]);
        EXPECT_EQ(23, vector[3]);
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, resize_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 4> vector{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.resize(5));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.resize(5, 23));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, iterator_when_using_at_with_precondition_violated)
{
    const score::cpp::inplace_vector<std::int32_t, 2> vector(2, 42);

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(vector, -1));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::at(vector, 0));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(score::cpp::at(vector, 1));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(vector, 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, push_back)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;

    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());

    vector.push_back(1);
    vector.push_back(2);
    vector.push_back(3);
    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector[0], 1);
    EXPECT_EQ(vector[1], 2);
    EXPECT_EQ(vector[2], 3);
    EXPECT_EQ(vector.back(), 3);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, push_back_when_lvalue)
{
    std::int32_t destructor_tracker{0};
    const data lvalue_data{&destructor_tracker};
    score::cpp::inplace_vector<data, 1U> vector{};
    vector.push_back(lvalue_data);

    EXPECT_EQ(1U, vector.size());
    EXPECT_EQ(1, vector[0].copy_constructs());
    EXPECT_EQ(0, vector[0].move_constructs());
    EXPECT_EQ(0, vector[0].copy_assignments());
    EXPECT_EQ(0, vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, push_back_when_rvalue)
{
    std::int32_t destructor_tracker{0};
    score::cpp::inplace_vector<data, 1U> vector{};
    vector.push_back(data{&destructor_tracker});

    EXPECT_EQ(1U, vector.size());
    EXPECT_EQ(0, vector[0].copy_constructs());
    EXPECT_EQ(1, vector[0].move_constructs());
    EXPECT_EQ(0, vector[0].copy_assignments());
    EXPECT_EQ(0, vector[0].move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, push_back_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 1> vector{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.push_back(23));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.push_back(23));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, emplace_back_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 1> vector{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.emplace_back(23));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.emplace_back(23));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, pop_back)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;

    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());

    vector.push_back(1);
    vector.push_back(2);
    vector.push_back(3);
    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector[0], 1);
    EXPECT_EQ(vector[1], 2);
    EXPECT_EQ(vector[2], 3);

    EXPECT_EQ(vector.back(), 3);

    vector.pop_back();
    EXPECT_EQ(vector.back(), 2);

    vector.pop_back();
    EXPECT_EQ(vector.back(), 1);

    vector.pop_back();
    EXPECT_TRUE(vector.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, pop_back_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 1> vector{};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.pop_back());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_element)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;

    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());

    vector.push_back(1);
    vector.push_back(2);
    vector.push_back(3);
    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector[0], 1);
    EXPECT_EQ(vector[1], 2);
    EXPECT_EQ(vector[2], 3);
    EXPECT_EQ(vector.back(), 3);

    auto it = vector.insert(vector.cbegin(), 0);
    EXPECT_EQ(vector.size(), 4U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 2);
    EXPECT_EQ(vector[3], 3);
    EXPECT_EQ(it, vector.begin());

    auto it2 = vector.insert(vector.cend(), 4);
    EXPECT_EQ(vector.size(), 5U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 2);
    EXPECT_EQ(vector[3], 3);
    EXPECT_EQ(vector[4], 4);
    EXPECT_EQ(it2, std::prev(vector.end()));

    auto it3 = vector.insert(vector.cbegin() + 2, 99);
    EXPECT_EQ(vector.size(), 6U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 99);
    EXPECT_EQ(vector[3], 2);
    EXPECT_EQ(vector[4], 3);
    EXPECT_EQ(vector[5], 4);
    EXPECT_EQ(it3, std::next(vector.begin(), 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_element_when_lvalue)
{
    std::int32_t destructor_tracker{0};
    const data lvalue_data{&destructor_tracker};
    score::cpp::inplace_vector<data, 1U> vector{};
    vector.insert(vector.begin(), lvalue_data);

    EXPECT_EQ(1U, vector.size());
    EXPECT_EQ(1, vector.front().copy_constructs());
    EXPECT_EQ(0, vector.front().move_constructs());
    EXPECT_EQ(0, vector.front().copy_assignments());
    EXPECT_EQ(0, vector.front().move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_element_when_rvalue)
{
    std::int32_t destructor_tracker{0};
    score::cpp::inplace_vector<data, 1U> vector{};
    vector.insert(vector.begin(), data{&destructor_tracker});

    EXPECT_EQ(1U, vector.size());
    EXPECT_EQ(0, vector.front().copy_constructs());
    EXPECT_EQ(1, vector.front().move_constructs());
    EXPECT_EQ(0, vector.front().copy_assignments());
    EXPECT_EQ(0, vector.front().move_assignments());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_element_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 3> vector(1, 23);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cbegin(vector) - 1, 23));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.insert(std::cbegin(vector), 23));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.insert(std::cend(vector), 23));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cend(vector) + 1, 23));

    EXPECT_EQ(3U, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_range)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector{10, 20, 30};
    std::vector<std::int32_t> origin{1, 2, 3, 4, 5, 6};
    ASSERT_EQ(vector.size(), 3U);

    auto it = vector.insert(vector.begin(), origin.begin(), origin.begin() + 2);
    score::cpp::inplace_vector<std::int32_t, 999U> expected_vector{1, 2, 10, 20, 30};
    EXPECT_EQ(vector, expected_vector);
    EXPECT_EQ(it, vector.begin());

    auto it2 = vector.insert(vector.end(), origin.begin() + 2, origin.begin() + 4);
    expected_vector = {1, 2, 10, 20, 30, 3, 4};
    EXPECT_EQ(vector, expected_vector);
    EXPECT_EQ(it2, std::prev(vector.end(), 2));

    auto it3 = vector.insert(vector.begin() + 3, origin.begin() + 4, origin.begin() + 6);
    expected_vector = {1, 2, 10, 5, 6, 20, 30, 3, 4};
    EXPECT_EQ(vector, expected_vector);
    EXPECT_EQ(it3, std::next(vector.begin(), 3));

    auto it4 = vector.insert(std::next(vector.begin(), 3), origin.begin(), origin.begin());
    EXPECT_EQ(it4, std::next(vector.begin(), 3));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, insert_range_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 3> vector(1, 23);
    ASSERT_EQ(vector.size(), 1);
    std::vector<std::int32_t> origin{1, 2, 3};

    // inserting at invalid position violates the preconditions
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cbegin(vector) - 1, origin.begin(), origin.end()));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cend(vector) + 1, origin.begin(), origin.end()));
    // inserting an invalid range violates the preconditions
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cend(vector), origin.end(), origin.begin()));
    // inserting more elements than the remaining capacity violates the preconditions
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.insert(std::cend(vector), origin.begin(), origin.end()));
    // inserting elements that still fit inside the inplace_vector shall not violate the preconditions
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.insert(std::cend(vector), origin.begin(), origin.end() - 1));
    // inserting zero elements shall not violate the preconditions
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.insert(std::begin(vector), origin.begin(), origin.begin()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, erase)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;

    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());

    vector.push_back(0);
    vector.push_back(1);
    vector.push_back(99);
    vector.push_back(2);
    vector.push_back(3);
    vector.push_back(4);
    EXPECT_EQ(vector.size(), 6U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 99);
    EXPECT_EQ(vector[3], 2);
    EXPECT_EQ(vector[4], 3);
    EXPECT_EQ(vector[5], 4);

    auto it = vector.erase(vector.cbegin() + 2);
    EXPECT_EQ(vector.size(), 5U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 2);
    EXPECT_EQ(vector[3], 3);
    EXPECT_EQ(vector[4], 4);
    EXPECT_EQ(it, vector.begin() + 2);

    auto it2 = vector.erase(vector.cend() - 1);
    EXPECT_EQ(vector.size(), 4U);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 2);
    EXPECT_EQ(vector[3], 3);
    EXPECT_EQ(it2, vector.end());

    auto it3 = vector.erase(vector.cbegin());
    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector[0], 1);
    EXPECT_EQ(vector[1], 2);
    EXPECT_EQ(vector[2], 3);
    EXPECT_EQ(it3, vector.begin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, erase_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 2> vector(2, 23);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.erase(std::cbegin(vector) - 1));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.erase(std::cbegin(vector)));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.erase(std::cend(vector) - 1));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.erase(std::cend(vector)));

    EXPECT_TRUE(vector.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, erase_range)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;

    EXPECT_EQ(vector.size(), 0U);
    EXPECT_TRUE(vector.empty());

    vector.push_back(0);
    vector.push_back(1);
    vector.push_back(99);
    vector.push_back(2);
    vector.push_back(3);
    vector.push_back(4);
    EXPECT_EQ(vector.size(), 6U);
    EXPECT_EQ(vector.front(), vector[0]);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 99);
    EXPECT_EQ(vector[3], 2);
    EXPECT_EQ(vector[4], 3);
    EXPECT_EQ(vector[5], 4);

    auto it = vector.erase(vector.cbegin(), vector.cbegin());
    EXPECT_EQ(vector.size(), 6U);
    EXPECT_EQ(vector.front(), vector[0]);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 99);
    EXPECT_EQ(vector[3], 2);
    EXPECT_EQ(vector[4], 3);
    EXPECT_EQ(vector[5], 4);
    EXPECT_EQ(it, vector.begin());

    auto it2 = vector.erase(vector.cend(), vector.cend());
    EXPECT_EQ(vector.size(), 6U);
    EXPECT_EQ(vector.front(), vector[0]);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 1);
    EXPECT_EQ(vector[2], 99);
    EXPECT_EQ(vector[3], 2);
    EXPECT_EQ(vector[4], 3);
    EXPECT_EQ(vector[5], 4);
    EXPECT_EQ(it2, vector.end());

    auto it3 = vector.erase(vector.cbegin() + 1, vector.cend() - 2);
    EXPECT_EQ(vector.size(), 3U);
    EXPECT_EQ(vector.front(), vector[0]);
    EXPECT_EQ(vector[0], 0);
    EXPECT_EQ(vector[1], 3);
    EXPECT_EQ(vector[2], 4);
    EXPECT_EQ(it3, vector.begin() + 1);

    auto it4 = vector.erase(vector.cbegin(), vector.cend());
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_EQ(it4, vector.end());

    auto it5 = vector.erase(vector.cbegin(), vector.cend());
    EXPECT_EQ(vector.size(), 0U);
    EXPECT_EQ(it5, vector.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, erase_range_when_precondition_violated)
{
    score::cpp::inplace_vector<std::int32_t, 2> vector(2, 23);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.erase(std::cend(vector), std::cbegin(vector)));

    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.erase(std::cbegin(vector) - 1, std::cbegin(vector)));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.erase(std::cbegin(vector), std::cbegin(vector) + 1));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(vector.erase(std::cend(vector) - 1, std::cend(vector)));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.erase(std::cend(vector), std::cend(vector) + 1));

    EXPECT_TRUE(vector.empty());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, reverse)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector;
    vector.push_back(0);
    vector.push_back(1);
    vector.push_back(2);
    vector.push_back(3);
    vector.push_back(4);

    auto rit = std::make_reverse_iterator(vector.end());
    const auto ret = std::make_reverse_iterator(vector.begin());

    std::int32_t i{4};
    for (; rit != ret; ++rit)
    {
        EXPECT_EQ(i, *rit);
        --i;
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, back)
{
    {
        score::cpp::inplace_vector<std::int32_t, 1U> vector{23};
        EXPECT_EQ(23, vector.back());
    }
    {
        const score::cpp::inplace_vector<std::int32_t, 1U> vector{23};
        EXPECT_EQ(23, vector.back());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, back_when_precondition_violated)
{
    {
        score::cpp::inplace_vector<std::int32_t, 1> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.back());
    }
    {
        const score::cpp::inplace_vector<std::int32_t, 1> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.back());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, front)
{
    {
        score::cpp::inplace_vector<std::int32_t, 1U> vector{23};
        EXPECT_EQ(23, vector.front());
    }
    {
        const score::cpp::inplace_vector<std::int32_t, 1U> vector{23};
        EXPECT_EQ(23, vector.front());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, front_when_precondition_violated)
{
    {
        score::cpp::inplace_vector<std::int32_t, 1> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.front());
    }
    {
        const score::cpp::inplace_vector<std::int32_t, 1> vector{};
        SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(vector.front());
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, iterator)
{
    score::cpp::inplace_vector<std::int32_t, 20> container(20, 23);

    EXPECT_EQ(20, container.end() - container.begin());

    std::int32_t i = 0;
    std::generate(container.begin(), container.end(), [&i] { return i++; });

    auto begin = container.begin();
    EXPECT_EQ(12, begin[12]);

    EXPECT_EQ(1, *(begin + 1));
    EXPECT_EQ(15, *(begin + 15));
    EXPECT_EQ(1, *(begin += 1));
    EXPECT_EQ(16, *(begin += 15));
    EXPECT_EQ(17, *(++begin));
    EXPECT_EQ(17, *(begin++));
    EXPECT_EQ(18, *(begin));
    EXPECT_EQ(18, *(begin.operator->()));

    auto end = container.end();
    EXPECT_EQ(19, *(end - 1));
    EXPECT_EQ(5, *(end - 15));
    EXPECT_EQ(19, *(end -= 1));
    EXPECT_EQ(4, *(end -= 15));
    EXPECT_EQ(3, *(--end));
    EXPECT_EQ(3, *(end--));
    EXPECT_EQ(2, *(end));
    EXPECT_EQ(2, *(end.operator->()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, difference_type_matches_with_iterator)
{
    using type = score::cpp::inplace_vector<std::int32_t, 20>;

    static_assert(std::is_same_v<type::difference_type, std::iterator_traits<type::iterator>::difference_type>);
    static_assert(std::is_same_v<type::difference_type, std::iterator_traits<type::const_iterator>::difference_type>);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, iterator_conversion_from_non_const_to_const_iterator)
{
    score::cpp::inplace_vector<std::int32_t, 20> container(20, 23);
    score::cpp::inplace_vector<std::int32_t, 20>::const_iterator it{container.begin()};

    static_assert(std::is_same<const std::int32_t&, decltype(*it)>::value, "failure");
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, default_constructed_iterator_cannot_be_dereferenced)
{
    score::cpp::inplace_vector<std::int32_t, 1U>::iterator unit;
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(unit.operator->());
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(unit.operator*());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_equals)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    vector_1.push_back(1);
    vector_1.push_back(2);

    score::cpp::inplace_vector<std::int32_t, 3U> vector_2 = vector_1;

    EXPECT_TRUE(vector_1 == vector_2);
    EXPECT_FALSE(vector_1 != vector_2);
    EXPECT_FALSE(vector_1 > vector_2);
    EXPECT_TRUE(vector_1 >= vector_2);
    EXPECT_FALSE(vector_1 < vector_2);
    EXPECT_TRUE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_not_equals)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    vector_1.push_back(1);
    vector_1.push_back(2);

    score::cpp::inplace_vector<std::int32_t, 3U> vector_2;
    vector_2.push_back(3);
    vector_2.push_back(4);

    EXPECT_FALSE(vector_1 == vector_2);
    EXPECT_TRUE(vector_1 != vector_2);
    EXPECT_FALSE(vector_1 > vector_2);
    EXPECT_FALSE(vector_1 >= vector_2);
    EXPECT_TRUE(vector_1 < vector_2);
    EXPECT_TRUE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_one_is_empty)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    vector_1.push_back(1);
    vector_1.push_back(2);

    score::cpp::inplace_vector<std::int32_t, 3U> vector_2;

    EXPECT_FALSE(vector_1 == vector_2);
    EXPECT_TRUE(vector_1 != vector_2);
    EXPECT_TRUE(vector_1 > vector_2);
    EXPECT_TRUE(vector_1 >= vector_2);
    EXPECT_FALSE(vector_1 < vector_2);
    EXPECT_FALSE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_both_are_empty)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    score::cpp::inplace_vector<std::int32_t, 3U> vector_2;

    EXPECT_TRUE(vector_1 == vector_2);
    EXPECT_FALSE(vector_1 != vector_2);
    EXPECT_FALSE(vector_1 > vector_2);
    EXPECT_TRUE(vector_1 >= vector_2);
    EXPECT_FALSE(vector_1 < vector_2);
    EXPECT_TRUE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_one_is_a_prefix_of_other)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    vector_1.push_back(1);
    vector_1.push_back(2);

    score::cpp::inplace_vector<std::int32_t, 3U> vector_2;
    vector_2.push_back(1);

    EXPECT_FALSE(vector_1 == vector_2);
    EXPECT_TRUE(vector_1 != vector_2);
    EXPECT_TRUE(vector_1 > vector_2);
    EXPECT_TRUE(vector_1 >= vector_2);
    EXPECT_FALSE(vector_1 < vector_2);
    EXPECT_FALSE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, comparison_operators_when_only_first_elements_are_equal)
{
    score::cpp::inplace_vector<std::int32_t, 3U> vector_1;
    vector_1.push_back(1);
    vector_1.push_back(4);
    vector_1.push_back(2);

    score::cpp::inplace_vector<std::int32_t, 3U> vector_2;
    vector_2.push_back(1);
    vector_2.push_back(4);
    vector_2.push_back(7);

    EXPECT_FALSE(vector_1 == vector_2);
    EXPECT_TRUE(vector_1 != vector_2);
    EXPECT_FALSE(vector_1 > vector_2);
    EXPECT_FALSE(vector_1 >= vector_2);
    EXPECT_TRUE(vector_1 < vector_2);
    EXPECT_TRUE(vector_1 <= vector_2);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, initializer_list)
{
    const score::cpp::inplace_vector<std::int32_t, 3> vector{3, 4, 5};
    EXPECT_EQ(3, vector[0]);
    EXPECT_EQ(4, vector[1]);
    EXPECT_EQ(5, vector[2]);
    EXPECT_EQ(3, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, initializer_list_while_element_throws)
{
    std::array<copy_bomb, 3U> source{};
    score::cpp::at(source, 1).shall_throw = true;
    score::cpp::inplace_vector<copy_bomb, 3U> vector{source.begin(), source.end()};

    EXPECT_EQ(0U, vector.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, initializer_list_when_precondition_violated)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED((score::cpp::inplace_vector<std::int32_t, 2>{3, 4, 5}));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, rbegin_and_rend)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector(4, 23);
    std::iota(vector.rbegin(), vector.rend(), 0);

    EXPECT_EQ(vector[0], 3);
    EXPECT_EQ(vector[1], 2);
    EXPECT_EQ(vector[2], 1);
    EXPECT_EQ(vector[3], 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, crbegin_and_crend)
{
    score::cpp::inplace_vector<std::int32_t, 999U> vector(5, 23);
    std::iota(vector.begin(), vector.end(), 0);

    static_assert(std::is_same<decltype(*vector.crbegin()), const std::int32_t&>::value,
                  "crbegin should return a constant iterator");
    auto rit = vector.crbegin();
    const auto ret = vector.crend();

    std::int32_t i{4};
    for (; rit != ret; ++rit)
    {
        EXPECT_EQ(i, *rit);
        --i;
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, construction_with_iterators)
{
    std::vector<std::int32_t> origin{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    score::cpp::inplace_vector<std::int32_t, 10U> vector(std::cbegin(origin), std::cend(origin));
    EXPECT_TRUE(std::equal(std::cbegin(origin), std::cend(origin), std::cbegin(vector), std::cend(vector)));
    EXPECT_EQ(vector.size(), origin.size());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, construction_with_iterators_precondition_violation)
{
    const std::vector<std::int32_t> origin{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    using inplace_vector = score::cpp::inplace_vector<int, 2>;
    const auto begin = std::begin(origin);
    const auto end = std::end(origin);
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(inplace_vector(begin, end));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_NOT_VIOLATED(inplace_vector(begin, std::next(begin, 2)));
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(inplace_vector(end, begin));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, is_inplace_vector)
{
    static_assert(score::cpp::is_inplace_vector<score::cpp::inplace_vector<std::int32_t, 2>>::value, "failure");
    static_assert(!score::cpp::is_inplace_vector<std::vector<std::int32_t>>::value, "failure");
    EXPECT_TRUE((score::cpp::is_inplace_vector<score::cpp::inplace_vector<std::int32_t, 2>>::value));
    EXPECT_FALSE((score::cpp::is_inplace_vector<std::vector<std::int32_t>>::value));
}

struct without_noexcept
{
    without_noexcept() = default;
    without_noexcept(const without_noexcept&) = default;
    without_noexcept& operator=(const without_noexcept&) = default;
    without_noexcept(without_noexcept&&) {}
    without_noexcept& operator=(without_noexcept&&) { return *this; }
    ~without_noexcept() = default;
};

struct noexcept_move_ctor
{
    noexcept_move_ctor() = default;
    noexcept_move_ctor(const noexcept_move_ctor&) = default;
    noexcept_move_ctor& operator=(const noexcept_move_ctor&) = default;
    noexcept_move_ctor(noexcept_move_ctor&&) noexcept {}
    noexcept_move_ctor& operator=(noexcept_move_ctor&&) { return *this; }
    ~noexcept_move_ctor() = default;
};

struct noexcept_move_assign
{
    noexcept_move_assign() = default;
    noexcept_move_assign(const noexcept_move_assign&) = default;
    noexcept_move_assign& operator=(const noexcept_move_assign&) = default;
    noexcept_move_assign(noexcept_move_assign&&) {}
    noexcept_move_assign& operator=(noexcept_move_assign&&) noexcept { return *this; }
    ~noexcept_move_assign() = default;
};

struct noexcept_move_assign_and_ctor
{
    noexcept_move_assign_and_ctor() = default;
    noexcept_move_assign_and_ctor(const noexcept_move_assign_and_ctor&) = default;
    noexcept_move_assign_and_ctor& operator=(const noexcept_move_assign_and_ctor&) = default;
    noexcept_move_assign_and_ctor(noexcept_move_assign_and_ctor&&) noexcept {}
    noexcept_move_assign_and_ctor& operator=(noexcept_move_assign_and_ctor&&) noexcept { return *this; }
    ~noexcept_move_assign_and_ctor() = default;
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17893146
TEST(inplace_vector_test, move_assign_noexcept)
{
    static_assert(std::is_nothrow_move_assignable<score::cpp::inplace_vector<noexcept_move_assign_and_ctor, 2>>::value,
                  "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::inplace_vector<without_noexcept, 2>>::value, "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::inplace_vector<noexcept_move_ctor, 2>>::value, "failed");
    static_assert(!std::is_nothrow_move_assignable<score::cpp::inplace_vector<noexcept_move_assign, 2>>::value, "failed");
}

} // namespace
