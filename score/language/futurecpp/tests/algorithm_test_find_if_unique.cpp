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

#include <score/algorithm.hpp>
#include <score/algorithm.hpp> // test include guard

#include <array>
#include <forward_list>
#include <functional>
#include <limits>
#include <list>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

namespace
{

using find_if_unique_fixture_types =
    testing::Types<std::vector<std::int8_t>, std::list<std::int16_t>, std::forward_list<std::int32_t>>;

template <typename T>
class find_if_unique_fixture : public testing::Test
{
};

TYPED_TEST_SUITE(find_if_unique_fixture, find_if_unique_fixture_types, /*unused*/);

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, check_empty_container)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "an empty container");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for an element equal to 0");
    ::testing::Test::RecordProperty("then", "the end iterator is returned");

    // given
    TypeParam container;

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == 0; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, container.cend());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, element_present_in_container_of_size_one)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "a container with exactly one matching element");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for that element");
    ::testing::Test::RecordProperty("then", "an iterator to that element is returned");

    // given
    TypeParam container = {99};

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == 99; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, container.cbegin());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, element_not_present_in_container_of_size_one)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "a container with exactly one element that does not match");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for a non-matching value");
    ::testing::Test::RecordProperty("then", "the end iterator is returned");

    // given
    TypeParam container = {100};

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == 99; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, container.end());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, element_not_present_in_container_with_multiple_elements)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "a container with several elements none of which match");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for a non-matching value");
    ::testing::Test::RecordProperty("then", "the end iterator is returned");

    // given
    TypeParam container = {-67, -1, 0, 1, 8, 25, 25, 100, 100};

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == 99; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, container.cend());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, element_present_in_container_with_multiple_elements)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "a container with several elements exactly one of which matches");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for that value");
    ::testing::Test::RecordProperty("then", "an iterator to the unique matching element is returned");

    // given
    TypeParam container = {-67, -2, -1, 0, 8, 25, 25, 100, 100};

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == -1; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, std::next(container.cbegin(), 2));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TYPED_TEST(find_if_unique_fixture, element_present_multiple_times_in_container)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given", "a container where more than one element matches the predicate");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for the repeated value");
    ::testing::Test::RecordProperty("then", "the end iterator is returned because the match is not unique");

    // given
    TypeParam container = {0, 0, 5, 1, 8, 100, 100, 25, 25};

    // when
    const auto equal_to = [](const typename TypeParam::value_type& val) { return val == 100; };
    const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

    // then
    EXPECT_EQ(it, container.cend());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TEST(find_if_unique_test, element_present_in_array_any_combination)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty(
        "given", "an array containing exactly one matching element, permuted into every possible ordering");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for that value in each permutation");
    ::testing::Test::RecordProperty("then", "an iterator to the matching element is returned every time");

    // given
    std::array<int, 5> container = {1, 5, 8, 100, 100};

    do
    {
        // when
        const auto equal_to = [](const int val) { return val == 1; };
        const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

        // then
        ASSERT_NE(it, container.cend());
        EXPECT_EQ(*it, 1);

    } while (std::next_permutation(container.begin(), container.end()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TEST(find_if_unique_test, element_not_present_in_array_any_combination)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty("given",
                                    "an array with no matching element, permuted into every possible ordering");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for a non-matching value in each permutation");
    ::testing::Test::RecordProperty("then", "the end iterator is returned every time");

    // given
    std::array<int, 5> container = {1, 5, 8, 100, 100};

    do
    {
        // when
        const auto equal_to = [](const int val) { return val == 10; };
        const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

        // then
        EXPECT_EQ(it, container.cend());

    } while (std::next_permutation(container.begin(), container.end()));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899210
TEST(find_if_unique_test, element_present_multiple_times_in_array_any_combination)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.FindIfUniqueElement");
    ::testing::Test::RecordProperty(
        "given", "an array where the searched-for value appears more than once, permuted into every possible ordering");
    ::testing::Test::RecordProperty("when", "find_if_unique searches for that repeated value in each permutation");
    ::testing::Test::RecordProperty("then", "the end iterator is returned every time because the match is not unique");

    // given
    std::array<int, 5> container = {1, 5, 8, 100, 100};

    do
    {
        // when
        const auto equal_to = [](const int val) { return val == 100; };
        const auto it = score::cpp::find_if_unique(container.cbegin(), container.cend(), equal_to);

        // then
        EXPECT_EQ(it, container.cend());

    } while (std::next_permutation(container.begin(), container.end()));
}

} // namespace
