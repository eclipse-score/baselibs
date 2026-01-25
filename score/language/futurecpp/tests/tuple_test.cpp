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

#include <score/tuple.hpp>
#include <score/tuple.hpp> // check include guard

#include <gtest/gtest.h>

namespace
{
struct test_type
{
    test_type() = default;
    test_type(const int a, const int b) : arg1{a}, arg2{b} {}

    int arg1{0};
    int arg2{0};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8147581
TEST(MakeFromTuple, DefaultConstructor)
{
    const auto result = score::cpp::make_from_tuple<test_type>(std::make_tuple());
    EXPECT_EQ(result.arg1, 0);
    EXPECT_EQ(result.arg2, 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8147581
TEST(MakeFromTuple, WithArguments)
{
    const auto result = score::cpp::make_from_tuple<test_type>(std::make_tuple(1, 2));
    EXPECT_EQ(result.arg1, 1);
    EXPECT_EQ(result.arg2, 2);
}

struct move_only_test_type
{
    move_only_test_type() = default;
    move_only_test_type(const int a) : arg{a} {}

    move_only_test_type(const move_only_test_type&) = delete;
    move_only_test_type(move_only_test_type&& other) : arg{other.arg} { other.arg = 0; }

    move_only_test_type& operator=(const move_only_test_type&) = delete;
    move_only_test_type& operator=(move_only_test_type&& other)
    {
        arg = other.arg;
        other.arg = 0;
        return *this;
    }

    ~move_only_test_type() = default;

    int arg{0};
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#8147581
TEST(MakeFromTuple, PerfectForwarding)
{
    move_only_test_type to_be_moved_from{5};
    const auto result = score::cpp::make_from_tuple<move_only_test_type>(std::make_tuple(std::move(to_be_moved_from)));
    EXPECT_EQ(result.arg, 5);
    EXPECT_EQ(to_be_moved_from.arg, 0);
}
} // namespace
