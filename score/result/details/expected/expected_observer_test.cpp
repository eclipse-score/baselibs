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

#include <csignal>
#include <type_traits>

namespace score::details
{
namespace
{

TEST(ExpectedTest, ArrowOperatorConstReturnsPointerToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    const expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When accessing the element through the arrow operator
    const auto* inner = unit.operator->();

    // Expect to observe the value
    EXPECT_EQ(inner->value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.operator->()), const CopyableType*>);
}

TEST(ExpectedTest, ArrowOperatorConstWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator->(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ArrowOperatorReturnsPointerToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When accessing the element through the arrow operator
    const auto* inner = unit.operator->();

    // Expect to observe the value
    EXPECT_EQ(inner->value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.operator->()), CopyableType*>);
}

TEST(ExpectedTest, ArrowOperatorWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator->(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorLValueConstReturnsReferenceToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    const expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When accessing the element through the star operator
    const auto& inner = unit.operator*();

    // Expect to observe the value
    EXPECT_EQ(inner.value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.operator*()), const CopyableType&>);
}

TEST(ExpectedTest, StarOperatorLValueConstWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorLValueReturnsReferenceToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When accessing the element through the star operator
    const auto& inner = unit.operator*();

    // Expect to observe the value
    EXPECT_EQ(inner.value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.operator*()), CopyableType&>);
}

TEST(ExpectedTest, StarOperatorLValueWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorRValueConstReturnsReferenceToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    const expected<NothrowMoveOnlyType, ErrorType> unit{value};

    // When accessing the element through the star operator
    const auto&& inner = std::move(unit).operator*();

    // Expect to observe the value
    EXPECT_EQ(inner.value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(std::move(unit).operator*()), const NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, StarOperatorRValueConstWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = std::move(unit).operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorRValueReturnsReferenceToValue)
{
    // Given an expected with a value
    std::int32_t value{13};
    expected<NothrowMoveOnlyType, ErrorType> unit{value};

    // When accessing the element through the star operator
    const auto&& inner = std::move(unit).operator*();

    // Expect to observe the value
    EXPECT_EQ(inner.value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(std::move(unit).operator*()), NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, StarOperatorRValueWillExitIfNoValueIsStored)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = std::move(unit).operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ExplicitConversionToBoolIsTrueIfHasValue)
{
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be true
    EXPECT_TRUE(value);
}

TEST(ExpectedTest, ExplicitConversionToBoolIsFalseIfHasNoValue)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be false
    EXPECT_FALSE(value);
}

TEST(ExpectedTest, HasValueReturnsTrueIfHasValue)
{
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Then expect has_value to return true
    EXPECT_TRUE(unit.has_value());
}

TEST(ExpectedTest, HasValueReturnsFalseIfHasNoValue)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Then expect it to be false
    EXPECT_FALSE(unit.has_value());
}

TEST(ExpectedTest, CanRetrieveValueFromLValueReference)
{
    // Given an expected with a value
    std::int32_t value{13};
    expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When using the expected as lvalue-reference the error matches the original error
    EXPECT_EQ(unit.value().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.value()), CopyableType&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveValueFromLValueReferenceWithoutValue)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromConstLValueReference)
{
    // Given an expected with a value
    std::int32_t value{13};
    const expected<CopyableType, ErrorType> unit{CopyableType{value}};

    // When using the expected as const-lvalue-reference the error matches the original error
    EXPECT_EQ(unit.value().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.value()), const CopyableType&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveValueFromConstLValueReferenceWithoutValue)
{
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromRValueReference)
{
    // Given an expected with a value
    std::int32_t value{13};
    expected<NothrowMoveOnlyType, ErrorType> unit{NothrowMoveOnlyType{value}};

    // When using the expected as rvalue-reference the error matches the original error
    EXPECT_EQ(std::move(unit).value().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).value()), NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveValueFromRValueReferenceWithoutValue)
{
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromConstRValueReference)
{
    // Given an expected with a value
    std::int32_t value{13};
    const expected<NothrowMoveOnlyType, ErrorType> unit{NothrowMoveOnlyType{value}};

    // When using the expected as const-rvalue-reference the error matches the original error
    EXPECT_EQ(std::move(unit).value().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).value()), const NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveValueFromConstRValueReferenceWithoutValue)
{
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveErrorFromLValueReference)
{
    // Given an expected with an error
    std::int32_t value{13};
    expected<ValueType, CopyableType> unit{unexpect, CopyableType{value}};

    // When using the expected as lvalue-reference the error matches the original error
    EXPECT_EQ(unit.error().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.error()), CopyableType&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveErrorFromLValueReferenceWithValue)
{
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = unit.error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromConstLValueReference)
{
    // Given an expected with an error
    std::int32_t value{13};
    const expected<ValueType, CopyableType> unit{unexpect, CopyableType{value}};

    // When using the expected as const-lvalue-reference the error matches the original error
    EXPECT_EQ(unit.error().value_, value);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.error()), const CopyableType&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveErrorFromConstLValueReferenceWithValue)
{
    // Given an expected with a value
    const expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromRValueReference)
{
    // Given an expected with an error
    std::int32_t value{13};
    expected<ValueType, NothrowMoveOnlyType> unit{unexpect, NothrowMoveOnlyType{value}};

    // When using the expected as rvalue-reference the error matches the original error
    EXPECT_EQ(std::move(unit).error().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).error()), NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveErrorFromRValueReferenceWithValue)
{
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromConstRValueReference)
{
    // Given an expected with an error
    std::int32_t value{13};
    const expected<ValueType, NothrowMoveOnlyType> unit{unexpect, NothrowMoveOnlyType{value}};

    // When using the expected as const-rvalue-reference the error matches the original error
    EXPECT_EQ(std::move(unit).error().value_, value);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).error()), const NothrowMoveOnlyType&&>);
}

TEST(ExpectedTest, AbortsWhenRetrieveErrorFromConstRValueReferenceWithValue)
{
    // Given an expected with a value
    const expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ValueOrConstLValueReturnsValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{19};
    const std::int32_t default_value{11};
    const expected<CopyableType, ErrorType> unit{value};

    // When retrieving the value via value_or
    const auto result = unit.value_or(default_value);

    // Expect the result to be the value
    EXPECT_EQ(result.value_, value);
}

TEST(ExpectedTest, ValueOrConstLValueReturnsDefaultIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t default_value{11};
    const expected<CopyableType, ErrorType> unit{unexpect};

    // When retrieving the value via value_or
    const auto result = unit.value_or(default_value);

    // Expect the result to be the default value
    EXPECT_EQ(result.value_, default_value);
}

TEST(ExpectedTest, ValueOrRValueReturnsValueIfHasValue)
{
    // Given an expected with a value
    const std::int32_t value{19};
    const std::int32_t default_value{11};
    expected<NothrowMoveOnlyType, ErrorType> unit{value};

    // When retrieving the value via value_or
    const auto result = std::move(unit).value_or(default_value);

    // Expect the result to be the value
    EXPECT_EQ(result.value_, value);
}

TEST(ExpectedTest, ValueOrRValueReturnsDefaultIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t default_value{11};
    expected<NothrowMoveOnlyType, ErrorType> unit{unexpect};

    // When retrieving the value via value_or
    const auto result = std::move(unit).value_or(default_value);

    // Expect the result to be the default value
    EXPECT_EQ(result.value_, default_value);
}

TEST(ExpectedTest, ErrorOrConstLValueReturnsErrorIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t value{19};
    const std::int32_t default_error{11};
    const expected<ValueType, CopyableType> unit{unexpect, value};

    // When retrieving the error via error_or
    const auto result = unit.error_or(CopyableType{default_error});

    // Expect the result to be the value
    EXPECT_EQ(result.value_, value);
}

TEST(ExpectedTest, ErrorOrConstLValueReturnsDefaultIfHasValue)
{
    // Given an expected with a value
    const std::int32_t default_error{11};
    const expected<ValueType, CopyableType> unit{};

    // When retrieving the error via error_or
    const auto result = unit.error_or(CopyableType{default_error});

    // Expect the result to be the default value
    EXPECT_EQ(result.value_, default_error);
}

TEST(ExpectedTest, ErrorOrRValueReturnsErrorIfHasNoValue)
{
    // Given an expected with a value
    const std::int32_t value{19};
    const std::int32_t default_error{11};
    expected<ValueType, NothrowMoveOnlyType> unit{unexpect, value};

    // When retrieving the error via error_or
    const auto result = std::move(unit).error_or(NothrowMoveOnlyType{default_error});

    // Expect the result to be the error
    EXPECT_EQ(result.value_, value);
}

TEST(ExpectedTest, ErrorOrRValueReturnsDefaultIfHasValue)
{
    // Given an expected with a value
    const std::int32_t default_error{11};
    expected<ValueType, NothrowMoveOnlyType> unit{};

    // When retrieving the error via error_or
    const auto result = std::move(unit).error_or(NothrowMoveOnlyType{default_error});

    // Expect the result to be the default error
    EXPECT_EQ(result.value_, default_error);
}

TEST(ExpectedVoidTest, StarOperatorWillNotExitIfValid)
{
    // Given an expected with an error
    const expected<void, ErrorType> unit{};

    // Expect an abort if accessing the value
    EXPECT_NO_THROW(unit.operator*());
}

TEST(ExpectedVoidTest, StarOperatorWillExitIfInErrorState)
{
    // Given an expected with an error
    const expected<void, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, ExplicitConversionToBoolIsTrueIfHasValue)
{
    // Given an expected with a value
    expected<void, ErrorType> unit{};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be true
    EXPECT_TRUE(value);
}

TEST(ExpectedVoidTest, ExplicitConversionToBoolIsFalseIfHasNoValue)
{
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be false
    EXPECT_FALSE(value);
}

TEST(ExpectedVoidTest, HasValueReturnsTrueIfHasValue)
{
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Then expect has_value to return true
    EXPECT_TRUE(unit.has_value());
}

TEST(ExpectedVoidTest, HasValueReturnsFalseIfHasNoValue)
{
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // Then expect it to be false
    EXPECT_FALSE(unit.has_value());
}

TEST(ExpectedVoidTest, NoAbortWhenCallValueFromConstLValueReference)
{
    // Given an expected with an error
    const expected<void, ErrorType> unit{};

    // Expect no exception when calling value
    EXPECT_NO_THROW(unit.value());
}

TEST(ExpectedVoidTest, AbortWhenCallValueFromConstLValueReferenceWithoutValue)
{
    // Given an expected with an error
    const expected<void, ErrorType> unit{unexpect};

    // Expect an exception when calling value value
    EXPECT_THROW(unit.value(), std::exception);
}

TEST(ExpectedVoidTest, NoAbortWhenCallValueFromRValueReference)
{
    // Given an expected with an error
    expected<void, ErrorType> unit{};

    // Expect no exception when calling value
    EXPECT_NO_THROW(std::move(unit).value());
}

TEST(ExpectedVoidTest, AbortWhenCallValueFromRValueReferenceWithoutValue)
{
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // Expect an exception when calling value value
    EXPECT_THROW(std::move(unit).value(), std::exception);
}

TEST(ExpectedVoidTest, CanRetrieveErrorFromConstLValueReference)
{
    // Given an expected with an error
    std::int32_t error{13};
    const expected<void, CopyableType> unit{unexpect, CopyableType{error}};

    // When using the expected as const-lvalue-reference the error matches the original error
    EXPECT_EQ(unit.error().value_, error);

    // Expect to be able to retrieve the error with correct type
    static_assert(std::is_same_v<decltype(unit.error()), const CopyableType&>);
}

TEST(ExpectedVoidTest, AbortsWhenRetrieveErrorFromConstLValueReferenceWithValue)
{
    // Given an expected with a value
    const expected<void, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, AbortsWhenRetrieveErrorFromRValueReferenceWithValue)
{
    // Given an expected with a value
    expected<void, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, ErrorOrConstLValueReturnsErrorIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t error{19};
    const std::int32_t default_error{11};
    const expected<void, CopyableType> unit{unexpect, error};

    // When retrieving the error via error_or
    const auto result = unit.error_or(CopyableType{default_error});

    // Expect the result to be the value
    EXPECT_EQ(result.value_, error);
}

TEST(ExpectedVoidTest, ErrorOrConstLValueReturnsDefaultIfHasValue)
{
    // Given a valid expected
    const std::int32_t default_error{11};
    const expected<void, CopyableType> unit{};

    // When retrieving the error via error_or
    const auto result = unit.error_or(CopyableType{default_error});

    // Expect the result to be the default value
    EXPECT_EQ(result.value_, default_error);
}

TEST(ExpectedVoidTest, ErrorOrRValueReturnsErrorIfHasNoValue)
{
    // Given an expected with an error
    const std::int32_t error{19};
    const std::int32_t default_error{11};
    expected<void, NothrowMoveOnlyType> unit{unexpect, error};

    // When retrieving the error via error_or
    const auto result = std::move(unit).error_or(NothrowMoveOnlyType{default_error});

    // Expect the result to be the error
    EXPECT_EQ(result.value_, error);
}

TEST(ExpectedVoidTest, ErrorOrRValueReturnsDefaultIfHasValue)
{
    // Given a valid expected
    const std::int32_t default_error{11};
    expected<void, NothrowMoveOnlyType> unit{};

    // When retrieving the error via error_or
    const auto result = std::move(unit).error_or(NothrowMoveOnlyType{default_error});

    // Expect the result to be the default error
    EXPECT_EQ(result.value_, default_error);
}

TEST(ExpectedVoidTest, CanRetrieveErrorFromRValueReference)
{
    // Given an expected with an error
    std::int32_t error{13};
    expected<void, NothrowMoveOnlyType> unit{unexpect, NothrowMoveOnlyType{error}};

    // When using the expected as rvalue-reference the error matches the original error
    EXPECT_EQ(std::move(unit).error().value_, error);

    // Expect to be able to retrieve the error with correct type
    // Double move fine because the operand of decltype is unevaluated:
    // https://timsong-cpp.github.io/cppwp/n4659/dcl.spec#dcl.type.simple-4
    static_assert(std::is_same_v<decltype(std::move(unit).error()), NothrowMoveOnlyType&&>);
}

}  // namespace
}  // namespace score::details
