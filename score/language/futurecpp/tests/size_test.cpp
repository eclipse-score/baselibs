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

#include <score/size.hpp>
#include <score/size.hpp> // test include guard

#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

namespace
{

struct foo
{
    std::uint32_t bar[100];
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_c_array)
{
    std::int32_t a[5] = {0, 1, 2, 3, 4};
    EXPECT_EQ(5U, score::cpp::size(a));
    EXPECT_EQ(5, score::cpp::ssize(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_const_c_array)
{
    std::int32_t const a[5] = {0, 1, 2, 3, 4};
    EXPECT_EQ(5U, score::cpp::size(a));
    EXPECT_EQ(5, score::cpp::ssize(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_struct_array)
{
    foo tmp = {};
    EXPECT_EQ(100U, score::cpp::size(foo{}.bar));
    EXPECT_EQ(100U, score::cpp::size(tmp.bar));
    EXPECT_EQ(100, score::cpp::ssize(foo{}.bar));
    EXPECT_EQ(100, score::cpp::ssize(tmp.bar));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_const_struct_array)
{
    foo const tmp = {};
    EXPECT_EQ(100U, score::cpp::size(foo{}.bar));
    EXPECT_EQ(100U, score::cpp::size(tmp.bar));
    EXPECT_EQ(100, score::cpp::ssize(foo{}.bar));
    EXPECT_EQ(100, score::cpp::ssize(tmp.bar));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_array)
{
    std::array<int, 5> tmp{};
    EXPECT_EQ(5U, score::cpp::size(tmp));
    EXPECT_EQ(5, score::cpp::ssize(tmp));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_const_array)
{
    std::array<int, 5> const tmp = {0, 0, 0, 0, 0};
    EXPECT_EQ(5U, score::cpp::size(tmp));
    EXPECT_EQ(5, score::cpp::ssize(tmp));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_vector)
{
    std::vector<int> tmp(5);
    EXPECT_EQ(5U, score::cpp::size(tmp));
    EXPECT_EQ(5, score::cpp::ssize(tmp));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, size_on_const_vector)
{
    std::vector<int> const tmp(5);
    EXPECT_EQ(5U, score::cpp::size(tmp));
    EXPECT_EQ(5, score::cpp::ssize(tmp));
}

struct no_size_member_function
{
    std::vector<int> store;
    auto begin() const { return store.cbegin(); }
    auto end() const { return store.cend(); }
};
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, ssize_on_object_without_size_member_function)
{
    no_size_member_function tmp{std::vector<int>(5)};
    EXPECT_EQ(5, score::cpp::ssize(tmp));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, type_literal_size_t)
{
    using namespace score::cpp::literals;
    EXPECT_TRUE((std::is_same<std::size_t, decltype(42_UZ)>::value));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#9338059
TEST(size, type_literal_ptrdiff_t)
{
    using namespace score::cpp::literals;
    EXPECT_TRUE((std::is_same<std::ptrdiff_t, decltype(42_Z)>::value));
}

} // namespace
