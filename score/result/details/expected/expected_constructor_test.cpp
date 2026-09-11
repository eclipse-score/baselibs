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

TEST(ExpectedTest, IsDefaultConstructibleWhenValueTypeIsDefaultConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<T, E> is default constructible and holds a default-constructed value "
                   "when T is default constructible.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a default-constructible type
    class DefaultConstructibleType
    {
      public:
        // added to make the class non-trivial so that default c-tor won't be optimized out.
        DefaultConstructibleType() {}
    };

    // When default-constructing expected with that type
    expected<DefaultConstructibleType, ErrorType> expected{};

    // Then it contains a value
    EXPECT_TRUE(expected.has_value());
}

TEST(ExpectedTest, IsNotDefaultConstructibleWhenValueTypeIsNotDefaultConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<T, E> is not default constructible when T is not default constructible.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a non-default-constructible type
    class NonDefaultConstructibleType
    {
      public:
        NonDefaultConstructibleType() = delete;
    };

    // Then expected with that type is not default-constructible
    static_assert(!std::is_default_constructible_v<expected<NonDefaultConstructibleType, ErrorType>>);
}

TEST(ExpectedTest, IsCopyConstructibleWhenInnerTypesAreCopyConstructibleAndWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that a value-holding expected<T, E> is trivially copy constructible when T and E are, "
                   "and that the copy holds the same value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected initialized with a copyable value
    std::int32_t value{14};
    expected<CopyableType, ErrorType> unit{value};

    // Then expected is trivially copy-constructible
    static_assert(std::is_trivially_copy_constructible_v<expected<CopyableType, ErrorType>>);

    // When copying with value
    auto copy{unit};

    // Then the values match
    EXPECT_EQ(unit.value().value_, value);
    EXPECT_EQ(copy.value().value_, value);
}

TEST(ExpectedTest, IsCopyConstructibleWhenInnerTypesAreCopyConstructibleAndWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that an error-holding expected<T, E> is trivially copy constructible when T and E are, "
                   "and that the copy holds the same error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an unexpected initialized with a copyable error
    std::int32_t value{14};
    unexpected<CopyableType> wrapped{value};
    expected<ValueType, CopyableType> unit{wrapped};

    // Then expected is trivially copy-constructible
    static_assert(std::is_trivially_copy_constructible_v<expected<ValueType, CopyableType>>);

    // When copying with error
    auto copy{unit};

    // Then the errors match
    EXPECT_EQ(unit.error().value_, value);
    EXPECT_EQ(copy.error().value_, value);
}

TEST(ExpectedTest, IsNotCopyConstructibleWhenInnerTypesAreNotCopyConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<T, E> is not copy constructible when either T or E is not copyable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_copy_constructible_v<expected<CopyableType, NothrowMoveOnlyType>>);
    static_assert(!std::is_copy_constructible_v<expected<NothrowMoveOnlyType, CopyableType>>);
}

TEST(ExpectedTest, IsMoveConstructibleWhenInnerTypesAreMoveConstructibleAndWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that a value-holding expected<T, E> is trivially move constructible when T and E are, "
                   "and that the value is transferred to the moved-to instance.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected initialized with a move-only value
    std::int32_t value{14};
    expected<NothrowMoveOnlyType, ErrorType> unit{value};

    // Then expected is trivially move-constructible
    static_assert(std::is_trivially_move_constructible_v<expected<NothrowMoveOnlyType, ErrorType>>);

    // When copying with value
    auto moved{std::move(unit)};

    // Then the value matches
    EXPECT_EQ(moved.value().value_, value);
}

TEST(ExpectedTest, IsMoveConstructibleWhenInnerTypesAreMoveConstructibleAndWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that an error-holding expected<T, E> is trivially move constructible when T and E are, "
                   "and that the error is transferred to the moved-to instance.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected initialized with a move-only value
    std::int32_t value{14};
    expected<ValueType, NothrowMoveOnlyType> unit{unexpected{value}};

    // Then expected is trivially move-constructible
    static_assert(std::is_trivially_move_constructible_v<expected<ValueType, NothrowMoveOnlyType>>);

    // When copying with value
    auto moved{std::move(unit)};

    // Then the error matches
    EXPECT_EQ(moved.error().value_, value);
}

TEST(ExpectedTest, IsNotMoveConstructibleWhenInnerTypesAreNotMoveConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<T, E> is not move constructible when either T or E is not movable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_move_constructible_v<expected<NothrowMoveOnlyType, UnmovableType>>);
    static_assert(!std::is_move_constructible_v<expected<UnmovableType, NothrowMoveOnlyType>>);
}

TEST(ExpectedTest, IsOnlyNothrowMoveConstructibleIfInnerTypesAre)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected<T, E>'s move constructor is noexcept only when both T and E are "
                   "themselves nothrow move constructible.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_nothrow_move_constructible_v<expected<ThrowMoveOnlyType, ThrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_constructible_v<expected<NothrowMoveOnlyType, ThrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_constructible_v<expected<ThrowMoveOnlyType, NothrowMoveOnlyType>>);
    static_assert(std::is_nothrow_move_constructible_v<expected<NothrowMoveOnlyType, NothrowMoveOnlyType>>);
}

TEST(ExpectedTest, CanCopyConstructFromCompatibleExpectedWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected holding a value can be copy constructed from another expected with a "
                   "convertible value type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with a value
    std::int32_t value{14};
    expected<CopyableType, ErrorType> e1{value};

    // When constructing an expected with a compatible type
    expected<CompatibleCopyableType, ErrorType> e2{e1};

    // Then both expected have the value
    EXPECT_EQ(e1->value_, value);
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->inner_.value_, value);
}

TEST(ExpectedTest, CanCopyConstructFromCompatibleExpectedWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected holding an error can be copy constructed from another expected with a "
                   "convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with an error
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};
    expected<ValueType, CopyableType> e1{wrapped};

    // When constructing an expected with a compatible type
    expected<ValueType, CompatibleCopyableType> e2{e1};

    // Then both expected have the error
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedTest, CanMoveConstructFromCompatibleExpectedWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected holding a value can be move constructed from another expected with a "
                   "convertible value type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with a value
    std::int32_t value{14};
    expected<NothrowMoveOnlyType, ErrorType> e1{value};

    // When constructing an expected with a compatible type
    expected<CompatibleNothrowMoveOnlyType, ErrorType> e2{std::move(e1)};

    // Then both expected have the value
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->inner_.value_, value);
}

TEST(ExpectedTest, CanMoveConstructFromCompatibleExpectedWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected holding an error can be move constructed from another expected with a "
                   "convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with an error
    std::int32_t error{14};
    expected<ValueType, NothrowMoveOnlyType> e1{unexpected{error}};

    // When constructing an expected with a compatible type
    expected<ValueType, CompatibleNothrowMoveOnlyType> e2{std::move(e1)};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedTest, CanCopyConstructFromCompatibleTypeWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected can be copy constructed directly from a value of a convertible type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable value
    std::int32_t value{14};
    CopyableType wrapped{value};

    // When constructing an expected with a compatible type
    expected<CompatibleCopyableType, ErrorType> e2{wrapped};

    // Then both expected have the value
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->inner_.value_, value);
}

TEST(ExpectedTest, CanMoveConstructFromCompatibleTypeWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected can be move constructed directly from a move-only value of a "
                   "convertible type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a move-only a value
    std::int32_t value{14};
    NothrowMoveOnlyType wrapped{value};

    // When constructing an expected with a compatible type
    expected<CompatibleNothrowMoveOnlyType, ErrorType> e2{std::move(wrapped)};

    // Then both expected have the value
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->inner_.value_, value);
}

TEST(ExpectedTest, CanCopyConstructFromCompatibleTypeWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected can be copy constructed directly from an unexpected of a convertible "
                   "error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable error
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};

    // When constructing an expected with a compatible type
    expected<ValueType, CompatibleCopyableType> e2{wrapped};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedTest, CanMoveConstructFromCompatibleTypeWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an expected can be move constructed directly from an unexpected of a move-only, "
                   "convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a move-only error
    std::int32_t error{14};
    NothrowMoveOnlyType wrapped{error};

    // When constructing an expected with a compatible type
    expected<ValueType, CompatibleNothrowMoveOnlyType> e2{unexpected{std::move(wrapped)}};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedTest, CanInPlaceConstructValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected can be constructed in place, forwarding constructor arguments directly to "
                   "the value type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<ArgumentType, ErrorType> unit{std::in_place, CopyableType{copyable}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit.value().copyable_.value_, copyable);
    EXPECT_EQ(unit.value().moveonly_.value_, moveonly);
}

TEST(ExpectedTest, CanInPlaceConstructValueWithInitializerList)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected can be constructed in place from an initializer list plus additional "
                   "constructor arguments forwarded to the value type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<ArgumentInitializerListType, ErrorType> unit{
        std::in_place, {CopyableType{copyable}}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the value
    ASSERT_TRUE(unit.has_value());
    EXPECT_EQ(unit.value().copyable_.value_, copyable);
    EXPECT_EQ(unit.value().moveonly_.value_, moveonly);
}

TEST(ExpectedTest, CanInPlaceConstructError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected can be constructed in an error state in place, forwarding constructor "
                   "arguments directly to the error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<ValueType, ArgumentType> unit{unexpect, CopyableType{copyable}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().copyable_.value_, copyable);
    EXPECT_EQ(unit.error().moveonly_.value_, moveonly);
}

TEST(ExpectedTest, CanInPlaceConstructErrorWithInitializerList)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected can be constructed in an error state in place from an initializer list "
                   "plus additional constructor arguments forwarded to the error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<ValueType, ArgumentInitializerListType> unit{
        unexpect, {CopyableType{copyable}}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().copyable_.value_, copyable);
    EXPECT_EQ(unit.error().moveonly_.value_, moveonly);
}

TEST(ExpectedTest, IsTriviallyDestructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__deterministic_behavior");
    RecordProperty("Description",
                   "Check that expected<T, E> is trivially destructible when T and E are, avoiding any dynamic "
                   "cleanup on teardown.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static_assert(std::is_trivially_destructible_v<expected<ValueType, ErrorType>>);
}

TEST(ExpectedTest, CanWrapExpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that an expected can itself be used as the value type of another expected, preserving "
                   "constructibility, copyability, and movability.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static_assert(std::is_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_move_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_assignable_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_move_assignable_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
}

TEST(ExpectedVoidTest, IsDefaultConstructibleWhenValueTypeIsVoid)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<void, E> is default constructible and default-constructs into the valid "
                   "state.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given default-constructing expected with void type
    expected<void, ErrorType> expected{};

    // Then it contains a value
    EXPECT_TRUE(expected.has_value());
}

TEST(ExpectedVoidTest, IsCopyConstructibleWhenErrorIsCopyConstructuble)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<void, E> is trivially copy constructible when E is, and that the copy "
                   "holds the same error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a void expected initialized with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Then expected is trivially copy-constructible
    static_assert(std::is_trivially_copy_constructible_v<expected<void, CopyableType>>);

    // When copying with value
    auto copy{unit};

    // Then the values match
    EXPECT_EQ(unit.error().value_, error);
    EXPECT_EQ(copy.error().value_, error);
}

TEST(ExpectedVoidTest, IsNotCopyConstructibleWhenErrorIsNotCopyConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that expected<void, E> is not copy constructible when E is not copyable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_copy_constructible_v<expected<void, NothrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, IsMoveConstructibleWhenErrorisMoveConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that expected<void, E> is trivially move constructible when E is, and that the error "
                   "is transferred to the moved-to instance.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected initialized with a move-only error
    std::int32_t error{14};
    expected<void, NothrowMoveOnlyType> unit{unexpected{error}};

    // Then expected is trivially move-constructible
    static_assert(std::is_trivially_move_constructible_v<expected<void, NothrowMoveOnlyType>>);

    // When copying with value
    auto moved{std::move(unit)};

    // Then the error matches
    EXPECT_EQ(moved.error().value_, error);
}

TEST(ExpectedVoidTest, IsNotMoveConstructibleWhenErrorIsNotMoveConstructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that expected<void, E> is not move constructible when E is not movable.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_move_constructible_v<expected<void, UnmovableType>>);
}

TEST(ExpectedVoidTest, IsOnlyNothrowMoveConstructibleIfErrorIs)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__exception_free_operation");
    RecordProperty("Description",
                   "Check that expected<void, E>'s move constructor is noexcept only when E is itself nothrow "
                   "move constructible.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    static_assert(!std::is_nothrow_move_constructible_v<expected<void, ThrowMoveOnlyType>>);
    static_assert(std::is_nothrow_move_constructible_v<expected<void, NothrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, CanCopyConstructFromCompatibleExpectedWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an error-holding expected<void, E> can be copy constructed from another "
                   "expected<void, E> with a convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with an error
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};
    expected<void, CopyableType> e1{wrapped};

    // When constructing an expected with a compatible type
    expected<void, CompatibleCopyableType> e2{e1};

    // Then both expected have the error
    EXPECT_EQ(e1.error().value_, error);
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedVoidTest, CanMoveConstructFromCompatibleExpectedWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that an error-holding expected<void, E> can be move constructed from another "
                   "expected<void, E> with a convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given an expected with an error
    std::int32_t error{14};
    expected<void, NothrowMoveOnlyType> e1{unexpected{error}};

    // When constructing an expected with a compatible type
    expected<void, CompatibleNothrowMoveOnlyType> e2{std::move(e1)};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedVoidTest, CanCopyConstructFromCompatibleTypeWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected<void, E> can be copy constructed directly from an unexpected of a "
                   "convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a copyable error
    std::int32_t error{14};
    unexpected<CopyableType> wrapped{error};

    // When constructing an expected with a compatible type
    expected<void, CompatibleCopyableType> e2{wrapped};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedVoidTest, CanMoveConstructFromCompatibleTypeWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected<void, E> can be move constructed directly from an unexpected of a "
                   "move-only, convertible error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // Given a move-only error
    std::int32_t error{14};
    NothrowMoveOnlyType wrapped{error};

    // When constructing an expected with a compatible type
    expected<void, CompatibleNothrowMoveOnlyType> e2{unexpected{std::move(wrapped)}};

    // Then both expected have the error
    ASSERT_FALSE(e2.has_value());
    EXPECT_EQ(e2.error().inner_.value_, error);
}

TEST(ExpectedVoidTest, CanInPlaceConstruct)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description", "Check that expected<void, E> can be constructed in place into the valid state.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    // When constructing an expected in-place
    expected<void, ErrorType> unit{std::in_place};

    // Then the expected holds the value
    ASSERT_TRUE(unit.has_value());
}

TEST(ExpectedVoidTest, CanInPlaceConstructError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected<void, E> can be constructed in an error state in place, forwarding "
                   "constructor arguments directly to the error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<void, ArgumentType> unit{unexpect, CopyableType{copyable}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().copyable_.value_, copyable);
    EXPECT_EQ(unit.error().moveonly_.value_, moveonly);
}

TEST(ExpectedVoidTest, CanInPlaceConstructErrorWithInitializerList)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that expected<void, E> can be constructed in an error state in place from an "
                   "initializer list plus additional constructor arguments forwarded to the error type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    std::int32_t copyable{15};
    std::int32_t moveonly{17};

    // When constructing an expected in-place
    expected<void, ArgumentInitializerListType> unit{unexpect, {CopyableType{copyable}}, NothrowMoveOnlyType{moveonly}};

    // Then the expected holds the error
    ASSERT_FALSE(unit.has_value());
    EXPECT_EQ(unit.error().copyable_.value_, copyable);
    EXPECT_EQ(unit.error().moveonly_.value_, moveonly);
}

TEST(ExpectedVoidTest, IsTriviallyDestructible)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__deterministic_behavior");
    RecordProperty("Description",
                   "Check that expected<void, E> is trivially destructible when E is, avoiding any dynamic "
                   "cleanup on teardown.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static_assert(std::is_trivially_destructible_v<expected<void, ErrorType>>);
}

TEST(ExpectedVoidTest, CanWrapExpected)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that a value-less expected<void, E> can itself be used as the value type of another "
                   "expected, preserving constructibility, copyability, and movability.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    static_assert(std::is_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_move_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_assignable_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_move_assignable_v<expected<expected<void, ErrorType>, ErrorType>>);
}

}  // namespace
}  // namespace score::details
