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

#include <score/private/memory/construct_at.hpp>
#include <score/private/memory/construct_at.hpp> // check include guard

#include <gtest/gtest.h>

namespace
{

// NOTRACING
TEST(ConstructAtTest, ConstructAtDefault)
{
    int storage{23};
    const int* const result{score::cpp::detail::construct_at(&storage)};
    EXPECT_EQ(result, &storage);
    EXPECT_EQ(*result, 0);
}

// NOTRACING
TEST(ConstructAtTest, ConstructAtWithArgument)
{
    int storage{23};
    const int argument{42};
    const int* const result{score::cpp::detail::construct_at(&storage, argument)};
    EXPECT_EQ(*result, argument);
}

} // namespace
