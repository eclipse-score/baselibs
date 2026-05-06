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
#include "score/concurrency/future/interruptible_shared_future.h"
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/future/test_types.h"

#include "score/stop_token.hpp"

#include "gtest/gtest.h"

#include <future>

namespace score
{
namespace concurrency
{
namespace
{

template <typename Value>
class TestOnlyBaseInterruptibleFuture final : public detail::BaseInterruptibleFuture<Value>
{
};

template <typename T>
class InterruptibleSharedFutureTestBase : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto future_expected = promise_.GetInterruptibleFuture();
        ASSERT_TRUE(future_expected.has_value());
        auto future = std::move(future_expected.value());
        this->future_ = future.Share();
    }

    InterruptiblePromise<T> promise_{};
    InterruptibleSharedFuture<T> future_{};
};

template <typename T>
class InterruptibleSharedFutureTest : public InterruptibleSharedFutureTestBase<T>
{
  protected:
    void SetPromise(InterruptiblePromise<T>& promise) const noexcept
    {
        promise.SetValue(value_);
    }

    void ExpectCorrectValue(const score::Result<std::reference_wrapper<const T>>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().get().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <typename T>
class InterruptibleSharedFutureTest<T&> : public InterruptibleSharedFutureTestBase<T&>
{
  protected:
    void SetPromise(InterruptiblePromise<T&>& promise) noexcept
    {
        promise.SetValue(value_);
    }

    void ExpectCorrectValue(const score::Result<std::reference_wrapper<T>>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().get().GetValue(), value_.GetValue());
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <>
class InterruptibleSharedFutureTest<testing::MoveOnlyType>
    : public InterruptibleSharedFutureTestBase<testing::MoveOnlyType>
{
  protected:
    void SetPromise(InterruptiblePromise<testing::MoveOnlyType>& promise) noexcept
    {
        promise.SetValue(std::move(value_));
    }

    void ExpectCorrectValue(
        const score::Result<std::reference_wrapper<const testing::MoveOnlyType>>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().get().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    testing::MoveOnlyType value_{expected_value_};
};

template <>
class InterruptibleSharedFutureTest<void> : public InterruptibleSharedFutureTestBase<void>
{
  protected:
    void SetPromise(InterruptiblePromise<void>& promise) const noexcept
    {
        promise.SetValue();
    }

    void ExpectCorrectValue(const score::Result<void>& actual_value_expected) const noexcept
    {
        EXPECT_TRUE(actual_value_expected.has_value());
    }
};

using TypesUnderTest = ::testing::Types<testing::CopyAndMovableType,
                                        testing::CopyAndMovableType&,
                                        testing::CopyOnlyType,
                                        testing::CopyOnlyType&,
                                        testing::MoveOnlyType,
                                        testing::MoveOnlyType&,
                                        void>;
TYPED_TEST_SUITE(InterruptibleSharedFutureTest, TypesUnderTest, /*unused*/);

TEST(AMP, CanUseCopyOnlyType)
{
    testing::CopyOnlyType value{1};
    score::cpp::expected<testing::CopyOnlyType, int> test_expected{value};
    test_expected = value;
}

TEST(AMP, CanCopyExpectedWithCopyOnlyType)
{
    testing::CopyOnlyType value{1};
    score::cpp::expected<testing::CopyOnlyType, int> test_expected{value};
    test_expected = value;
    score::cpp::expected<testing::CopyOnlyType, int> copy{test_expected};
}

TEST(AMP, CanMoveExpectedWithCopyOnlyType)
{
    testing::CopyOnlyType value{1};
    score::cpp::expected<testing::CopyOnlyType, int> test_expected{value};
    test_expected = value;
    score::cpp::expected<testing::CopyOnlyType, int> copy{std::move(test_expected)};
}

TYPED_TEST(InterruptibleSharedFutureTest, CanDefaultConstructInterruptibleSharedFuture)
{
    EXPECT_NO_FATAL_FAILURE(InterruptibleSharedFuture<TypeParam> future{});
}

TYPED_TEST(InterruptibleSharedFutureTest, CanCopyConstructInterruptibleSharedFuture)
{
    EXPECT_TRUE(std::is_copy_constructible<InterruptibleSharedFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleSharedFutureTest, CanCopyAssignInterruptibleSharedFuture)
{
    EXPECT_TRUE(std::is_copy_assignable<InterruptibleSharedFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleSharedFutureTest, CanMoveConstructInterruptibleSharedFuture)
{
    EXPECT_TRUE(std::is_move_constructible<InterruptibleSharedFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleSharedFutureTest, CanMoveAssignInterruptibleSharedFuture)
{
    EXPECT_TRUE(std::is_move_assignable<InterruptibleSharedFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleSharedFutureTest, DefaultConstructedInterruptibleSharedFutureIsInvalid)
{
    InterruptibleSharedFuture<TypeParam> future{};
    EXPECT_FALSE(future.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, Destruction)
{
    {
        InterruptibleSharedFuture<TypeParam> stack_future{};
        (void)stack_future;
    }

    auto heap_future = std::make_unique<InterruptibleSharedFuture<TypeParam>>();
    heap_future.reset();

    std::unique_ptr<detail::BaseInterruptibleFuture<TypeParam>> heap_base_future =
        std::make_unique<TestOnlyBaseInterruptibleFuture<TypeParam>>();
    heap_base_future.reset();
}

TYPED_TEST(InterruptibleSharedFutureTest, MovedFromFutureHasNoStateAfterMoveConstruction)
{
    InterruptibleSharedFuture<TypeParam> moved_to_future{std::move(this->future_)};
    EXPECT_FALSE(this->future_.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, MovedFromFutureHasNoStateAfterMoveAssignment)
{
    InterruptibleSharedFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_);
    EXPECT_FALSE(this->future_.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, MovedToFutureHasStateAfterMoveConstruction)
{
    InterruptibleSharedFuture<TypeParam> moved_to_future{std::move(this->future_)};
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, MovedToFutureHasStateAfterMoveAssignment)
{
    InterruptibleSharedFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_);
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, CopiedFromFutureHasStateAfterCopyConstruction)
{
    InterruptibleSharedFuture<TypeParam> copied_to_future{this->future_};
    EXPECT_TRUE(this->future_.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, CopiedFromFutureHasStateAfterCopyAssignment)
{
    InterruptibleSharedFuture<TypeParam> copied_to_future{};
    copied_to_future = this->future_;
    EXPECT_TRUE(this->future_.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, CopiedToFutureHasStateAfterCopyConstruction)
{
    InterruptibleSharedFuture<TypeParam> copied_to_future{this->future_};
    EXPECT_TRUE(copied_to_future.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, CopiedToFutureHasStateAfterCopyAssignment)
{
    InterruptibleSharedFuture<TypeParam> copied_to_future{};
    copied_to_future = this->future_;
    EXPECT_TRUE(copied_to_future.Valid());
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsNoStateErrorForInterruptibleSharedFutureWithoutState)
{
    score::cpp::stop_token stop_token{};
    InterruptibleSharedFuture<TypeParam> future{};
    auto expected = future.Get(stop_token);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kNoState);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsBrokenPromiseErrorForInterruptibleSharedFutureWithBrokenPromise)
{
    score::cpp::stop_token stop_token{};
    InterruptibleSharedFuture<TypeParam> future{};
    {
        InterruptiblePromise<TypeParam> promise{};
        auto expected_future = promise.GetInterruptibleFuture();
        future = expected_future.value().Share();
    }
    auto expected = future.Get(stop_token);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kPromiseBroken);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsStopRequestedErrorWhenStopRequestedAndNotReadyOrBroken)
{
    score::cpp::stop_source stop_source{};
    stop_source.request_stop();
    auto expected = this->future_.Get(stop_source.get_token());
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kStopRequested);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsPromiseBrokenErrorWhenStopRequestedButPromiseBroken)
{
    score::cpp::stop_source stop_source{};
    stop_source.request_stop();
    InterruptibleSharedFuture<TypeParam> future{};
    {
        InterruptiblePromise<TypeParam> promise{};
        auto expected_future = promise.GetInterruptibleFuture();
        future = expected_future.value().Share();
    }
    auto expected = future.Get(stop_source.get_token());
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kPromiseBroken);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsValueWhenStopRequestedButPromiseReady)
{
    score::cpp::stop_source stop_source{};
    stop_source.request_stop();
    InterruptibleSharedFutureTest<TypeParam>::SetPromise(this->promise_);
    auto expected = this->future_.Get(stop_source.get_token());
    EXPECT_TRUE(expected.has_value());
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsValueMultipleTimes)
{
    score::cpp::stop_token stop_token{};
    InterruptibleSharedFutureTest<TypeParam>::SetPromise(this->promise_);
    auto expected_1 = this->future_.Get(stop_token);
    ASSERT_TRUE(expected_1.has_value());
    InterruptibleSharedFutureTest<TypeParam>::ExpectCorrectValue(expected_1);

    auto expected_2 = this->future_.Get(stop_token);
    ASSERT_TRUE(expected_2.has_value());
    InterruptibleSharedFutureTest<TypeParam>::ExpectCorrectValue(expected_2);
}

TYPED_TEST(InterruptibleSharedFutureTest, MultipleSharedFuturesCanGetValue)
{
    score::cpp::stop_token stop_token{};
    InterruptibleSharedFutureTest<TypeParam>::SetPromise(this->promise_);
    auto future_2 = this->future_;
    auto expected_1 = this->future_.Get(stop_token);
    ASSERT_TRUE(expected_1.has_value());
    InterruptibleSharedFutureTest<TypeParam>::ExpectCorrectValue(expected_1);

    auto expected_2 = future_2.Get(stop_token);
    ASSERT_TRUE(expected_2.has_value());
    InterruptibleSharedFutureTest<TypeParam>::ExpectCorrectValue(expected_2);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsOnlyAfterValueWasSet)
{
    score::cpp::stop_token stop_token{};

    auto async_future = std::async([this, &stop_token]() noexcept {
        return this->future_.Get(stop_token);
    });

    InterruptibleSharedFutureTest<TypeParam>::SetPromise(this->promise_);

    auto expected = std::move(async_future.get());
    InterruptibleSharedFutureTest<TypeParam>::ExpectCorrectValue(expected);
}

TYPED_TEST(InterruptibleSharedFutureTest, GetReturnsErrorWhenSet)
{
    this->promise_.SetError(Error::kFutureAlreadyRetrieved);

    score::cpp::stop_token stop_token{};
    auto expected = std::move(this->future_.Get(stop_token));
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kFutureAlreadyRetrieved);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
