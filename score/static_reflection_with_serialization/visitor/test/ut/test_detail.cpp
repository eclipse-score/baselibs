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
#include "static_reflection_with_serialization/visitor/visit.h"
#include "static_reflection_with_serialization/visitor/visit_as_struct.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <string>
#include <type_traits>

// Ihis code is testing the current implementations detail of struct visitor
// This is not public interface.
// No assumptions should be made about its support in future.

template <std::size_t N>
constexpr inline bool check_type_span(const char (&pretty_name)[N], std::size_t first, std::size_t second)
{
    auto pair = ::score::common::visitor::detail::visitor_extract_type_span(pretty_name);
    return pair.first == first && pair.second == second;
}

TEST(detail, extract_type)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Logging library shall provide an annotation mechanism for data structures to support automatic "
                   "serialization/deserialization.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    const auto& likely_format = "static constexpr auto& test::struct_visitable_impl<test::S1>::namedata()";
    const auto type_string = ::score::common::visitor::detail::visitor_extract_type<std::string>(likely_format);
    EXPECT_STREQ(type_string.c_str(), "test::S1");

    static_assert(check_type_span("q<er>y", 2, 4), "normal logic - with angle brackets");
    static_assert(check_type_span("qwerty", 0, 6), "backup logic - without angle brackets");
    static_assert(check_type_span("q<erty", 2, 6), "abnormal logic - with left bracket");
    static_assert(check_type_span("qwer>y", 0, 6), "abnormal logic - with right bracket");
    static_assert(check_type_span("qw<>ty", 3, 3), "abnormal logic - with empty brackets");
    static_assert(check_type_span("", 0, 0), "logic with empty string");
    static_assert(check_type_span("q<er >y", 2, 4), "trailing whitespace shall be stripped");
}

TEST(detail, skip_trailing_space)
{
    RecordProperty("ParentRequirement", "SCR-1633893");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that 'strip_trailing_spaces' API shall return the value of the last parameter provided if "
                   "it gets a value out of range or a value zero for parameter 'end'.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    constexpr std::size_t expected_out_of_bounds_end_value = 16;
    constexpr std::size_t expected_zero_output_when_zero_input_end_value = 0;
    auto& simple_text = "simple text";
    //  Whenever 'strip_trailing_spaces' gets parameter 'end' value out of range it returns the value of the last
    //  parameter provided:
    EXPECT_EQ(score::common::visitor::detail::strip_trailing_spaces(
                  simple_text, 0, expected_zero_output_when_zero_input_end_value),
              expected_zero_output_when_zero_input_end_value);
    EXPECT_EQ(score::common::visitor::detail::strip_trailing_spaces(simple_text, 0, expected_out_of_bounds_end_value),
              expected_out_of_bounds_end_value);

    auto& text_of_spaces_bigger_than_or_equal_to_the_bound = "                 ";
    EXPECT_EQ(score::common::visitor::detail::strip_trailing_spaces(
                  text_of_spaces_bigger_than_or_equal_to_the_bound, 0, expected_out_of_bounds_end_value),
              1);
}
