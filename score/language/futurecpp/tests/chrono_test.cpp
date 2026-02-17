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

#include <score/chrono.hpp>
#include <score/chrono.hpp> // include guard test

#include <chrono>

#include <gtest/gtest.h>

namespace
{

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990907
TEST(ChronoTest, AbsDurationPositive)
{
    const std::chrono::seconds duration{123};

    ASSERT_EQ(score::cpp::chrono::abs(duration), std::chrono::seconds{123});
}

/// @testmethods TM_REQUIREMENT
/// @requirement CB-#7990907
TEST(ChronoTest, AbsDurationNegative)
{
    const std::chrono::seconds duration{-123};

    ASSERT_EQ(score::cpp::chrono::abs(duration), std::chrono::seconds{123});
}

} // namespace
