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

#include <gtest/gtest.h>

namespace
{

enum color
{
    red,
    green,
    blue,
    black,
    purple,
};

const auto map = std::array<std::tuple<color, int, std::string>, 5>{
    // clang-format off
    std::make_tuple(red,   1, "red"),
    std::make_tuple(green, 2, "green"),
    std::make_tuple(blue,  3, "blue"),
    std::make_tuple(black, 4, "black"),
    std::make_tuple(blue,  5, "blue")
    // clang-format on
};

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899979
TEST(multidirectional_map_to_test, having_one_entry_in_the_map)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.MultidirectionalMapLookup");
    ::testing::Test::RecordProperty("given", "a multidirectional map where each key appears only once");
    ::testing::Test::RecordProperty("when", "map_to looks up an existing key in either direction");
    ::testing::Test::RecordProperty("then", "the associated value is returned");

    const auto result_string = score::cpp::map_to<std::string>(green, map);
    ASSERT_TRUE(result_string.has_value());
    EXPECT_EQ("green", result_string.value());

    const auto result_color = score::cpp::map_to<color>(std::string{"green"}, map);
    ASSERT_TRUE(result_color.has_value());
    EXPECT_EQ(green, result_color.value());

    const auto result_int = score::cpp::map_to<int>(std::string{"black"}, map);
    ASSERT_TRUE(result_int.has_value());
    EXPECT_EQ(4, result_int.value());

    const auto result_color_same_type = score::cpp::map_to<color>(green, map);
    ASSERT_TRUE(result_color_same_type.has_value());
    EXPECT_EQ(green, result_color_same_type.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899979
TEST(multidirectional_map_to_test, having_more_than_one_entry_in_the_map)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.MultidirectionalMapLookup");
    ::testing::Test::RecordProperty("given",
                                    "a multidirectional map containing duplicate keys mapped to different values");
    ::testing::Test::RecordProperty("when", "map_to looks up the duplicated key");
    ::testing::Test::RecordProperty("then", "the value of the first matching entry is returned");

    const auto result_int = score::cpp::map_to<int>(blue, map);
    ASSERT_TRUE(result_int.has_value());
    EXPECT_EQ(3, result_int.value());
    // this is here to emphasize that the first map entry found is taken:
    EXPECT_NE(5, result_int.value());
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#17899979
TEST(multidirectional_map_to_test, having_no_entry_in_the_map)
{
    ::testing::Test::RecordProperty("lobster-tracing", "amp.MultidirectionalMapLookup");
    ::testing::Test::RecordProperty("given", "a multidirectional map");
    ::testing::Test::RecordProperty("when", "map_to looks up a key that is not present in the map");
    ::testing::Test::RecordProperty("then", "an empty optional is returned");

    EXPECT_FALSE(score::cpp::map_to<std::string>(purple, map).has_value());
    EXPECT_FALSE(score::cpp::map_to<color>(15, map).has_value());
    EXPECT_FALSE(score::cpp::map_to<int>(std::string{"orange"}, map).has_value());
}

} // namespace
