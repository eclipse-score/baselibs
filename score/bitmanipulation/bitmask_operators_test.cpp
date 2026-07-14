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
#include "score/bitmanipulation/bitmask_operators.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace score
{
namespace test
{

enum class MyBitmask : std::int32_t
{
    a = 1,
    b = 2,
    c = 4,
};

}  // namespace test

template <>
class enable_bitmask_operators<test::MyBitmask> : public std::true_type
{
};

namespace test
{
namespace
{
using UnderlyingType = std::underlying_type_t<MyBitmask>;

TEST(MyBitmask, UnderlyingValuesMatchExpectations)
{
    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::a), 1);
    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::b), 2);
    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::c), 4);
}

TEST(MyBitmask, SupportsOperatorOr)
{
    MyBitmask bitmask{MyBitmask::a | MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
    bitmask = MyBitmask::b | MyBitmask::c;
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 6);
}

TEST(MyBitmask, SupportsOperatorAnd)
{
    auto func = [](MyBitmask bitmask) {
        EXPECT_TRUE(bitmask & MyBitmask::a);
        EXPECT_TRUE(bitmask & MyBitmask::b);
        EXPECT_FALSE(bitmask & MyBitmask::c);
    };

    func(MyBitmask::a | MyBitmask::b);
}

TEST(MyBitmask, SupportsOperatorXor)
{
    MyBitmask abc{MyBitmask::a ^ MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(abc), 3);
    abc = abc ^ MyBitmask::b;
    EXPECT_EQ(static_cast<UnderlyingType>(abc), 1);
}

TEST(MyBitmask, SupportsOperatorNot)
{
    MyBitmask bitmask{MyBitmask::a};
    bitmask = ~bitmask;

    EXPECT_FALSE(bitmask & MyBitmask::a);
    EXPECT_TRUE(bitmask & MyBitmask::b);
    EXPECT_TRUE(bitmask & MyBitmask::c);
}

TEST(MyBitmask, SupportsAssignOperatorAnd)
{
    MyBitmask bitmask{MyBitmask::a};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

TEST(MyBitmask, SupportsAssignOperatorAndMatching)
{
    MyBitmask bitmask{MyBitmask::b};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 2);
}

TEST(MyBitmask, SupportsAssignOperatorOr)
{
    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorOrMatching)
{
    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 1);
}

TEST(MyBitmask, SupportsAssignOperatorXOr)
{
    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorXOrMatching)
{
    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

}  // namespace
}  // namespace test
}  // namespace score
