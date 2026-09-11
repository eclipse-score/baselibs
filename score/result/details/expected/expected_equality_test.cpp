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
#include "score/result/details/expected/expected.h"

#include <gtest/gtest.h>

namespace score::details
{
namespace
{

struct A1
{
    std::int32_t value;
};
struct A2
{
    std::int32_t value;
};

bool operator==(const A1& a1, const A2& a2)
{
    return a1.value == a2.value;
}

struct B1
{
    std::int32_t value;
};
struct B2
{
    std::int32_t value;
};

bool operator==(const B1& b1, const B2& b2)
{
    return b1.value == b2.value;
}

TEST(ExpectedTest, EqualityBetweenExpectedBothWithValues)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that two expected holding values of different but comparable types compare equal iff "
                   "the values compare equal.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given three expected with different but comparable types where the first two shall compare equal and the third
    // not
    std::int32_t same{46};
    expected<A1, B1> lhs{A1{same}};
    expected<A2, B2> rhs_same{A2{same}};
    expected<A2, B2> rhs_different{A2{same + 1}};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs == rhs_same);
    EXPECT_FALSE(lhs != rhs_same);
    EXPECT_FALSE(lhs == rhs_different);
    EXPECT_TRUE(lhs != rhs_different);
}

TEST(ExpectedTest, EqualityBetweenExpectedBothWithErrors)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that two expected holding errors of different but comparable types compare equal iff "
                   "the errors compare equal.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given three expected with different but comparable types where the first two shall compare equal and the third
    // not
    std::int32_t same{46};
    expected<A1, B1> lhs{unexpect, B1{same}};
    expected<A2, B2> rhs_same{unexpect, B2{same}};
    expected<A2, B2> rhs_different{unexpect, B2{same + 1}};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs == rhs_same);
    EXPECT_FALSE(lhs != rhs_same);
    EXPECT_FALSE(lhs == rhs_different);
    EXPECT_TRUE(lhs != rhs_different);
}

TEST(ExpectedTest, EqualityBetweenExpectedWithValueAndError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that an expected holding a value never compares equal to one holding an error, "
                   "regardless of the contained types.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given two expected with different but comparable types where one has a value and the other an error
    std::int32_t same{46};
    expected<A1, B1> lhs{A1{same}};
    expected<A2, B2> rhs_same{unexpect, B2{same}};

    // Then expect equality operator to behave correctly
    EXPECT_FALSE(lhs == rhs_same);
    EXPECT_TRUE(lhs != rhs_same);
}

TEST(ExpectedTest, EqualityBetweenExpectedAndValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that an expected compares equal to a bare value of a comparable type iff it holds a "
                   "matching value, and never compares equal while holding an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given two expected and two values where only lhs_value and rhs_same shall be equal
    std::int32_t same{46};
    expected<A1, B1> lhs_value{A1{same}};
    expected<A1, B1> lhs_error{unexpect, B1{same}};
    A2 rhs_same{same};
    A2 rhs_different{same + 1};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs_value == rhs_same);
    EXPECT_FALSE(lhs_value != rhs_same);
    EXPECT_FALSE(lhs_value == rhs_different);
    EXPECT_TRUE(lhs_value != rhs_different);
    EXPECT_FALSE(lhs_error == rhs_same);
    EXPECT_TRUE(lhs_error != rhs_same);
}

TEST(ExpectedTest, EqualityBetweenExpectedAndUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that an expected compares equal to an unexpected wrapping a comparable error type iff it "
                   "holds a matching error, and never compares equal while holding a value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given two expected and two values where only lhs_value and rhs_same shall be equal
    std::int32_t same{46};
    expected<A1, B1> lhs_value{A1{same}};
    expected<A1, B1> lhs_error{unexpect, B1{same}};
    unexpected<B2> rhs_same{B2{same}};
    unexpected<B2> rhs_different{B2{same + 1}};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs_error == rhs_same);
    EXPECT_FALSE(lhs_error != rhs_same);
    EXPECT_FALSE(lhs_error == rhs_different);
    EXPECT_TRUE(lhs_error != rhs_different);
    EXPECT_FALSE(lhs_value == rhs_same);
    EXPECT_TRUE(lhs_value != rhs_same);
}

TEST(ExpectedVoidTest, EqualityBetweenExpectedBothWithErrors)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that two value-less expected<void, E> holding errors of different but comparable types "
                   "compare equal iff the errors compare equal.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given three expected with different but comparable types where the first two shall compare equal and the third
    // not
    std::int32_t same{46};
    expected<void, B1> lhs{unexpect, B1{same}};
    expected<void, B2> rhs_same{unexpect, B2{same}};
    expected<void, B2> rhs_different{unexpect, B2{same + 1}};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs == rhs_same);
    EXPECT_FALSE(lhs != rhs_same);
    EXPECT_FALSE(lhs == rhs_different);
    EXPECT_TRUE(lhs != rhs_different);
}

TEST(ExpectedVoidTest, EqualityBetweenExpectedWithValueAndError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that a valid expected<void, E> never compares equal to one holding an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given two expected with different but comparable types where one has a value and the other an error
    std::int32_t error{46};
    expected<void, B1> lhs{};
    expected<void, B2> rhs_same{unexpect, B2{error}};

    // Then expect equality operator to behave correctly
    EXPECT_FALSE(lhs == rhs_same);
    EXPECT_TRUE(lhs != rhs_same);
}

TEST(ExpectedVoidTest, EqualityBetweenExpectedAndUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that an expected<void, E> compares equal to an unexpected wrapping a comparable error "
                   "type iff it holds a matching error, and never compares equal while valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given two expected and two values where only lhs_value and rhs_same shall be equal
    std::int32_t same{46};
    expected<void, B1> lhs_value{};
    expected<void, B1> lhs_error{unexpect, B1{same}};
    unexpected<B2> rhs_same{B2{same}};
    unexpected<B2> rhs_different{B2{same + 1}};

    // Then expect equality operator to behave correctly
    EXPECT_TRUE(lhs_error == rhs_same);
    EXPECT_FALSE(lhs_error != rhs_same);
    EXPECT_FALSE(lhs_error == rhs_different);
    EXPECT_TRUE(lhs_error != rhs_different);
    EXPECT_FALSE(lhs_value == rhs_same);
    EXPECT_TRUE(lhs_value != rhs_same);
}

}  // namespace
}  // namespace score::details
