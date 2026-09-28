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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score::details
{
namespace
{

TEST(ExpectedTest, AndThenLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an lvalue expected holding a value invokes the given function with "
                   "that value and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an lvalue expected holding an error skips the given function and "
                   "propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>(ValueType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, AndThenLValueConstRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-lvalue expected holding a value invokes the given function "
                   "with that value and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenLValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-lvalue expected holding an error skips the given function "
                   "and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>(const ValueType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, AndThenRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an rvalue expected holding a value invokes the given function with "
                   "the moved-out value and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an rvalue expected holding an error skips the given function and "
                   "propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>(ValueType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, AndThenRValueConstRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-rvalue expected holding a value invokes the given function "
                   "with that value and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedTest, AndThenRValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-rvalue expected holding an error skips the given function "
                   "and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>(const ValueType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, OrElseLValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an lvalue expected holding an error invokes the given function with "
                   "that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedTest, OrElseLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an lvalue expected holding a value skips the given function and "
                   "propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CopyableType, ErrorType>(ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, OrElseConstLValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-lvalue expected holding an error invokes the given function "
                   "with that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(const CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedTest, OrElseConstLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-lvalue expected holding a value skips the given function and "
                   "propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CopyableType, ErrorType>(const ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, OrElseRValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an rvalue expected holding an error invokes the given function with "
                   "the moved-out error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedTest, OrElseRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an rvalue expected holding a value skips the given function and "
                   "propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CopyableType, ErrorType>(ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, OrElseConstRValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-rvalue expected holding an error invokes the given function "
                   "with that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<ValueType, CopyableType>(const CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<ValueType, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedTest, OrElseConstRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-rvalue expected holding a value skips the given function and "
                   "propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<CopyableType, ErrorType>(const ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an lvalue expected holding a value invokes the given function with "
                   "that value and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an lvalue expected holding an error skips the given function and "
                   "propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, TransformConstLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-lvalue expected holding a value invokes the given function "
                   "with that value and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformConstLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-lvalue expected holding an error skips the given function "
                   "and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, TransformRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an rvalue expected holding a value invokes the given function with "
                   "the moved-out value and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an rvalue expected holding an error skips the given function and "
                   "propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, TransformConstRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-rvalue expected holding a value invokes the given function "
                   "with that value and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, ErrorType> unit{wrapped};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedTest, TransformConstRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-rvalue expected holding an error skips the given function "
                   "and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<CopyableType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedTest, TransformErrorLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an lvalue expected holding an error invokes the given "
                   "function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedTest, TransformErrorLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an lvalue expected holding a value skips the given function "
                   "and propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorConstLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-lvalue expected holding an error invokes the given "
                   "function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedTest, TransformErrorConstLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-lvalue expected holding a value skips the given "
                   "function and propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an rvalue expected holding an error invokes the given "
                   "function with the moved-out error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedTest, TransformErrorRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an rvalue expected holding a value skips the given function "
                   "and propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedTest, TransformErrorConstRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-rvalue expected holding an error invokes the given "
                   "function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<ValueType, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedTest, TransformErrorConstRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-rvalue expected holding a value skips the given "
                   "function and propagates the rebound value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const std::int32_t value{83};
    CopyableType wrapped{value};
    const expected<CopyableType, CopyableType> unit{wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, value);
}

TEST(ExpectedVoidTest, AndThenLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an lvalue void-valued expected that is valid invokes the given "
                   "function and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedVoidTest, AndThenLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an lvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, AndThenLValueConstRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-lvalue void-valued expected that is valid invokes the given "
                   "function and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedVoidTest, AndThenLValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-lvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling and_then
    const auto result = unit.and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, AndThenRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an rvalue void-valued expected that is valid invokes the given "
                   "function and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedVoidTest, AndThenRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on an rvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, AndThenRValueConstRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-rvalue void-valued expected that is valid invokes the given "
                   "function and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<expected<CopyableType, ErrorType>()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(expected<CopyableType, ErrorType>{monad_value}));

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->value_, monad_value);
}

TEST(ExpectedVoidTest, AndThenRValueConstRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that and_then() on a const-rvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator not to be called
    testing::MockFunction<expected<ValueType, CopyableType>()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling and_then
    const auto result = std::move(unit).and_then(monad.AsStdFunction());

    // Then the result is the rebound expected propagating the error
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, OrElseLValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an lvalue void-valued expected holding an error invokes the given "
                   "function with that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<void, CopyableType>(CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<void, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedVoidTest, OrElseLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an lvalue void-valued expected that is valid skips the given "
                   "function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<void, ErrorType>(ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, OrElseConstLValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-lvalue void-valued expected holding an error invokes the "
                   "given function with that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<void, CopyableType>(const CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(expected<void, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedVoidTest, OrElseConstLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-lvalue void-valued expected that is valid skips the given "
                   "function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<void, ErrorType>(const ErrorType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = unit.or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, OrElseRValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an rvalue void-valued expected holding an error invokes the given "
                   "function with the moved-out error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<void, CopyableType>(CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<void, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedVoidTest, OrElseRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on an rvalue void-valued expected that is valid skips the given function "
                   "and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<void, ErrorType>(ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, OrElseConstRValueRefWillCallFunctionIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-rvalue void-valued expected holding an error invokes the "
                   "given function with that error and returns its result.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<expected<void, CopyableType>(const CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(expected<void, CopyableType>{unexpect, monad_error}));

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, monad_error);
}

TEST(ExpectedVoidTest, OrElseConstRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that or_else() on a const-rvalue void-valued expected that is valid skips the given "
                   "function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<expected<void, ErrorType>(const ErrorType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling or_else
    const auto result = std::move(unit).or_else(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, TransformLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an lvalue void-valued expected that is valid invokes the given "
                   "function and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedVoidTest, TransformLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an lvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, TransformConstLValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-lvalue void-valued expected that is valid invokes the "
                   "given function and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedVoidTest, TransformConstLValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-lvalue void-valued expected holding an error skips the "
                   "given function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling transform
    const auto result = unit.transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, TransformRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an rvalue void-valued expected that is valid invokes the given "
                   "function and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedVoidTest, TransformRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on an rvalue void-valued expected holding an error skips the given "
                   "function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, TransformConstRValueRefWillCallFunctionIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-rvalue void-valued expected that is valid invokes the "
                   "given function and wraps the result in a rebound expected.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, ErrorType> unit{};

    // Expect the monadic operator to be called with the value
    testing::MockFunction<CompatibleCopyableType()> monad{};
    const std::int32_t monad_value{64};
    EXPECT_CALL(monad, Call()).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_value}}));

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->inner_.value_, monad_value);
}

TEST(ExpectedVoidTest, TransformConstRValueRefWillReturnReboundErrorIfHasNoValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform() on a const-rvalue void-valued expected holding an error skips the "
                   "given function and propagates the rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType()> monad{};
    EXPECT_CALL(monad, Call()).Times(0);

    // When calling transform
    const auto result = std::move(unit).transform(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value_, error);
}

TEST(ExpectedVoidTest, TransformErrorLValueRefWillCallFunctionIfHasError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an lvalue void-valued expected holding an error invokes the "
                   "given function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedVoidTest, TransformErrorLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an lvalue void-valued expected that is valid skips the "
                   "given function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    expected<void, CopyableType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, TransformErrorConstLValueRefWillCallFunctionIfHasError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-lvalue void-valued expected holding an error invokes "
                   "the given function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(wrapped)).WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedVoidTest, TransformErrorConstLValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-lvalue void-valued expected that is valid skips the "
                   "given function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, CopyableType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = unit.transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, TransformErrorRValueRefWillCallFunctionIfHasError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an rvalue void-valued expected holding an error invokes the "
                   "given function with the moved-out error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedVoidTest, TransformErrorRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on an rvalue void-valued expected that is valid skips the given "
                   "function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with a value
    expected<void, CopyableType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

TEST(ExpectedVoidTest, TransformErrorConstRValueRefWillCallFunctionIfHasError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-rvalue void-valued expected holding an error invokes "
                   "the given function with that error and wraps the result in a rebound error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given an expected with an error
    const std::int32_t error{83};
    CopyableType wrapped{error};
    const expected<void, CopyableType> unit{unexpect, wrapped};

    // Expect the monadic operator to be called with the error
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    const std::int32_t monad_error{64};
    EXPECT_CALL(monad, Call(std::move(wrapped)))
        .WillOnce(::testing::Return(CompatibleCopyableType{CopyableType{monad_error}}));

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().inner_.value_, monad_error);
}

TEST(ExpectedVoidTest, TransformErrorConstRValueRefWillReturnReboundValueIfHasValue)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__error_handling");
    RecordProperty("Description",
                   "Check that transform_error() on a const-rvalue void-valued expected that is valid skips the "
                   "given function and leaves the expected valid.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    // Given a valid expected
    const expected<void, CopyableType> unit{};

    // Expect the monadic operator to not be called
    testing::MockFunction<CompatibleCopyableType(const CopyableType&&)> monad{};
    EXPECT_CALL(monad, Call(testing::_)).Times(0);

    // When calling transform_error
    const auto result = std::move(unit).transform_error(monad.AsStdFunction());

    // Then the result is the rebound expected
    ASSERT_TRUE(result.has_value());
}

}  // namespace
}  // namespace score::details
