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
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "design-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that a scoped enum's power-of-two enumerators keep their expected underlying integer values.");

    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::a), 1);
    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::b), 2);
    EXPECT_EQ(static_cast<UnderlyingType>(MyBitmask::c), 4);
}

TEST(MyBitmask, SupportsOperatorOr)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator| combines two bitmask enum values into their bitwise union.");

    MyBitmask bitmask{MyBitmask::a | MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
    bitmask = MyBitmask::b | MyBitmask::c;
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 6);
}

TEST(MyBitmask, SupportsOperatorAnd)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator& reports whether individual flags are present in a combined bitmask value.");

    auto func = [](MyBitmask bitmask) {
        EXPECT_TRUE(bitmask & MyBitmask::a);
        EXPECT_TRUE(bitmask & MyBitmask::b);
        EXPECT_FALSE(bitmask & MyBitmask::c);
    };

    func(MyBitmask::a | MyBitmask::b);
}

TEST(MyBitmask, SupportsOperatorXor)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator^ toggles flags between two bitmask enum values as expected.");

    MyBitmask bitmask{MyBitmask::a ^ MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
    bitmask = bitmask ^ MyBitmask::b;
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 1);
}

TEST(MyBitmask, SupportsOperatorNot)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator~ inverts all flags of a bitmask enum value.");

    MyBitmask bitmask{MyBitmask::a};
    bitmask = ~bitmask;

    EXPECT_FALSE(bitmask & MyBitmask::a);
    EXPECT_TRUE(bitmask & MyBitmask::b);
    EXPECT_TRUE(bitmask & MyBitmask::c);
}

TEST(MyBitmask, SupportsAssignOperatorAnd)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator&= updates a bitmask value in place to the intersection with another value.");

    MyBitmask bitmask{MyBitmask::a};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

TEST(MyBitmask, SupportsAssignOperatorAndMatching)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "equivalence-classes");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator&= leaves a bitmask value unchanged when ANDed with itself.");

    MyBitmask bitmask{MyBitmask::b};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 2);
}

TEST(MyBitmask, SupportsAssignOperatorOr)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator|= updates a bitmask value in place to the union with another value.");

    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorOrMatching)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "equivalence-classes");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator|= leaves a bitmask value unchanged when ORed with itself.");

    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 1);
}

TEST(MyBitmask, SupportsAssignOperatorXOr)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "requirements-analysis");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator^= updates a bitmask value in place by toggling the flags of another value.");

    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorXOrMatching)
{
    ::testing::Test::RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("TestType", "requirements-based");
    ::testing::Test::RecordProperty("DerivationTechnique", "equivalence-classes");
    ::testing::Test::RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    ::testing::Test::RecordProperty("Description", "Check that operator^= clears a bitmask value to zero when XORed with itself.");

    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

}  // namespace
}  // namespace test
}  // namespace score
