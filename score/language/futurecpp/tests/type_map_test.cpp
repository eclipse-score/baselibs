/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///

#include <score/type_map.hpp>
#include <score/type_map.hpp> // test include guard

#include <gtest/gtest.h>

#include <cstdint>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18221007
TEST(type_mapping, entry_storage_key_field)
{
    std::int32_t validation_key_type;
    double validation_value_type;
    score::cpp::type_map_entry<decltype(validation_key_type), decltype(validation_value_type)> entry;
    constexpr bool has_key_field = std::is_same<decltype(validation_key_type), decltype(entry)::key_type>::value;
    static_assert(has_key_field, "Entry does not have a key_type field (via using)");
    EXPECT_TRUE(has_key_field);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18221007
TEST(type_mapping, entry_storage_value_field)
{
    std::int32_t validation_key_type;
    double validation_value_type;
    score::cpp::type_map_entry<decltype(validation_key_type), decltype(validation_value_type)> entry;
    constexpr bool has_value_field = std::is_same<decltype(validation_value_type), decltype(entry)::value_type>::value;
    static_assert(has_value_field, "Entry does not have a key_type field (via using)");
    EXPECT_TRUE(has_value_field);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18221007
TEST(type_mapping, extract_entry)
{
    std::int32_t validation_key1_type;
    std::uint32_t validation_key2_type;
    double validation_value1_type;
    std::int64_t validation_value2_type;

    using mapping = score::cpp::mapping<score::cpp::type_map_entry<decltype(validation_key1_type), decltype(validation_value1_type)>,
                                 score::cpp::type_map_entry<decltype(validation_key2_type), decltype(validation_value2_type)>>;
    constexpr bool is_second_validation_value =
        std::is_same<decltype(validation_value2_type),
                     score::cpp::type_map_t<decltype(validation_key2_type), mapping>::value_type>::value;
    static_assert(is_second_validation_value,
                  "The look-up is not correct, since the requested value-type for the given key does not match");
    EXPECT_TRUE(is_second_validation_value);
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#18221872
TEST(type_mapping, consistency)
{
    {
        constexpr score::cpp::detail::consistency<score::cpp::type_map_entry<int, int>, score::cpp::type_map_entry<float, int>> c{};

        static_assert(c.are_all_keys_unique(), "failure");
        EXPECT_TRUE(c.are_all_keys_unique());
    }
    {
        constexpr score::cpp::detail::consistency<score::cpp::type_map_entry<int, int>, score::cpp::type_map_entry<int, int>> c{};

        static_assert(!c.are_all_keys_unique(), "failure");
        EXPECT_FALSE(c.are_all_keys_unique());
    }
    {
        constexpr score::cpp::detail::consistency<score::cpp::type_map_entry<int, int>, score::cpp::type_map_entry<int, float>> c{};

        static_assert(!c.are_all_keys_unique(), "failure");
        EXPECT_FALSE(c.are_all_keys_unique());
    }
}

} // namespace
