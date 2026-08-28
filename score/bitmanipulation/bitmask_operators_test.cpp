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

// Enumerators must be distinct, non-zero powers of two, per aou_req__bitmanipulation__enum_constraints.
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

TEST(MyBitmask, SupportsOperatorOr)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description", "Check that operator| combines two bitmask enumerators into their bitwise union.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a | MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
    bitmask = MyBitmask::b | MyBitmask::c;
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 6);
}

TEST(MyBitmask, SupportsOperatorAnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description",
                   "Check that operator& reports whether each queried bit is present in a combined bitmask.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    auto func = [](MyBitmask bitmask) {
        EXPECT_TRUE(bitmask & MyBitmask::a);
        EXPECT_TRUE(bitmask & MyBitmask::b);
        EXPECT_FALSE(bitmask & MyBitmask::c);
    };

    func(MyBitmask::a | MyBitmask::b);
}

TEST(MyBitmask, SupportsOperatorXor)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description",
                   "Check that operator^ combines two bitmask enumerators into their bitwise exclusive-or.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a ^ MyBitmask::b};
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
    bitmask = bitmask ^ MyBitmask::b;
    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 1);
}

TEST(MyBitmask, SupportsOperatorNot)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description", "Check that operator~ inverts all bits of a bitmask enumerator.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a};
    bitmask = ~bitmask;

    EXPECT_FALSE(bitmask & MyBitmask::a);
    EXPECT_TRUE(bitmask & MyBitmask::b);
    EXPECT_TRUE(bitmask & MyBitmask::c);
}

TEST(MyBitmask, SupportsAssignOperatorAnd)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description",
                   "Check that operator&= clears bits of a bitmask that are not present in the right-hand operand.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

TEST(MyBitmask, SupportsAssignOperatorAndMatching)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty(
        "Description",
        "Check that operator&= leaves a bitmask unchanged when the right-hand operand matches the same bit.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MyBitmask bitmask{MyBitmask::b};

    bitmask &= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 2);
}

TEST(MyBitmask, SupportsAssignOperatorOr)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description",
                   "Check that operator|= sets additional bits of a bitmask from the right-hand operand.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorOrMatching)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty(
        "Description",
        "Check that operator|= leaves a bitmask unchanged when the right-hand operand matches an already-set bit.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    MyBitmask bitmask{MyBitmask::a};

    bitmask |= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 1);
}

TEST(MyBitmask, SupportsAssignOperatorXOr)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description", "Check that operator^= toggles bits of a bitmask from the right-hand operand.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::b;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 3);
}

TEST(MyBitmask, SupportsAssignOperatorXOrMatching)
{
    RecordProperty("PartiallyVerifies", "comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("lobster-tracing", "CompReqBitmanipulation.comp_req__bitmanipulation__bitmask_operators");
    RecordProperty("Description", "Check that operator^= clears a bitmask to zero when XOR-ed with itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");

    MyBitmask bitmask{MyBitmask::a};

    bitmask ^= MyBitmask::a;

    EXPECT_EQ(static_cast<UnderlyingType>(bitmask), 0);
}

}  // namespace
}  // namespace test
}  // namespace score
