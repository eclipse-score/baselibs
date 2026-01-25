/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/private/memory/uninitialized_move.hpp>
#include <score/private/memory/uninitialized_move.hpp> // check include guard

#include <score/utility.hpp>

#include <array>
#include <exception>
#include <iterator>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct trivial_test_type
{
    int value;
};

struct non_trivial_test_type
{
    int value{42};
};

struct non_copyable_test_type
{
    non_copyable_test_type() = default;
    non_copyable_test_type(const non_copyable_test_type&) = delete;
    non_copyable_test_type& operator=(const non_copyable_test_type&) = delete;
    non_copyable_test_type(non_copyable_test_type&& other) = default;
    non_copyable_test_type& operator=(non_copyable_test_type&&) = default;
    ~non_copyable_test_type() noexcept = default;
    int value;
};

static_assert(!std::is_copy_constructible<non_copyable_test_type>::value &&
                  !std::is_copy_assignable<non_copyable_test_type>::value,
              "must be non-copyable for this test");
static_assert(!std::is_trivial<non_trivial_test_type>::value, "must be non-trivial for this test");
static_assert(std::is_trivial<trivial_test_type>::value, "must be trivial for this test");
using test_types = ::testing::Types<trivial_test_type, non_trivial_test_type, non_copyable_test_type>;

template <typename T>
class uninitialized_move_test_fixture : public ::testing::Test
{
};

TYPED_TEST_SUITE(uninitialized_move_test_fixture, test_types, /*unused*/);

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42721376
TYPED_TEST(uninitialized_move_test_fixture, when_no_exception)
{
    std::vector<TypeParam> source(1);
    source[0].value = 23;

    std::aligned_storage_t<sizeof(TypeParam), alignof(TypeParam)> target[2];

    auto* first = reinterpret_cast<TypeParam*>(&target[0]);
    const TypeParam* result{};
    EXPECT_NO_THROW(result = score::cpp::uninitialized_move(std::begin(source), std::end(source), first));

    EXPECT_EQ(23, first->value);
    EXPECT_EQ(result, reinterpret_cast<TypeParam*>(&target[1]));
}

template <typename T>
class uninitialized_move_n_test_fixture : public ::testing::Test
{
};

TYPED_TEST_SUITE(uninitialized_move_n_test_fixture, test_types, /*unused*/);

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42721376
TYPED_TEST(uninitialized_move_n_test_fixture, when_no_exception)
{
    std::vector<TypeParam> source(1);
    source[0].value = 23;

    std::aligned_storage_t<sizeof(TypeParam), alignof(TypeParam)> target[2];

    auto* first = reinterpret_cast<TypeParam*>(&target[0]);
    std::pair<typename std::vector<TypeParam>::iterator, TypeParam*> result{};
    EXPECT_NO_THROW(result = score::cpp::uninitialized_move_n(std::begin(source), source.size(), first));

    EXPECT_EQ(23, first->value);
    EXPECT_EQ(result.first, source.end());
    EXPECT_EQ(result.second, reinterpret_cast<TypeParam*>(&target[1]));
}

struct observer
{
    int move;
    int dtor;
};

struct throwing_test_type
{
    throwing_test_type(observer& arg) : observe{arg}, shall_throw{false} {}
    throwing_test_type(const throwing_test_type&) = delete;
    throwing_test_type& operator=(const throwing_test_type&) = delete;
    throwing_test_type(throwing_test_type&& other) : observe{other.observe}, shall_throw{other.shall_throw}
    {
        ++observe.move;
        if (shall_throw)
        {
            throw std::exception{};
        }
    }
    throwing_test_type& operator=(throwing_test_type&&) = delete;
    ~throwing_test_type() noexcept { ++observe.dtor; }
    observer& observe;
    bool shall_throw;
};

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42721376
TEST(uninitialized_move_test, when_exception)
{
    std::array<observer, 2> test_type_observer{};

    std::vector<throwing_test_type> source{};
    source.reserve(2);
    source.emplace_back(test_type_observer[0]);
    source.emplace_back(test_type_observer[1]);

    source[1].shall_throw = true;

    std::aligned_storage_t<sizeof(throwing_test_type), alignof(throwing_test_type)> target[2];

    auto* const first = reinterpret_cast<throwing_test_type*>(&target[0]);
    EXPECT_THROW(score::cpp::ignore = score::cpp::uninitialized_move(std::begin(source), std::end(source), first), std::exception);

    EXPECT_EQ(1, test_type_observer[0].move);
    EXPECT_EQ(1, test_type_observer[1].move);
    EXPECT_EQ(1, test_type_observer[0].dtor);
    EXPECT_EQ(0, test_type_observer[1].dtor);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42721376
TEST(uninitialized_move_n_test, when_exception)
{
    std::array<observer, 2> test_type_observer{};

    std::vector<throwing_test_type> source{};
    source.reserve(2);
    source.emplace_back(test_type_observer[0]);
    source.emplace_back(test_type_observer[1]);

    source[1].shall_throw = true;

    std::aligned_storage_t<sizeof(throwing_test_type), alignof(throwing_test_type)> target[2];

    auto* const first = reinterpret_cast<throwing_test_type*>(&target[0]);
    EXPECT_THROW(score::cpp::ignore = score::cpp::uninitialized_move_n(std::begin(source), source.size(), first), std::exception);

    EXPECT_EQ(1, test_type_observer[0].move);
    EXPECT_EQ(1, test_type_observer[1].move);
    EXPECT_EQ(1, test_type_observer[0].dtor);
    EXPECT_EQ(0, test_type_observer[1].dtor);
}

} // namespace
