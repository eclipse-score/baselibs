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

#include <score/algorithm.hpp>
#include <score/algorithm.hpp> // test include guard

#include <cmath>
#include <cstdint>
#include <functional>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#15969146
TEST(algorithm_test, clamp)
{
    EXPECT_DOUBLE_EQ(5.0, score::cpp::clamp(0.0, 5.0, 5.0, std::greater<double>()));

    // floating point
    EXPECT_FLOAT_EQ(5.0F, score::cpp::clamp(0.0F, 5.0F, 5.0F));

    EXPECT_FLOAT_EQ(5.0F, score::cpp::clamp(4.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-5.0F, score::cpp::clamp(-9.0F, -5.0F, -1.0F));

    EXPECT_FLOAT_EQ(5.0F, score::cpp::clamp(-4.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-5.0F, score::cpp::clamp(-9.0F, -5.0F, 9.0F));

    EXPECT_FLOAT_EQ(5.0F, score::cpp::clamp(5.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-5.0F, score::cpp::clamp(-5.0F, -5.0F, 9.0F));

    EXPECT_FLOAT_EQ(7.0F, score::cpp::clamp(7.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-7.0F, score::cpp::clamp(-7.0F, -9.0F, -5.0F));

    EXPECT_FLOAT_EQ(9.0F, score::cpp::clamp(9.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-5.0F, score::cpp::clamp(-5.0F, -9.0F, -5.0F));

    EXPECT_FLOAT_EQ(9.0F, score::cpp::clamp(10.0F, 5.0F, 9.0F));
    EXPECT_FLOAT_EQ(-5.0F, score::cpp::clamp(-2.0F, -9.0F, -5.0F));

    // integral
    EXPECT_EQ(5, score::cpp::clamp(0, 5, 5));

    EXPECT_EQ(5, score::cpp::clamp(4, 5, 9));
    EXPECT_EQ(-5, score::cpp::clamp(-9, -5, -1));

    EXPECT_EQ(5, score::cpp::clamp(-4, 5, 9));
    EXPECT_EQ(-5, score::cpp::clamp(-9, -5, 9));

    EXPECT_EQ(5, score::cpp::clamp(5, 5, 9));
    EXPECT_EQ(-5, score::cpp::clamp(-5, -5, 9));

    EXPECT_EQ(7, score::cpp::clamp(7, 5, 9));
    EXPECT_EQ(-7, score::cpp::clamp(-7, -9, -5));

    EXPECT_EQ(9, score::cpp::clamp(9, 5, 9));
    EXPECT_EQ(-5, score::cpp::clamp(-5, -9, -5));

    EXPECT_EQ(9, score::cpp::clamp(10, 5, 9));
    EXPECT_EQ(-5, score::cpp::clamp(-2, -9, -5));

    // references
    {
        std::int32_t v = 5;
        std::int32_t lo = 5;
        std::int32_t hi = 9;
        EXPECT_EQ(&v, &score::cpp::clamp(v, lo, hi));
    }
    {
        std::int32_t v = -5;
        std::int32_t lo = -5;
        std::int32_t hi = 9;
        EXPECT_EQ(&v, &score::cpp::clamp(v, lo, hi));
    }

    {
        std::int32_t v = 9;
        std::int32_t lo = 5;
        std::int32_t hi = 9;
        EXPECT_EQ(&v, &score::cpp::clamp(v, lo, hi));
    }
    {
        std::int32_t v = -5;
        std::int32_t lo = -9;
        std::int32_t hi = -5;
        EXPECT_EQ(&v, &score::cpp::clamp(v, lo, hi));
    }
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#15969146
TEST(algorithm_test, constexpr_clamp)
{
    static_assert(score::cpp::equals_bitexact(5.0 - score::cpp::clamp(0.0, 5.0, 5.0, std::greater<double>()), 0.0), "");

    // floating point
    static_assert(score::cpp::equals_bitexact(5.0F - score::cpp::clamp(0.0F, 5.0F, 5.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(5.0F - score::cpp::clamp(4.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-5.0F - score::cpp::clamp(-9.0F, -5.0F, -1.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(5.0F - score::cpp::clamp(-4.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-5.0F - score::cpp::clamp(-9.0F, -5.0F, 9.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(5.0F - score::cpp::clamp(5.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-5.0F - score::cpp::clamp(-5.0F, -5.0F, 9.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(7.0F - score::cpp::clamp(7.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-7.0F - score::cpp::clamp(-7.0F, -9.0F, -5.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(9.0F - score::cpp::clamp(9.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-5.0F - score::cpp::clamp(-5.0F, -9.0F, -5.0F), 0.0F), "");

    static_assert(score::cpp::equals_bitexact(9.0F - score::cpp::clamp(10.0F, 5.0F, 9.0F), 0.0F), "");
    static_assert(score::cpp::equals_bitexact(-5.0F - score::cpp::clamp(-2.0F, -9.0F, -5.0F), 0.0F), "");

    // integral
    static_assert(5 == score::cpp::clamp(0, 5, 5), "");

    static_assert(5 == score::cpp::clamp(4, 5, 9), "");
    static_assert(-5 == score::cpp::clamp(-9, -5, -1), "");

    static_assert(5 == score::cpp::clamp(-4, 5, 9), "");
    static_assert(-5 == score::cpp::clamp(-9, -5, 9), "");

    static_assert(5 == score::cpp::clamp(5, 5, 9), "");
    static_assert(-5 == score::cpp::clamp(-5, -5, 9), "");

    static_assert(7 == score::cpp::clamp(7, 5, 9), "");
    static_assert(-7 == score::cpp::clamp(-7, -9, -5), "");

    static_assert(9 == score::cpp::clamp(9, 5, 9), "");
    static_assert(-5 == score::cpp::clamp(-5, -9, -5), "");

    static_assert(9 == score::cpp::clamp(10, 5, 9), "");
    static_assert(-5 == score::cpp::clamp(-2, -9, -5), "");
}

} // namespace
