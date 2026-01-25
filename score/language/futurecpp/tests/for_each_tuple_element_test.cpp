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

#include <score/for_each_tuple_element.hpp>
#include <score/for_each_tuple_element.hpp> // test include guard

#include <gtest/gtest.h>

namespace
{

/// \brief Spy object that acts as functor accepting int and float objects and stores them upon call

struct functor_spy
{
    void operator()(int integer_value) { captured_int = integer_value; }
    void operator()(float floatingpoint_value) { captured_float = floatingpoint_value; }

    int captured_int{0};
    float captured_float{0};
};

struct for_each_tuple_element_test_fixture : public ::testing::Test
{
    decltype(auto) get_functor()
    {
        return [this](auto&&) { functor_call_count++; };
    }

    decltype(auto) make_one_element_tuple() { return std::make_tuple(arbitrary_integer); }
    decltype(auto) make_two_element_tuple() { return std::make_tuple(arbitrary_integer, arbitrary_float); }

    functor_spy apply_counter_spy{};

    std::size_t functor_call_count{0U};

    const int arbitrary_integer{42};
    const float arbitrary_float{84.0F};
};

/// \test Test: Check apply with empty tuple
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, empty_tuple)
{
    auto empty_tuple = std::make_tuple();

    score::cpp::for_each_tuple_element(empty_tuple, get_functor());

    EXPECT_EQ(functor_call_count, 0U);
}

/// \test Test: Check apply with a tuple of size 1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, tuple_with_one_element)
{
    auto unit = make_one_element_tuple();

    score::cpp::for_each_tuple_element(unit, get_functor());

    EXPECT_EQ(functor_call_count, 1U);
}

/// \test Test: Check apply with a tuple of size >1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, tuple_with_more_than_one_element)
{
    auto unit = make_two_element_tuple();

    score::cpp::for_each_tuple_element(unit, get_functor());

    EXPECT_EQ(functor_call_count, 2U);
}

/// \test Test: Check that apply maps to correct function calls
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, apply_calls_with_actual_value)
{
    auto unit = make_one_element_tuple();

    score::cpp::for_each_tuple_element(unit, apply_counter_spy);

    EXPECT_EQ(apply_counter_spy.captured_int, arbitrary_integer);
}

/// \test Test: Check that apply maps to correct function calls with multiple values
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, apply_calls_with_actual_value_on_multiple_values)
{
    auto unit = make_two_element_tuple();

    score::cpp::for_each_tuple_element(unit, apply_counter_spy);

    EXPECT_EQ(apply_counter_spy.captured_int, arbitrary_integer);
    EXPECT_EQ(apply_counter_spy.captured_float, arbitrary_float);
}

/// \test Test: Check apply with empty tuple
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, const_empty_tuple)
{
    const auto empty_tuple = std::make_tuple();

    score::cpp::for_each_tuple_element(empty_tuple, get_functor());

    EXPECT_EQ(functor_call_count, 0U);
}

/// \test Test: Check apply with a tuple of size 1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, const_tuple_with_one_element)
{
    const auto unit = make_one_element_tuple();

    score::cpp::for_each_tuple_element(unit, get_functor());

    EXPECT_EQ(functor_call_count, 1U);
}

/// \test Test: Check apply with a tuple of size >1
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, const_tuple_with_more_than_one_element)
{
    const auto unit = make_two_element_tuple();

    score::cpp::for_each_tuple_element(unit, get_functor());

    EXPECT_EQ(functor_call_count, 2U);
}

/// \test Test: Check that apply maps to correct function calls
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, const_apply_calls_with_actual_value)
{
    const auto unit = make_one_element_tuple();

    score::cpp::for_each_tuple_element(unit, apply_counter_spy);

    EXPECT_EQ(apply_counter_spy.captured_int, arbitrary_integer);
}

/// \test Test: Check that apply maps to correct function calls with multiple values
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18226719
TEST_F(for_each_tuple_element_test_fixture, const_apply_calls_with_actual_value_on_multiple_values)
{
    const auto unit = make_two_element_tuple();

    score::cpp::for_each_tuple_element(unit, apply_counter_spy);

    EXPECT_EQ(apply_counter_spy.captured_int, arbitrary_integer);
    EXPECT_EQ(apply_counter_spy.captured_float, arbitrary_float);
}

} // namespace
