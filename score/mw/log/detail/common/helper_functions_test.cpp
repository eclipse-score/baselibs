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
#include "score/mw/log/detail/common/helper_functions.h"
#include <tuple>

#include "gtest/gtest.h"

namespace helper
{
using testing::Types;
TEST(SumTest, SumNoOverflow)
{
    std::numeric_limits<std::size_t>::max();
    const std::size_t expected_value = 6U;
    std::size_t result = Sum<uint8_t>(1U, 2U, 3U);
    EXPECT_EQ(result, expected_value);
}

TEST(SumTest, SumOverflow)
{
    std::size_t v1 = std::numeric_limits<std::size_t>::max() - 1U;
    std::size_t v2 = 2U;
    std::size_t v3 = 3U;

    const std::size_t expected_value = std::numeric_limits<std::size_t>::max();
    std::size_t result1 = Sum<std::size_t>(v1, v2);
    EXPECT_EQ(result1, expected_value);
    std::size_t result2 = Sum<std::size_t>(v1, v2, v3);
    EXPECT_EQ(result2, expected_value);
}

TEST(ClampAddNullTerminatorTest, ClampAddNullTerminatorNoOverFlow)
{
    std::uint16_t v1 = 1U;
    constexpr std::uint16_t expected_value = 2U;
    std::size_t result1 = ClampAddNullTerminator(v1);
    EXPECT_EQ(result1, expected_value);
}

TEST(ClampAddNullTerminatorTest, ClampAddNullTerminatorOverFlow)
{
    std::uint16_t v1 = std::numeric_limits<std::uint16_t>::max();
    constexpr std::uint16_t expected_value = std::numeric_limits<std::uint16_t>::max();
    std::size_t result1 = ClampAddNullTerminator(v1);
    EXPECT_EQ(result1, expected_value);
}

TEST(ClampTest, ClampSourceOverflow)
{
    using SourceType = std::uint32_t;
    using TargetType = std::uint8_t;

    static_assert(std::numeric_limits<SourceType>::max() > std::numeric_limits<TargetType>::max(),
                  "SourceType type must be greater than TargetType");

    constexpr SourceType source = std::numeric_limits<SourceType>::max();
    const auto result = helper::ClampTo<TargetType>(source);
    EXPECT_EQ(result, std::numeric_limits<TargetType>::max());
}

}  // namespace helper
