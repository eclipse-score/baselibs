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
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description", "Check that a Result can be set to an error state using the Unexpected type alias.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    Result<bool> result{Unexpected{error}};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(UnexpectedTests, CanMakeErroneousResultUsingMakeUnexpectedWithCodeAndUserMessage)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that MakeUnexpected(code, user_message) sets a Result to an error state carrying both.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    Result<bool> result{MakeUnexpected(DummyErrorCode::kFirstError, error.UserMessage())};
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), error);
}

TEST_F(UnexpectedTests, LegacyCanMakeErroneousResultUsingMakeUnexpectedWithError)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__set_result");
    RecordProperty("Description",
                   "Check that the legacy MakeUnexpected<T>(Error) overload sets a Result to an error state carrying "
                   "a pre-built Error.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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

TEST_F(ConversionTests, CanConvertLValueResultWithValueToStdOptional)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__std_integration");
    RecordProperty("Description",
                   "Check that ResultToOptionalOrElse converts an lvalue Result holding a value into an "
                   "std::optional holding that value, without invoking the error handler.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    CopyableType value{14};
    const Result<CopyableType> result{value};
    EXPECT_CALL(error_handling, Call(_)).Times(0);
    std::optional<CopyableType> optional = ResultToOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_TRUE(optional.has_value());
    EXPECT_EQ(optional.value(), value);
}

TEST_F(ConversionTests, CanConvertLValueResultWithErrorToStdOptional)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__std_integration");
    RecordProperty("Description",
                   "Check that ResultToOptionalOrElse converts an lvalue Result holding an error into an empty "
                   "std::optional, invoking the error handler exactly once.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    const Result<CopyableType> result{unexpect, error};
    EXPECT_CALL(error_handling, Call(error)).Times(1);
    std::optional<CopyableType> optional = ResultToOptionalOrElse(result, error_handling.AsStdFunction());
    EXPECT_FALSE(optional.has_value());
}

TEST_F(ConversionTests, CanConvertRValueResultWithValueToStdOptional)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__std_integration");
    RecordProperty("Description",
                   "Check that ResultToOptionalOrElse converts an rvalue Result holding a move-only value into an "
                   "std::optional holding that value.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__std_integration");
    RecordProperty("Description",
                   "Check that ResultToOptionalOrElse converts an rvalue Result holding an error into an empty "
                   "std::optional, invoking the error handler exactly once.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
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
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description",
                   "Check that IsResultV is true for a Result<T> instantiation with a concrete value type.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    EXPECT_TRUE(IsResultV<Result<bool>>);
}

TEST_F(TypeTraitsTests, IsResultVIsTrueIfIsResultBlank)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that IsResultV is true for the value-less Result<void> instantiation.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    EXPECT_TRUE(IsResultV<Result<void>>);
}

TEST_F(TypeTraitsTests, IsResultVIsFalseIfIsNoResult)
{
    RecordProperty("PartiallyVerifies", "comp_req__result__type_safety");
    RecordProperty("Description", "Check that IsResultV is false for a type that is not a Result instantiation.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    EXPECT_FALSE(IsResultV<bool>);
}

}  // namespace
}  // namespace score::result
