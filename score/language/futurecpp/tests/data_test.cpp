/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

#include <score/private/iterator/data.hpp>
#include <score/private/iterator/data.hpp> // check include guard

#include <array>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_c_array)
{
    std::int32_t a[1]{};
    EXPECT_EQ(a, score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_c_array)
{
    const std::int32_t a[1]{};
    EXPECT_EQ(a, score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_array)
{
    std::array<std::int32_t, 1> a{};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_array)
{
    const std::array<std::int32_t, 1> a{};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_vector)
{
    std::vector<std::int32_t> a{1};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_const_vector)
{
    const std::vector<std::int32_t> a{1};
    EXPECT_EQ(a.data(), score::cpp::data(a));
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#22116168
TEST(data, data_on_initializer_list)
{
    const std::initializer_list<std::int32_t> a{1};
    EXPECT_EQ(a.begin(), score::cpp::data(a));
}

} // namespace
