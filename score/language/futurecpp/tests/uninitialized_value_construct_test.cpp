/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

#include <score/private/memory/uninitialized_value_construct.hpp>
#include <score/private/memory/uninitialized_value_construct.hpp> // check include guard

#include <score/utility.hpp>

#include <array>
#include <cstdint>
#include <exception>
#include <iterator>
#include <type_traits>

#include <gtest/gtest.h>

namespace
{

// NOTRACING
TEST(UninitializedValueConstructTest, WhenNoException)
{
    struct test_type
    {
        test_type() : value{23} {}
        test_type(const test_type&) = delete;
        test_type& operator=(const test_type&) = delete;
        test_type(test_type&& other) = delete;
        test_type& operator=(test_type&&) = delete;
        ~test_type() noexcept { value = 0; }
        int value;
    };

    std::aligned_storage_t<sizeof(test_type), alignof(test_type)> target[2];

    auto* first = reinterpret_cast<test_type*>(&target[0]);
    const test_type* result{};
    EXPECT_NO_THROW(result = score::cpp::uninitialized_value_construct_n(first, 1));

    EXPECT_EQ(23, first->value);
    EXPECT_EQ(result, reinterpret_cast<test_type*>(&target[1]));
}

struct test_type
{
    struct observer
    {
        std::int32_t default_ctor;
        std::int32_t dtor;
        bool shall_throw;
    };

    test_type()
    {
        id = count++;

        ++observe[id].default_ctor;
        if (observe[id].shall_throw)
        {
            throw std::exception{};
        }
    }
    test_type(const test_type&) = delete;
    test_type& operator=(const test_type&) = delete;
    test_type(test_type&& other) = delete;
    test_type& operator=(test_type&&) = delete;
    ~test_type() noexcept { ++observe[id].dtor; }

    std::uint32_t id;
    static std::uint32_t count;
    static std::array<observer, 2> observe;
};

std::uint32_t test_type::count{};
std::array<test_type::observer, 2> test_type::observe{};

// NOTRACING
TEST(UninitializedValueConstructTest, WhenException)
{
    test_type::count = 0U;
    test_type::observe = {};
    test_type::observe[0U].shall_throw = false;
    test_type::observe[1U].shall_throw = true;

    std::aligned_storage_t<sizeof(test_type), alignof(test_type)> target[2];

    auto* const first = reinterpret_cast<test_type*>(&target[0]);
    EXPECT_THROW(score::cpp::ignore = score::cpp::uninitialized_value_construct_n(first, 2), std::exception);

    EXPECT_EQ(1, test_type::observe[0U].default_ctor);
    EXPECT_EQ(1, test_type::observe[1U].default_ctor);
    EXPECT_EQ(1, test_type::observe[0U].dtor);
    EXPECT_EQ(0, test_type::observe[1U].dtor);
}

} // namespace
