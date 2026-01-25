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

#include <score/range_pair.hpp>
#include <score/range_pair.hpp> // test include guard

#include <score/size.hpp>
#include <score/utility.hpp>

#include <array>
#include <cstdint>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, make_range_pair_on_c_array_with_first_and_last)
{
    std::int32_t const a[5] = {0, 1, 2, 3, 4};
    auto const rp = score::cpp::make_range_pair(a, a + 5U);
    EXPECT_EQ(rp.begin(), a);
    EXPECT_EQ(rp.first, a);
    EXPECT_EQ(rp.end(), a + 5U);
    EXPECT_EQ(rp.second, a + 5U);
    EXPECT_FALSE(score::cpp::empty(rp));
    EXPECT_EQ(score::cpp::ssize(rp), 5U);
    EXPECT_EQ(rp.size(), 5U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, make_range_pair_on_c_array_with_first_and_size)
{
    std::int32_t const a[5] = {0, 1, 2, 3, 4};
    auto const rp = score::cpp::make_range_pair(a, 5U);
    EXPECT_EQ(rp.begin(), a);
    EXPECT_EQ(rp.first, a);
    EXPECT_EQ(rp.end(), a + 5U);
    EXPECT_EQ(rp.second, a + 5U);
    EXPECT_FALSE(score::cpp::empty(rp));
    EXPECT_EQ(score::cpp::ssize(rp), 5U);
    EXPECT_EQ(rp.size(), 5U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, empty_range)
{
    auto const rp = score::cpp::make_range_pair(static_cast<std::int32_t*>(0), static_cast<std::int32_t*>(0));
    EXPECT_TRUE(score::cpp::empty(rp));
    EXPECT_EQ(score::cpp::ssize(rp), 0);
    EXPECT_EQ(rp.size(), 0);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, at)
{
    std::int32_t a[5] = {0, 1, 2, 3, 4};
    auto rp = score::cpp::make_range_pair(a, 5U);

    score::cpp::at(rp, 1) = 2;

    EXPECT_EQ(2, score::cpp::at(rp, 1));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, construct_range_pair_on_pair)
{
    std::int32_t a[5] = {0, 1, 2, 3, 4};
    score::cpp::range_pair<std::int32_t*> const rp = std::pair<std::int32_t*, std::int32_t*>(a, a + 5);
    EXPECT_EQ(rp.begin(), a);
    EXPECT_EQ(rp.first, a);
    EXPECT_EQ(rp.end(), a + 5U);
    EXPECT_EQ(rp.second, a + 5U);
    EXPECT_FALSE(score::cpp::empty(rp));
    EXPECT_EQ(score::cpp::ssize(rp), 5U);
    EXPECT_EQ(rp.size(), 5U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, swap_a_range_pair)
{
    std::int32_t a[5] = {0, 1, 2, 3, 4};
    auto rp_a = score::cpp::make_range_pair(a, a + 1U);
    auto rp_b = score::cpp::make_range_pair(a + 2U, a + 3U);
    std::swap(rp_a, rp_b);
    EXPECT_EQ(rp_a.begin(), a + 2U);
    EXPECT_EQ(rp_a.end(), a + 3U);
    EXPECT_EQ(rp_b.begin(), a);
    EXPECT_EQ(rp_b.end(), a + 1U);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, assign_range_pair_on_pair)
{
    std::int32_t a[5] = {0, 1, 2, 3, 4};
    // here we also test the non-const version of range_pair
    score::cpp::range_pair<std::int32_t*> rp = score::cpp::make_range_pair(a, a);
    rp = std::pair<std::int32_t*, std::int32_t*>(a, a + 5U);
    EXPECT_EQ(rp.begin(), a);
    EXPECT_EQ(rp.first, a);
    EXPECT_EQ(rp.end(), a + 5);
    EXPECT_EQ(rp.second, a + 5);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18259325
TEST(range_pair, range_pair_in_range_based_for_loop)
{
    std::int32_t const a[5] = {0, 1, 2, 3, 4};
    auto const rp = score::cpp::make_range_pair(a, a + 5U);

    // the actual reason for this class
    std::int32_t i = 0;
    for (auto k : rp)
    {
        EXPECT_EQ(k, i);
        ++i;
    }
    EXPECT_EQ(i, 5U);
    EXPECT_EQ(std::accumulate(rp.begin(), rp.end(), 0), 10U);
}
