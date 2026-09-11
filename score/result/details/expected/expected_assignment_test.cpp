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

#include "score/result/details/expected/test_types.h"

#include <gtest/gtest.h>

#include <type_traits>

namespace score::details
{
namespace
{

TEST(ExpectedTest, IsCopyAssignableWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that copy-assigning one value-holding expected to another replaces the target's value "
                   "with the source's.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    std::int32_t value{14};
    expected<CopyableType, ErrorType> e1{value};
    expected<CopyableType, ErrorType> e2{value + 1};

    // When copy assigning the first to the second
    e2 = e1;

    // Then both expected have the value of the first
    ASSERT_TRUE(e1.has_value());
    EXPECT_EQ(e1->value_, value);
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->value_, value);
}

TEST(ExpectedTest, IsCopyAssignableWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that copy-assigning one error-holding expected to another replaces the target's error "
                   "with the source's.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different errors
    std::int32_t error{14};
    expected<ValueType, CopyableType> e1{unexpected{error}};
    expected<ValueType, CopyableType> e2{unexpected{error + 1}};

    // When copy assigning the first to the second
    e2 = e1;

    // Then both expected have the error of the first
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().value_, error);
}

TEST(ExpectedTest, IsMoveAssignableWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that move-assigning a value-holding expected transfers its value into the target.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    std::int32_t value{14};
    expected<NothrowMoveOnlyType, ErrorType> e1{value};
    expected<NothrowMoveOnlyType, ErrorType> e2{value + 1};

    // When move assigning the first to the second
    e2 = std::move(e1);

    // Then the second expected has the value of the first
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->value_, value);
}

TEST(ExpectedTest, IsMoveAssignableWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that move-assigning an error-holding expected transfers its error into the target.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different errors
    std::int32_t error{14};
    expected<ValueType, NothrowMoveOnlyType> e1{unexpected{error}};
    expected<ValueType, NothrowMoveOnlyType> e2{unexpected{error + 1}};

    // When move assigning the first to the second
    e2 = std::move(e1);

    // Then the second expected has the error of the first
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().value_, error);
}

TEST(ExpectedTest, MoveAssignmentHasCorrectNoexcept)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected's move assignment is noexcept only when both the value and error types "
                   "are themselves nothrow move-assignable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(std::is_nothrow_move_assignable_v<expected<NothrowMoveOnlyType, NothrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_assignable_v<expected<NothrowMoveOnlyType, ThrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_assignable_v<expected<ThrowMoveOnlyType, NothrowMoveOnlyType>>);
}

TEST(ExpectedTest, CanCopyAssignFromCompatibleType)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that assigning a value of a compatible type sets the expected to hold that value and "
                   "returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable type and an expected
    std::int32_t value{14};
    CopyableType wrapped{14};
    expected<CompatibleCopyableType, ErrorType> unit{unexpect};

    // When assigning the value to an expected
    const auto& result = (unit = wrapped);

    // Then expect the expected to hold the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit->inner_.value_, value);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedTest, CanMoveAssignFromCompatibleType)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that move-assigning a value of a compatible type sets the expected to hold that value "
                   "and returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable type and an expected
    std::int32_t value{14};
    NothrowMoveOnlyType wrapped{14};
    expected<CompatibleNothrowMoveOnlyType, ErrorType> unit{unexpect};

    // When assigning the value to an expected
    const auto& result = (unit = std::move(wrapped));

    // Then expect the expected to hold the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit->inner_.value_, value);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedTest, CanCopyAssignFromUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that assigning an unexpected of a compatible type sets the expected to hold that error "
                   "and returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable unexpected and an expected
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};
    expected<ValueType, CompatibleCopyableType> unit{};

    // When assigning the error to an expected
    const auto& result = (unit = wrapped);

    // Then expect the expected to hold the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().inner_.value_, error);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedTest, CanMoveAssignFromUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that move-assigning an unexpected of a compatible type sets the expected to hold that "
                   "error and returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable unexpected and an expected
    std::int32_t error{14};
    unexpected<NothrowMoveOnlyType> wrapped{error};
    expected<ValueType, CompatibleNothrowMoveOnlyType> unit{};

    // When assigning the error to an expected
    const auto& result = (unit = std::move(wrapped));

    // Then expect the expected to hold the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().inner_.value_, error);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedTest, CanEmplaceWithArgs)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that emplace() constructs the held value in place from forwarded constructor arguments "
                   "and returns a reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with error
    expected<ArgumentType, ErrorType> unit{unexpect};
    std::int32_t copyable{29};
    std::int32_t moveonly{51};

    // When emplacing a value
    auto& result = unit.emplace(CopyableType{copyable}, NothrowMoveOnlyType{moveonly});

    // Then expect the expected to hold the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit->copyable_.value_, copyable);
    EXPECT_EQ(unit->moveonly_.value_, moveonly);

    // And result of the emplacement is a reference to the value
    EXPECT_EQ(result.copyable_.value_, copyable);
    EXPECT_EQ(result.moveonly_.value_, moveonly);
}

TEST(ExpectedTest, CanEmplaceWithInitializerListAndArgs)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that emplace() constructs the held value in place from an initializer list plus "
                   "forwarded constructor arguments.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with error
    expected<ArgumentInitializerListType, ErrorType> unit{unexpect};
    std::int32_t copyable{29};
    std::int32_t moveonly{51};

    // When emplacing a value
    auto& result = unit.emplace({CopyableType{copyable}}, NothrowMoveOnlyType{moveonly});

    // Then expect the expected to hold the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit->copyable_.value_, copyable);
    EXPECT_EQ(unit->moveonly_.value_, moveonly);

    // And result of the emplacement is a reference to the value
    EXPECT_EQ(result.copyable_.value_, copyable);
    EXPECT_EQ(result.moveonly_.value_, moveonly);
}

TEST(ExpectedTest, CanSwapWithMemberSwap)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that the member swap() function exchanges the value/error states of two expected instances.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    std::int32_t v1{82};
    expected<CopyableType, NothrowMoveOnlyType> e1{v1};
    std::int32_t v2{30};
    expected<CopyableType, NothrowMoveOnlyType> e2{unexpect, v2};

    // When swapping the two with the member swap
    e1.swap(e2);

    // Then the value and error are swapped
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, v2);
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->value_, v1);
}

TEST(ExpectedTest, SwapHasCorrectNoexceptSpecification)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected's swap is noexcept only when both the value and error types are "
                   "themselves nothrow swappable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(std::is_nothrow_swappable_v<expected<NothrowMoveOnlyType, NothrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_swappable_v<expected<ThrowMoveOnlyType, NothrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_swappable_v<expected<NothrowMoveOnlyType, ThrowMoveOnlyType>>);
}

TEST(ExpectedTest, CanSwapWithStdSwap)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that std::swap exchanges the value/error states of two expected instances.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    std::int32_t v1{82};
    expected<CopyableType, NothrowMoveOnlyType> e1{v1};
    std::int32_t v2{30};
    expected<CopyableType, NothrowMoveOnlyType> e2{unexpect, v2};

    // When swapping the two with the member swap
    std::swap(e1, e2);

    // Then the value and error are swapped
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, v2);
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->value_, v1);
}

TEST(ExpectedVoidTest, IsCopyAssignableWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that copy-assigning an error-holding expected<void, E> replaces the target's error with "
                   "the source's.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different errors
    std::int32_t error{14};
    expected<void, CopyableType> e1{unexpected{error}};
    expected<void, CopyableType> e2{};

    // When copy assigning the first to the second
    e2 = e1;

    // Then both expected have the error of the first
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().value_, error);
}

TEST(ExpectedVoidTest, IsMoveAssignableWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that move-assigning an error-holding expected<void, E> transfers its error into the "
                   "target.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different errors
    std::int32_t error{14};
    expected<void, NothrowMoveOnlyType> e1{unexpected{error}};
    expected<void, NothrowMoveOnlyType> e2{};

    // When move assigning the first to the second
    e2 = std::move(e1);

    // Then the second expected has the error of the first
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().value_, error);
}

TEST(ExpectedVoidTest, MoveAssignmentHasCorrectNoexcept)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected<void, E>'s move assignment is noexcept only when E is itself nothrow "
                   "move-assignable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(std::is_nothrow_move_assignable_v<expected<void, NothrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_assignable_v<expected<void, ThrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, CanCopyAssignFromUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that assigning an unexpected of a compatible type sets a value-less expected<void, E> "
                   "to hold that error and returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable unexpected and an expected
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};
    expected<void, CompatibleCopyableType> unit{};

    // When assigning the error to an expected
    const auto& result = (unit = wrapped);

    // Then expect the expected to hold the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().inner_.value_, error);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedVoidTest, CanMoveAssignFromUnexpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that move-assigning an unexpected of a compatible type sets a value-less "
                   "expected<void, E> to hold that error and returns a reference to the expected itself.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable unexpected and an expected
    std::int32_t error{14};
    unexpected<NothrowMoveOnlyType> wrapped{error};
    expected<void, CompatibleNothrowMoveOnlyType> unit{};

    // When assigning the error to an expected
    const auto& result = (unit = std::move(wrapped));

    // Then expect the expected to hold the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().inner_.value_, error);

    // And the return value of the assignment is a reference to the expected
    EXPECT_EQ(&result, &unit);
}

TEST(ExpectedVoidTest, CanEmplace)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description", "Check that emplace() on a value-less expected<void, E> puts it into the valid state.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with error
    expected<void, ErrorType> unit{unexpect};

    // When emplacing
    unit.emplace();

    // Then expect the expected to be valid
    ASSERT_TRUE(unit.has_value());
}

TEST(ExpectedVoidTest, CanSwapWithMemberSwap)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that the member swap() function exchanges the states of two expected<void, E> instances.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    expected<void, NothrowMoveOnlyType> e1{};
    std::int32_t error{30};
    expected<void, NothrowMoveOnlyType> e2{unexpect, error};

    // When swapping the two with the member swap
    e1.swap(e2);

    // Then the value and error are swapped
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_TRUE(e2.has_value());
}

TEST(ExpectedVoidTest, SwapHasCorrectNoexceptSpecification)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected<void, E>'s swap is noexcept only when E is itself nothrow swappable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(std::is_nothrow_swappable_v<expected<void, NothrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_swappable_v<expected<void, ThrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, CanSwapWithStdSwap)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that std::swap exchanges the states of two expected<void, E> instances.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given two expected with different values
    expected<void, NothrowMoveOnlyType> e1{};
    std::int32_t error{30};
    expected<void, NothrowMoveOnlyType> e2{unexpect, error};

    // When swapping the two with the member swap
    std::swap(e1, e2);

    // Then the value and error are swapped
    ASSERT_FALSE(e1.has_value());
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_TRUE(e2.has_value());
}

}  // namespace
}  // namespace score::details
