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
#include "score/result/result.h"
#include "score/result/dummy_error_code.h"
#include "score/result/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace score::result
{
namespace
{

using ::testing::_;

Error MakeError(DummyErrorCode code, const std::string_view user_message = "") noexcept
{
    return Error{static_cast<ErrorCode>(code), dummy_error_domain, user_message};
}

constexpr Error error{static_cast<ErrorCode>(DummyErrorCode::kFirstError), dummy_error_domain, "Some User Message"};

class UnexpectedTests : public ::testing::Test
{
  public:
};

TEST_F(UnexpectedTests, CanMakeErroneousResultUsingUnexpectedTypeAlias)
{
    Result<bool> result{Unexpected{error}};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(UnexpectedTests, CanMakeErroneousResultUsingMakeUnexpectedWithCodeAndUserMessage)
{
    Result<bool> result{MakeUnexpected(DummyErrorCode::kFirstError, error.UserMessage())};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(UnexpectedTests, LegacyCanMakeErroneousResultUsingMakeUnexpectedWithError)
{
    Result<bool> result{MakeUnexpected<bool>(error)};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

class ConversionTests : public ::testing::Test
{
  public:
    testing::MockFunction<void(Error)> error_handling{};
};

class CopyableType
{
  public:
    explicit CopyableType(std::int32_t value) noexcept : value_{value} {}

    bool operator==(const CopyableType& other) const noexcept
    {
        return value_ == other.value_;
    }

    std::int32_t value_;
};

class MoveOnlyType
{
  public:
    explicit MoveOnlyType(std::int32_t value) noexcept : value_{value} {}

    MoveOnlyType(const MoveOnlyType&) = delete;
    MoveOnlyType(MoveOnlyType&&) noexcept = default;
    MoveOnlyType& operator=(const MoveOnlyType&) = delete;
    MoveOnlyType& operator=(MoveOnlyType&&) noexcept = default;

    bool operator==(const MoveOnlyType& other) const noexcept
    {
        return value_ == other.value_;
    }

    std::int32_t value_;
};

TEST_F(ConversionTests, CanConvertLValueResultWithValueToAmpOptional)
{
    CopyableType value{32};
    const Result<CopyableType> result{value};
    EXPECT_CALL(error_handling, Call(_)).Times(0);
    score::cpp::optional<CopyableType> optional = ResultToAmpOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_TRUE(optional.has_value());
    EXPECT_EQ(optional.value(), value);
}

TEST_F(ConversionTests, CanConvertLValueResultWithErrorToAmpOptional)
{
    const Result<CopyableType> result{unexpect, error};
    EXPECT_CALL(error_handling, Call(error)).Times(1);
    score::cpp::optional<CopyableType> optional = ResultToAmpOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_FALSE(optional.has_value());
}

TEST_F(ConversionTests, CanConvertRValueResultWithValueToAmpOptional)
{
    const auto raw_value{53};
    MoveOnlyType value{raw_value};
    Result<MoveOnlyType> result{std::move(value)};
    EXPECT_CALL(error_handling, Call(_)).Times(0);
    score::cpp::optional<MoveOnlyType> optional = ResultToAmpOptionalOrElse(std::move(result), error_handling.AsStdFunction());
    EXPECT_TRUE(optional.has_value());
    EXPECT_EQ(optional.value().value_, raw_value);
}

TEST_F(ConversionTests, CanConvertRValueResultWithErrorToAmpOptional)
{
    Result<MoveOnlyType> result{unexpect, error};
    EXPECT_CALL(error_handling, Call(error)).Times(1);
    score::cpp::optional<MoveOnlyType> optional = ResultToAmpOptionalOrElse(std::move(result), error_handling.AsStdFunction());
    EXPECT_FALSE(optional.has_value());
}

TEST_F(ConversionTests, CanConvertLValueResultWithValueToStdOptional)
{
    CopyableType value{14};
    const Result<CopyableType> result{value};
    EXPECT_CALL(error_handling, Call(_)).Times(0);
    std::optional<CopyableType> optional = ResultToOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_TRUE(optional.has_value());
    EXPECT_EQ(optional.value(), value);
}

TEST_F(ConversionTests, CanConvertLValueResultWithErrorToStdOptional)
{
    const Result<CopyableType> result{unexpect, error};
    EXPECT_CALL(error_handling, Call(error)).Times(1);
    std::optional<CopyableType> optional = ResultToOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_FALSE(optional.has_value());
}

TEST_F(ConversionTests, CanConvertRValueResultWithValueToStdOptional)
{
    const auto raw_value{96};
    MoveOnlyType value{raw_value};
    Result<MoveOnlyType> result{std::move(value)};
    EXPECT_CALL(error_handling, Call(_)).Times(0);
    std::optional<MoveOnlyType> optional = ResultToOptionalOrElse(std::move(result), error_handling.AsStdFunction());
    EXPECT_TRUE(optional.has_value());
    EXPECT_EQ(optional.value().value_, raw_value);
}

TEST_F(ConversionTests, CanConvertRValueResultWithErrorToStdOptional)
{
    Result<MoveOnlyType> result{unexpect, error};
    EXPECT_CALL(error_handling, Call(error)).Times(1);
    std::optional<MoveOnlyType> optional = ResultToOptionalOrElse(std::move(result), error_handling.AsStdFunction());
    EXPECT_FALSE(optional.has_value());
}

class TypeTraitsTests : public ::testing::Test
{
};

TEST_F(TypeTraitsTests, IsResultVIsTrueIfIsTemplatedResult)
{
    EXPECT_TRUE(IsResultV<Result<bool>>);
}

TEST_F(TypeTraitsTests, IsResultVIsTrueIfIsResultBlank)
{
    EXPECT_TRUE(IsResultV<ResultBlank>);
}

TEST_F(TypeTraitsTests, IsResultVIsFalseIfIsNoResult)
{
    EXPECT_FALSE(IsResultV<bool>);
}

}  // namespace
}  // namespace score::result
