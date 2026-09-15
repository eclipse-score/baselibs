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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator-> on a const expected holding a value returns a const pointer to that "
                   "value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator-> on a const expected holding an error aborts instead of returning a "
                   "dangling pointer.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator->(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ArrowOperatorReturnsPointerToValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator-> on a mutable expected holding a value returns a mutable pointer to "
                   "that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator-> on a mutable expected holding an error aborts instead of returning a "
                   "dangling pointer.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator->(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorLValueConstReturnsReferenceToValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a const-lvalue expected holding a value returns a const reference to "
                   "that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a const-lvalue expected holding an error aborts instead of "
                   "returning a dangling reference.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorLValueReturnsReferenceToValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a mutable lvalue expected holding a value returns a mutable "
                   "reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a mutable lvalue expected holding an error aborts instead of "
                   "returning a dangling reference.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorRValueConstReturnsReferenceToValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a const-rvalue expected holding a value returns a const rvalue "
                   "reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a const-rvalue expected holding an error aborts instead of "
                   "returning a dangling reference.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = std::move(unit).operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, StarOperatorRValueReturnsReferenceToValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a mutable rvalue expected holding a value returns a mutable rvalue "
                   "reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a mutable rvalue expected holding an error aborts instead of "
                   "returning a dangling reference.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(std::ignore = std::move(unit).operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ExplicitConversionToBoolIsTrueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that explicit conversion to bool yields true for an expected holding a value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be true
    EXPECT_TRUE(value);
}

TEST(ExpectedTest, ExplicitConversionToBoolIsFalseIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that explicit conversion to bool yields false for an expected holding an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be false
    EXPECT_FALSE(value);
}

TEST(ExpectedTest, HasValueReturnsTrueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that has_value() returns true for an expected holding a value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Then expect has_value to return true
    EXPECT_TRUE(unit.has_value());
}

TEST(ExpectedTest, HasValueReturnsFalseIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that has_value() returns false for an expected holding an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Then expect it to be false
    EXPECT_FALSE(unit.has_value());
}

TEST(ExpectedTest, CanRetrieveValueFromLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on an lvalue expected holding a value returns a mutable reference to that "
                   "value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on an lvalue expected holding an error throws instead of returning a "
                   "dangling value.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromConstLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on a const-lvalue expected holding a value returns a const reference to "
                   "that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on a const-lvalue expected holding an error throws instead of returning a "
                   "dangling value.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromRValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on an rvalue expected holding a value returns a moved-out rvalue "
                   "reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on an rvalue expected holding an error throws instead of returning a "
                   "dangling value.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveValueFromConstRValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on a const-rvalue expected holding a value returns a const moved-out "
                   "rvalue reference to that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on a const-rvalue expected holding an error throws instead of returning "
                   "a dangling value.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<ValueType, ErrorType> unit{unexpect};

    // Expect an exception if accessing the value
    EXPECT_THROW(std::ignore = std::move(unit).value(), std::exception);
}

TEST(ExpectedTest, CanRetrieveErrorFromLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an lvalue expected holding an error returns a mutable reference to "
                   "that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an lvalue expected holding a value aborts instead of returning a "
                   "dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = unit.error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromConstLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-lvalue expected holding an error returns a const reference to "
                   "that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-lvalue expected holding a value aborts instead of returning a "
                   "dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    const expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromRValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an rvalue expected holding an error returns a moved-out rvalue "
                   "reference to that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an rvalue expected holding a value aborts instead of returning a "
                   "dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, CanRetrieveErrorFromConstRValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-rvalue expected holding an error returns a const moved-out "
                   "rvalue reference to that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-rvalue expected holding a value aborts instead of returning a "
                   "dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    const expected<ValueType, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedTest, ValueOrConstLValueReturnsValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value_or() on a const-lvalue expected holding a value returns that value instead "
                   "of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value_or() on a const-lvalue expected holding an error returns the supplied "
                   "default value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value_or() on an rvalue expected holding a value returns the moved-out value "
                   "instead of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value_or() on an rvalue expected holding an error returns the supplied default "
                   "value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on a const-lvalue expected holding an error returns that error "
                   "instead of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on a const-lvalue expected holding a value returns the supplied "
                   "default error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on an rvalue expected holding an error returns the moved-out error "
                   "instead of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on an rvalue expected holding a value returns the supplied default "
                   "error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that operator* on a void-valued expected that is valid does not abort or throw.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const expected<void, ErrorType> unit{};

    // Expect an abort if accessing the value
    EXPECT_NO_THROW(unit.operator*());
}

TEST(ExpectedVoidTest, StarOperatorWillExitIfInErrorState)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that operator* on a void-valued expected holding an error aborts.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<void, ErrorType> unit{unexpect};

    // Expect an abort if accessing the value
    EXPECT_EXIT(unit.operator*(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, ExplicitConversionToBoolIsTrueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that explicit conversion to bool yields true for a void-valued expected that is "
                   "valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    expected<void, ErrorType> unit{};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be true
    EXPECT_TRUE(value);
}

TEST(ExpectedVoidTest, ExplicitConversionToBoolIsFalseIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that explicit conversion to bool yields false for a void-valued expected holding an "
                   "error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // When converting to bool
    bool value = static_cast<bool>(unit);

    // Then expect it to be false
    EXPECT_FALSE(value);
}

TEST(ExpectedVoidTest, HasValueReturnsTrueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that has_value() returns true for a void-valued expected that is valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Then expect has_value to return true
    EXPECT_TRUE(unit.has_value());
}

TEST(ExpectedVoidTest, HasValueReturnsFalseIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that has_value() returns false for a void-valued expected holding an error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // Then expect it to be false
    EXPECT_FALSE(unit.has_value());
}

TEST(ExpectedVoidTest, NoAbortWhenCallValueFromConstLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that value() on a const-lvalue void-valued expected that is valid does not throw.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const expected<void, ErrorType> unit{};

    // Expect no exception when calling value
    EXPECT_NO_THROW(unit.value());
}

TEST(ExpectedVoidTest, AbortWhenCallValueFromConstLValueReferenceWithoutValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that value() on a const-lvalue void-valued expected holding an error throws.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    const expected<void, ErrorType> unit{unexpect};

    // Expect an exception when calling value value
    EXPECT_THROW(unit.value(), std::exception);
}

TEST(ExpectedVoidTest, NoAbortWhenCallValueFromRValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that value() on an rvalue void-valued expected that is valid does not throw.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    expected<void, ErrorType> unit{};

    // Expect no exception when calling value
    EXPECT_NO_THROW(std::move(unit).value());
}

TEST(ExpectedVoidTest, AbortWhenCallValueFromRValueReferenceWithoutValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description", "Check that value() on an rvalue void-valued expected holding an error throws.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with an error
    expected<void, ErrorType> unit{unexpect};

    // Expect an exception when calling value value
    EXPECT_THROW(std::move(unit).value(), std::exception);
}

TEST(ExpectedVoidTest, CanRetrieveErrorFromConstLValueReference)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-lvalue void-valued expected holding an error returns a const "
                   "reference to that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on a const-lvalue void-valued expected that is valid aborts instead of "
                   "returning a dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    const expected<void, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, AbortsWhenRetrieveErrorFromRValueReferenceWithValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an rvalue void-valued expected that is valid aborts instead of "
                   "returning a dangling error.");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    // Given an expected with a value
    expected<void, ErrorType> unit{};

    // Expect an abort if accessing the error
    EXPECT_EXIT(std::ignore = std::move(unit).error(), testing::KilledBySignal(SIGABRT), "");
}

TEST(ExpectedVoidTest, ErrorOrConstLValueReturnsErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on a const-lvalue void-valued expected holding an error returns that "
                   "error instead of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on a const-lvalue void-valued expected that is valid returns the "
                   "supplied default error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on an rvalue void-valued expected holding an error returns the "
                   "moved-out error instead of the default.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error_or() on an rvalue void-valued expected that is valid returns the supplied "
                   "default error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that error() on an rvalue void-valued expected holding an error returns a moved-out "
                   "rvalue reference to that error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
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
