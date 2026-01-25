/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

#include <score/blank.hpp>
#include <score/blank.hpp> // include guard test

#include <gtest/gtest.h>

namespace
{

/// \test `blank` shall compare equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, equality)
{
    const score::cpp::blank b{};
    EXPECT_TRUE(b == b);
}

/// \test `blank` shall not compare unequal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, inequality)
{
    const score::cpp::blank b{};
    EXPECT_FALSE(b != b);
}

/// \test `blank` shall not compare less than
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, less_than)
{
    const score::cpp::blank b{};
    EXPECT_FALSE(b < b);
}

/// \test `blank` shall not compare greater than
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, greater_than)
{
    const score::cpp::blank b{};
    EXPECT_FALSE(b > b);
}

/// \test `blank` shall compare less than or equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, less_than_or_equal)
{
    const score::cpp::blank b{};
    EXPECT_TRUE(b <= b);
}

/// \test `blank` shall compare greater than or equal
/// @testmethods TM_REQUIREMENT
/// @requirement CB-#16977307
TEST(blank_test, greater_than_or_equal)
{
    const score::cpp::blank b{};
    EXPECT_TRUE(b >= b);
}

} // namespace
