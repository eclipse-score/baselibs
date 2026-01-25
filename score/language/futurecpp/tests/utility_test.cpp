/********************************************************************************
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2017 Contributors to the Eclipse Foundation
///

#include <score/utility.hpp>
#include <score/utility.hpp> // test include guard

#include <score/assert_support.hpp>

#include <array>
#include <cstdint>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct foo
{
    std::int32_t bar() const { return 23; }
    std::int32_t bar() { return 42; }
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299345
TEST(select_const_function, as_const)
{
    foo test;
    EXPECT_EQ(23, score::cpp::as_const(test).bar());
    EXPECT_EQ(42, test.bar());
}

///
/// \test Test if the check for valid changes of data type representation is correct.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299406
///
TEST(utility_test, is_convertible)
{
    EXPECT_TRUE(score::cpp::is_convertible<bool>(0));
    EXPECT_TRUE(score::cpp::is_convertible<bool>(1));
    EXPECT_TRUE(score::cpp::is_convertible<std::string>(std::string{""}));
    EXPECT_TRUE(score::cpp::is_convertible<std::string>(std::string{"42"}));

    EXPECT_FALSE(score::cpp::is_convertible<bool>(2));
    EXPECT_FALSE(score::cpp::is_convertible<bool>(-2));

    EXPECT_TRUE(score::cpp::is_convertible<int>(42));
    EXPECT_TRUE(score::cpp::is_convertible<int>(-42));
    EXPECT_TRUE(score::cpp::is_convertible<int>(42.0));
    EXPECT_TRUE(score::cpp::is_convertible<int>(42.0F));
    EXPECT_TRUE(score::cpp::is_convertible<int>(std::numeric_limits<int>::max()));
    EXPECT_TRUE(score::cpp::is_convertible<int>(std::numeric_limits<int>::min()));
    EXPECT_TRUE(score::cpp::is_convertible<int>(std::numeric_limits<uint>::min()));

    EXPECT_FALSE(score::cpp::is_convertible<int>(std::numeric_limits<uint>::max()));
    EXPECT_FALSE(score::cpp::is_convertible<int>(42.42));
    EXPECT_FALSE(score::cpp::is_convertible<int>(42.42F));

    EXPECT_TRUE(score::cpp::is_convertible<std::int8_t>(-128));
    EXPECT_TRUE(score::cpp::is_convertible<std::int8_t>(123));
    EXPECT_TRUE(score::cpp::is_convertible<std::int8_t>(std::numeric_limits<std::int8_t>::min()));
    EXPECT_TRUE(score::cpp::is_convertible<std::int16_t>(std::numeric_limits<std::uint16_t>::min()));
    EXPECT_TRUE(score::cpp::is_convertible<std::uint16_t>(std::numeric_limits<std::uint16_t>::max()));
    EXPECT_TRUE(score::cpp::is_convertible<std::uint16_t>(std::numeric_limits<std::uint8_t>::max()));
    EXPECT_TRUE(score::cpp::is_convertible<std::uint32_t>(std::numeric_limits<std::uint16_t>::max()));
    EXPECT_TRUE(score::cpp::is_convertible<std::uint32_t>(std::numeric_limits<std::uint16_t>::min()));

    EXPECT_FALSE(score::cpp::is_convertible<std::int16_t>(std::numeric_limits<std::uint16_t>::max()));
    EXPECT_FALSE(score::cpp::is_convertible<std::uint16_t>(std::numeric_limits<std::int16_t>::min()));
    EXPECT_FALSE(score::cpp::is_convertible<std::uint16_t>(-32000));
    EXPECT_FALSE(score::cpp::is_convertible<std::uint8_t>(-128));
    EXPECT_FALSE(score::cpp::is_convertible<std::uint8_t>(300));
    EXPECT_FALSE(score::cpp::is_convertible<std::int8_t>(200));
}

///
/// \test Narrow cast shall return value if conversion is valid.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299489
///
TEST(utility_test, narrow_cast)
{ //
    EXPECT_EQ(42, score::cpp::narrow_cast<uint8_t>(42));
}

///
/// \test Narrow cast shall report a precondition violation if conversion is invalid.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299489
///
TEST(utility_test, narrow_cast_when_precondition_violated)
{
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::narrow_cast<uint8_t>(4242));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299370
TEST(utility_test, to_underlying_enum_class_int8)
{
    enum class test_enum : std::int8_t
    {
        e
    };
    const bool is_same{std::is_same<std::int8_t, decltype(score::cpp::to_underlying(test_enum::e))>::value};
    EXPECT_TRUE(is_same);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299370
TEST(utility_test, to_underlying_enum_struct_int32)
{
    enum struct test_enum : std::int32_t
    {
        e
    };
    const bool is_same{std::is_same<std::int32_t, decltype(score::cpp::to_underlying(test_enum::e))>::value};
    EXPECT_TRUE(is_same);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299370
TEST(utility_test, to_underlying_enum_uint64)
{
    enum test_enum : std::uint64_t
    {
        e
    };
    const bool is_same{std::is_same<std::uint64_t, decltype(score::cpp::to_underlying(e))>::value};
    EXPECT_TRUE(is_same);
}

/// @test is_maplike_container trait is only true on subclasses of
/// - Associative containers and
/// - Unordered associative containers,
/// namely *map
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(is_maplike_container, check_trait_equivalence_classes)
{
    {
        static_assert(score::cpp::detail::is_maplike_container<std::map<int, double>>::value,
                      "wrong classification in associative containers");
        static_assert(score::cpp::detail::is_maplike_container<std::multimap<int, double>>::value,
                      "wrong classification in associative containers");
        static_assert(score::cpp::detail::is_maplike_container<std::unordered_map<int, double>>::value,
                      "wrong classification in unordered associative containers");
        static_assert(score::cpp::detail::is_maplike_container<std::unordered_multimap<int, double>>::value,
                      "wrong classification in unordered associative containers");
    }
    {
        static_assert(!score::cpp::detail::is_maplike_container<std::set<int>>::value,
                      "wrong classification in associative containers");
        static_assert(!score::cpp::detail::is_maplike_container<std::multiset<int>>::value,
                      "wrong classification in associative containers");
        static_assert(!score::cpp::detail::is_maplike_container<std::unordered_set<int>>::value,
                      "wrong classification in unordered associative containers");
        static_assert(!score::cpp::detail::is_maplike_container<std::unordered_multiset<int>>::value,
                      "wrong classification in unordered associative containers");

        static_assert(!score::cpp::detail::is_maplike_container<std::array<int, 2>>::value,
                      "wrong classification in sequence containers");
        static_assert(!score::cpp::detail::is_maplike_container<std::vector<int>>::value,
                      "wrong classification in sequence containers");
        static_assert(!score::cpp::detail::is_maplike_container<std::list<int>>::value,
                      "wrong classification in sequence containers");

        static_assert(!score::cpp::detail::is_maplike_container<std::stack<int>>::value,
                      "wrong classification in container adapters");
        static_assert(!score::cpp::detail::is_maplike_container<std::queue<int>>::value,
                      "wrong classification in container adapters");

        static_assert(!score::cpp::detail::is_maplike_container<std::string>::value,
                      "wrong classification in pseudo container");
    }
}

/// @test Mutable access to C-style array shall return a non-const reference.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_c_array)
{
    int array[23];
    for (int i = 0; i < 23; ++i)
    {
        score::cpp::at(array, i) = i;
    }

    for (int i = 0; i < 23; ++i)
    {
        EXPECT_EQ(i, score::cpp::at(array, i));
    }
}

/// @test Mutable access to std::array shall return a non-const reference.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_stl_array)
{
    std::array<int, 23U> array;
    for (int i = 0; i < 23; ++i)
    {
        score::cpp::at(array, i) = i;
    }

    for (int i = 0; i < 23; ++i)
    {
        EXPECT_EQ(i, score::cpp::at(array, i));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_custom_minimal_flat_map)
{
    struct test_struct : std::vector<std::pair<std::ptrdiff_t, std::int32_t>>
    {
        using key_type = std::ptrdiff_t;
        using mapped_type = std::int32_t;
        using base = std::vector<std::pair<std::ptrdiff_t, std::int32_t>>;
        using base::base;

        iterator find(const key_type& k)
        {
            return std::find_if(begin(), end(), [&k](const auto& entry) { return entry.first == k; });
        }
    };

    test_struct s{{3, 0}, {2, 1}, {1, 2}, {0, 3}};
    EXPECT_EQ(score::cpp::at(s, 3), 0);
}

/// @test Imutable access to a const std::unordered_map shall be possible.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_stl_const_unordered_map)
{
    std::unordered_map<int, int> map{{1, 1}};

    EXPECT_EQ(1, score::cpp::at(score::cpp::as_const(map), 1));
}

/// @test Mutable access to std::unordered_map shall return a non-const reference.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_stl_unordered_map)
{
    std::unordered_map<int, int> map{{1, 1}};

    score::cpp::at(map, 1) = 2;

    EXPECT_EQ(2, score::cpp::at(map, 1));
}

/// @test Imutable access to a const std::unordered_map shall be possible.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_out_of_bounds_unordered_map)
{
    std::unordered_map<int, int> map{{1, 1}};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(map, 2));
}

/// @test Out-of-bounds access shall trigger the bounds check in debug mode.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_out_of_bounds_c_array)
{
    const int array[23] = {};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(array, 42));
}

/// @test Out-of-bounds access shall trigger the bounds check in debug mode.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, at_out_of_bounds_stl_array)
{
    const std::array<std::size_t, 23> array = {};
    SCORE_LANGUAGE_FUTURECPP_EXPECT_CONTRACT_VIOLATED(score::cpp::at(array, 42));
}

/// @test At() shall return const reference for an immutable container and non-const reference for a mutable container.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, constness_c_array)
{
    {
        const int array[1] = {};
        using ReturnType = decltype(score::cpp::at(array, 0));
        static_assert(std::is_same<const int&, ReturnType>::value, "Wrong return type.");
    }
    {
        int array[1] = {};
        using ReturnType = decltype(score::cpp::at(array, 0));
        static_assert(std::is_same<int&, ReturnType>::value, "Wrong return type.");
    }
}

/// @test At() shall return const reference for an immutable container and non-const reference for a mutable container.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, constness_stl_array)
{
    {
        const std::array<int, 1> array = {};
        using ReturnType = decltype(score::cpp::at(array, 0));
        static_assert(std::is_same<const int&, ReturnType>::value, "Wrong return type.");
    }
    {
        std::array<int, 1> array = {};
        using ReturnType = decltype(score::cpp::at(array, 0));
        static_assert(std::is_same<int&, ReturnType>::value, "Wrong return type.");
    }
}

/// @test At() shall return const reference for an immutable container and non-const reference for a mutable container.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17299475
TEST(at, constness_unordered_map)
{
    {
        const std::map<int, double> map = {};
        using ReturnType = decltype(score::cpp::at(map, 0));
        static_assert(std::is_same<const double&, ReturnType>::value, "Wrong return type.");
    }
    {
        std::map<int, double> map = {};
        using ReturnType = decltype(score::cpp::at(map, 0));
        static_assert(std::is_same<double&, ReturnType>::value, "Wrong return type.");
    }
}

///@test Assigning a value to ignore shall not produce compiler warnings of unused variable.
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17300055
TEST(ignore, swallow)
{
    constexpr int v{23};
    score::cpp::ignore = v;                         // test lvalue
    score::cpp::ignore = foo{}.bar();               // test rvalue
    score::cpp::ignore = std::make_unique<int>(23); // test move-only object
}

} // namespace
