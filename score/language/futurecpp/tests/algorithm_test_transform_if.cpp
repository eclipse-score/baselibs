/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// @file
/// @copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/algorithm.hpp>
#include <score/algorithm.hpp> // test include guard

#include <array>
#include <forward_list>
#include <list>
#include <vector>

#include <gtest/gtest.h>

namespace score::cpp
{
namespace
{
int test_transform(const double value) { return 2 * static_cast<int>(value); }
bool test_predicate(const double value) { return value < 4.0; }

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42720373
TEST(transform_if, ForwardIterator)
{
    const std::forward_list<double> input{{1.1, 2.2, 3.3, 4.4, 5.5, 6.6}};
    std::array<int, 42> output{};

    const auto out_end =
        transform_if(std::cbegin(input), std::cend(input), std::begin(output), test_transform, test_predicate);

    ASSERT_EQ(std::distance(output.begin(), out_end), 3);
    EXPECT_EQ(output[0], 2);
    EXPECT_EQ(output[1], 4);
    EXPECT_EQ(output[2], 6);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42720373
TEST(transform_if, BidirectionalIterator)
{
    const std::list<double> input{{1.1, 2.2, 3.3, 4.4, 5.5, 6.6}};
    std::array<int, 42> output{};

    const auto out_end =
        transform_if(std::cbegin(input), std::cend(input), std::begin(output), test_transform, test_predicate);

    ASSERT_EQ(std::distance(output.begin(), out_end), 3);
    EXPECT_EQ(output[0], 2);
    EXPECT_EQ(output[1], 4);
    EXPECT_EQ(output[2], 6);
}

///
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#42720373
TEST(transform_if, RandomAccessIterator)
{
    const std::vector<double> input{{1.1, 2.2, 3.3, 4.4, 5.5, 6.6}};
    std::array<int, 42> output{};

    const auto out_end =
        transform_if(std::cbegin(input), std::cend(input), std::begin(output), test_transform, test_predicate);

    ASSERT_EQ(std::distance(output.begin(), out_end), 3);
    EXPECT_EQ(output[0], 2);
    EXPECT_EQ(output[1], 4);
    EXPECT_EQ(output[2], 6);
}
} // namespace
} // namespace score::cpp
