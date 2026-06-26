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
#include "score/result/details/expected/extensions.h"

#include "score/result/details/expected/expected.h"
#include "score/result/details/expected/test_types.h"

#include <score/expected.hpp>

#include <include/gmock/gmock.h>
#include <include/gtest/gtest.h>

namespace score::details
{
namespace
{

TEST(ExtensionsTest, CanConvertFromExpectedConstLValueRefToAmpExpectedWithValue)
{
    // Given a score::details::expected with a value
    const std::int32_t value{57};
    const score::details::expected<CopyableType, ErrorType> expected{value};

    // When converting it to a score::cpp::expected
    const score::cpp::expected<CopyableType, ErrorType> score_future_cpp_expected{
        to_score_future_cpp_expected(expected)};

    // Then the score::cpp::expected holds the value
    ASSERT_TRUE(score_future_cpp_expected.has_value());
    EXPECT_EQ(score_future_cpp_expected->value_, value);
}

TEST(ExtensionsTest, CanConvertFromExpectedConstLValueRefToAmpExpectedWithError)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    const score::details::expected<ValueType, CopyableType> expected{unexpect, value};

    // When converting it to a score::cpp::expected
    const score::cpp::expected<ValueType, CopyableType> score_future_cpp_expected{
        to_score_future_cpp_expected(expected)};

    // Then the score::cpp::expected holds the error
    ASSERT_FALSE(score_future_cpp_expected.has_value());
    EXPECT_EQ(score_future_cpp_expected.error().value_, value);
}

TEST(ExtensionsTest, CanConvertFromExpectedRValueRefToAmpExpectedWithValue)
{
    // Given a score::details::expected with a value
    const std::int32_t value{57};
    score::details::expected<NothrowMoveOnlyType, ErrorType> expected{value};

    // When converting it to a score::cpp::expected
    const score::cpp::expected<NothrowMoveOnlyType, ErrorType> score_future_cpp_expected{
        to_score_future_cpp_expected(std::move(expected))};

    // Then the score::cpp::expected holds the value
    ASSERT_TRUE(score_future_cpp_expected.has_value());
    EXPECT_EQ(score_future_cpp_expected->value_, value);
}

TEST(ExtensionsTest, CanConvertFromExpectedRValueRefToAmpExpectedWithError)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    score::details::expected<ValueType, NothrowMoveOnlyType> expected{unexpect, value};

    // When converting it to a score::cpp::expected
    const score::cpp::expected<ValueType, NothrowMoveOnlyType> score_future_cpp_expected{
        to_score_future_cpp_expected(std::move(expected))};

    // Then the score::cpp::expected holds the error
    ASSERT_FALSE(score_future_cpp_expected.has_value());
    EXPECT_EQ(score_future_cpp_expected.error().value_, value);
}

TEST(ExtensionsTest, CanConvertFromAmpExpectedConstLValueRefToExpectedWithValue)
{
    // Given an score::cpp::expected with a value
    const std::int32_t value{57};
    const score::cpp::expected<CopyableType, ErrorType> score_future_cpp_expected{value};

    // When converting it to a score::details::expected
    const score::details::expected<CopyableType, ErrorType> expected{to_score_expected(score_future_cpp_expected)};

    // Then the score::expected holds the value
    ASSERT_TRUE(expected.has_value());
    EXPECT_EQ(expected->value_, value);
}

TEST(ExtensionsTest, CanConvertFromAmpExpectedConstLValueRefToExpectedWithError)
{
    // Given an score::cpp::expected with an error
    const std::int32_t value{57};
    const score::cpp::expected<ValueType, CopyableType> score_future_cpp_expected{
        score::cpp::make_unexpected(CopyableType{value})};

    // When converting it to a score::details::expected
    const score::details::expected<ValueType, CopyableType> expected{to_score_expected(score_future_cpp_expected)};

    // Then the score::expected holds the error
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error().value_, value);
}

TEST(ExtensionsTest, CanConvertFromAmpExpectedRValueRefToExpectedWithValue)
{
    // Given an score::cpp::expected with a value
    const std::int32_t value{57};
    score::cpp::expected<NothrowMoveOnlyType, ErrorType> score_future_cpp_expected{value};

    // When converting it to a score::details::expected
    const score::details::expected<NothrowMoveOnlyType, ErrorType> expected{
        to_score_expected(std::move(score_future_cpp_expected))};

    // Then the score::expected holds the value
    ASSERT_TRUE(expected.has_value());
    EXPECT_EQ(expected->value_, value);
}

TEST(ExtensionsTest, CanConvertFromAmpExpectedRValueRefToExpectedWithError)
{
    // Given an score::cpp::expected with an error
    const std::int32_t value{57};
    score::cpp::expected<ValueType, NothrowMoveOnlyType> score_future_cpp_expected{
        score::cpp::make_unexpected(NothrowMoveOnlyType{value})};

    // When converting it to a score::details::expected
    const score::details::expected<ValueType, NothrowMoveOnlyType> expected{
        to_score_expected(std::move(score_future_cpp_expected))};

    // Then the score::expected holds the error
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error().value_, value);
}

TEST(ExtensionsTest, ConvertToAmpOptionalWhenExpectedConstLValueRefHasValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    const score::details::expected<CopyableType, ErrorType> expected{value};

    // Expect the operator to not be called
    testing::MockFunction<void(const ErrorType&)> on_error{};
    EXPECT_CALL(on_error, Call(testing::_)).Times(0);

    // When converting it to an optional
    const score::cpp::optional<CopyableType> optional{
        expected_value_to_score_future_cpp_optional_or_else(expected, on_error.AsStdFunction())};

    // Then the optional holds the value
    ASSERT_TRUE(optional.has_value());
    EXPECT_EQ(optional->value_, value);
}

TEST(ExtensionsTest, OnConversionToAmpOptionalCallInvocableWhenExpectedConstLValueRefHasNoValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    const score::details::expected<ValueType, CopyableType> expected{unexpect, value};

    // Expect the operator to not be called
    testing::MockFunction<void(const CopyableType&)> on_error{};
    EXPECT_CALL(on_error, Call(CopyableType{value})).Times(1);

    // When converting it to an optional
    const score::cpp::optional<ValueType> optional{
        expected_value_to_score_future_cpp_optional_or_else(expected, on_error.AsStdFunction())};

    // Then the optional holds no value
    ASSERT_FALSE(optional.has_value());
}

TEST(ExtensionsTest, ConvertToAmpOptionalWhenExpectedRValueRefHasValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    score::details::expected<NothrowMoveOnlyType, ErrorType> expected{value};

    // Expect the operator to not be called
    testing::MockFunction<void(ErrorType&&)> on_error{};
    EXPECT_CALL(on_error, Call(testing::_)).Times(0);

    // When converting it to an optional
    const score::cpp::optional<NothrowMoveOnlyType> optional{
        expected_value_to_score_future_cpp_optional_or_else(std::move(expected), on_error.AsStdFunction())};

    // Then the optional holds the value
    ASSERT_TRUE(optional.has_value());
    EXPECT_EQ(optional->value_, value);
}

TEST(ExtensionsTest, OnConversionToAmpOptionalCallInvocableWhenExpectedLValueRefHasNoValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    score::details::expected<ValueType, NothrowMoveOnlyType> expected{unexpect, value};

    // Expect the operator to not be called
    testing::MockFunction<void(NothrowMoveOnlyType&&)> on_error{};
    EXPECT_CALL(on_error, Call(::testing::_)).WillOnce([value](NothrowMoveOnlyType&& error) {
        EXPECT_EQ(error.value_, value);
    });

    // When converting it to an optional
    const score::cpp::optional<ValueType> optional{
        expected_value_to_score_future_cpp_optional_or_else(std::move(expected), on_error.AsStdFunction())};

    // Then the optional holds no value
    ASSERT_FALSE(optional.has_value());
}

TEST(ExtensionsTest, ConvertToStdOptionalWhenExpectedConstLValueRefHasValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    const score::details::expected<CopyableType, ErrorType> expected{value};

    // Expect the operator to not be called
    testing::MockFunction<void(const ErrorType&)> on_error{};
    EXPECT_CALL(on_error, Call(testing::_)).Times(0);

    // When converting it to an optional
    const std::optional<CopyableType> optional{expected_value_to_optional_or_else(expected, on_error.AsStdFunction())};

    // Then the optional holds the value
    ASSERT_TRUE(optional.has_value());
    EXPECT_EQ(optional->value_, value);
}

TEST(ExtensionsTest, OnConversionToStdOptionalCallInvocableWhenExpectedConstLValueRefHasNoValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    const score::details::expected<ValueType, CopyableType> expected{unexpect, value};

    // Expect the operator to not be called
    testing::MockFunction<void(const CopyableType&)> on_error{};
    EXPECT_CALL(on_error, Call(CopyableType{value})).Times(1);

    // When converting it to an optional
    const std::optional<ValueType> optional{expected_value_to_optional_or_else(expected, on_error.AsStdFunction())};

    // Then the optional holds no value
    ASSERT_FALSE(optional.has_value());
}

TEST(ExtensionsTest, ConvertToStdOptionalWhenExpectedRValueRefHasValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    score::details::expected<NothrowMoveOnlyType, ErrorType> expected{value};

    // Expect the operator to not be called
    testing::MockFunction<void(ErrorType&&)> on_error{};
    EXPECT_CALL(on_error, Call(testing::_)).Times(0);

    // When converting it to an optional
    const std::optional<NothrowMoveOnlyType> optional{
        expected_value_to_optional_or_else(std::move(expected), on_error.AsStdFunction())};

    // Then the optional holds the value
    ASSERT_TRUE(optional.has_value());
    EXPECT_EQ(optional->value_, value);
}

TEST(ExtensionsTest, OnConversionToStdOptionalCallInvocableWhenExpectedLValueRefHasNoValue)
{
    // Given a score::details::expected with an error
    const std::int32_t value{57};
    score::details::expected<ValueType, NothrowMoveOnlyType> expected{unexpect, value};

    // Expect the operator to not be called
    testing::MockFunction<void(NothrowMoveOnlyType&&)> on_error{};
    EXPECT_CALL(on_error, Call(::testing::_)).WillOnce([value](NothrowMoveOnlyType&& error) {
        EXPECT_EQ(error.value_, value);
    });

    // When converting it to an optional
    const std::optional<ValueType> optional{
        expected_value_to_optional_or_else(std::move(expected), on_error.AsStdFunction())};

    // Then the optional holds no value
    ASSERT_FALSE(optional.has_value());
}

}  // namespace
}  // namespace score::details
