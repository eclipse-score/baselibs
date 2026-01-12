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
    static_assert(!std::is_copy_constructible_v<expected<CopyableType, NothrowMoveOnlyType>>);
    static_assert(!std::is_copy_constructible_v<expected<NothrowMoveOnlyType, CopyableType>>);
}

TEST(ExpectedTest, IsMoveConstructibleWhenInnerTypesAreMoveConstructibleAndWithValue)
{
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
    static_assert(!std::is_move_constructible_v<expected<NothrowMoveOnlyType, UnmovableType>>);
    static_assert(!std::is_move_constructible_v<expected<UnmovableType, NothrowMoveOnlyType>>);
}

TEST(ExpectedTest, IsOnlyNothrowMoveConstructibleIfInnerTypesAre)
{
    static_assert(!std::is_nothrow_move_constructible_v<expected<ThrowMoveOnlyType, ThrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_constructible_v<expected<NothrowMoveOnlyType, ThrowMoveOnlyType>>);
    static_assert(!std::is_nothrow_move_constructible_v<expected<ThrowMoveOnlyType, NothrowMoveOnlyType>>);
    static_assert(std::is_nothrow_move_constructible_v<expected<NothrowMoveOnlyType, NothrowMoveOnlyType>>);
}

TEST(ExpectedTest, CanCopyConstructFromCompatibleExpectedWithValue)
{
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
    static_assert(std::is_trivially_destructible_v<expected<ValueType, ErrorType>>);
}

TEST(ExpectedTest, CanWrapExpected)
{
    static_assert(std::is_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_move_constructible_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_assignable_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
    static_assert(std::is_move_assignable_v<expected<expected<ValueType, ErrorType>, ErrorType>>);
}

TEST(ExpectedVoidTest, IsDefaultConstructibleWhenValueTypeIsVoid)
{
    // Given default-constructing expected with void type
    expected<void, ErrorType> expected{};

    // Then it contains a value
    EXPECT_TRUE(expected.has_value());
}

TEST(ExpectedVoidTest, IsCopyConstructibleWhenErrorIsCopyConstructuble)
{
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
    static_assert(!std::is_copy_constructible_v<expected<void, NothrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, IsMoveConstructibleWhenErrorisMoveConstructible)
{
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
    static_assert(!std::is_move_constructible_v<expected<void, UnmovableType>>);
}

TEST(ExpectedVoidTest, IsOnlyNothrowMoveConstructibleIfErrorIs)
{
    static_assert(!std::is_nothrow_move_constructible_v<expected<void, ThrowMoveOnlyType>>);
    static_assert(std::is_nothrow_move_constructible_v<expected<void, NothrowMoveOnlyType>>);
}

TEST(ExpectedVoidTest, CanCopyConstructFromCompatibleExpectedWithError)
{
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
    // When constructing an expected in-place
    expected<void, ErrorType> unit{std::in_place};

    // Then the expected holds the value
    ASSERT_TRUE(unit.has_value());
}

TEST(ExpectedVoidTest, CanInPlaceConstructError)
{
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
    static_assert(std::is_trivially_destructible_v<expected<void, ErrorType>>);
}

TEST(ExpectedVoidTest, CanWrapExpected)
{
    static_assert(std::is_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_move_constructible_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_copy_assignable_v<expected<expected<void, ErrorType>, ErrorType>>);
    static_assert(std::is_move_assignable_v<expected<expected<void, ErrorType>, ErrorType>>);
}

}  // namespace
}  // namespace score::details
