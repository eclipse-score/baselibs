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
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/test_types.h"

#include "score/expected.hpp"
#include "score/stop_token.hpp"

#include "gtest/gtest.h"

#include <future>

namespace score
{
namespace concurrency
{
namespace
{

template <typename T>
class InterruptiblePromiseTestBase : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto expected_future = promise_.GetInterruptibleFuture();
        ASSERT_TRUE(expected_future.has_value());
        future_ = std::move(expected_future.value());
    }

  protected:
    InterruptiblePromise<T> promise_{};
    InterruptibleFuture<T> future_{};
};

template <typename T>
class InterruptiblePromiseTest : public InterruptiblePromiseTestBase<T>
{
  protected:
    score::cpp::expected_blank<Error> SetPromise(InterruptiblePromise<T>& promise) const noexcept
    {
        return promise.SetValue(value_);
    }

    void ExpectCorrectValue(const score::Result<T>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().GetValue(), expected_value_);
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
class InterruptiblePromiseTest<T&> : public InterruptiblePromiseTestBase<T&>
{
  protected:
    score::cpp::expected_blank<Error> SetPromise(InterruptiblePromise<T&>& promise) noexcept
    {
        return promise.SetValue(value_);
    }

    void ExpectCorrectValue(const score::Result<std::reference_wrapper<T>>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().get().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <>
class InterruptiblePromiseTest<testing::MoveOnlyType> : public InterruptiblePromiseTestBase<testing::MoveOnlyType>
{
  protected:
    score::cpp::expected_blank<Error> SetPromise(InterruptiblePromise<testing::MoveOnlyType>& promise) noexcept
    {
        return promise.SetValue(std::move(value_));
    }

    void ExpectCorrectValue(const score::Result<testing::MoveOnlyType>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().GetValue(), expected_value_);
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
class InterruptiblePromiseTest<void> : public InterruptiblePromiseTestBase<void>
{
  protected:
    score::cpp::expected_blank<Error> SetPromise(InterruptiblePromise<void>& promise) const noexcept
    {
        return promise.SetValue();
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
TYPED_TEST_SUITE(InterruptiblePromiseTest, TypesUnderTest, /*unused*/);

TYPED_TEST(InterruptiblePromiseTest, CanDefaultConstruct)
{
    EXPECT_NO_FATAL_FAILURE(InterruptiblePromise<TypeParam> promise{});
}

TYPED_TEST(InterruptiblePromiseTest, CanNotCopyConstruct)
{
    EXPECT_FALSE(std::is_copy_constructible<InterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(InterruptiblePromiseTest, CanNotCopyAssign)
{
    EXPECT_FALSE(std::is_copy_assignable<InterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(InterruptiblePromiseTest, CanMoveConstruct)
{
    EXPECT_TRUE(std::is_move_constructible<InterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(InterruptiblePromiseTest, CanMoveAssign)
{
    EXPECT_TRUE(std::is_move_assignable<InterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(InterruptiblePromiseTest, Destruction)
{
    {
        detail::BaseInterruptiblePromise<TypeParam> stack_promise{};
        (void)stack_promise;
    }

    {
        InterruptiblePromise<TypeParam> stack_promise{};
        (void)stack_promise;
    }

    auto heap_base_promise = std::make_unique<detail::BaseInterruptiblePromise<TypeParam>>();
    heap_base_promise.reset();

    std::unique_ptr<detail::BaseInterruptiblePromise<TypeParam>> heap_base_derived_promise =
        std::make_unique<InterruptiblePromise<TypeParam>>();
    heap_base_derived_promise.reset();

    auto heap_promise = std::make_unique<InterruptiblePromise<TypeParam>>();
    heap_promise.reset();
}

TYPED_TEST(InterruptiblePromiseTest, MoveConstructingFromInvalidatesState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_from_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(InterruptiblePromiseTest, MoveAssigningFromInvalidatesState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_error = moved_from_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(InterruptiblePromiseTest, MoveConstructingToMovesState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromiseTest<TypeParam>::SetPromise(moved_from_promise);
    InterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_future = moved_to_promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());
    auto future = std::move(expected_future.value());

    score::cpp::stop_token stop_token{};
    auto expected_value = future.Get(stop_token);
    ASSERT_TRUE(expected_value.has_value());
    InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
}

TYPED_TEST(InterruptiblePromiseTest, MoveConstructingAlsoMovesFutureRetrievalState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.GetInterruptibleFuture();
    InterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_to_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(InterruptiblePromiseTest, MoveAssignmentToMovesState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromiseTest<TypeParam>::SetPromise(moved_from_promise);
    InterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_future = moved_to_promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());
    auto future = std::move(expected_future.value());

    score::cpp::stop_token stop_token{};
    auto expected_value = future.Get(stop_token);
    ASSERT_TRUE(expected_value.has_value());
    InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
}

TYPED_TEST(InterruptiblePromiseTest, MoveAssignmentAlsoMovesFutureRetrievalState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.GetInterruptibleFuture();
    InterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_error = moved_to_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(InterruptiblePromiseTest, DestructionBreaksPromise)
{
    InterruptibleFuture<TypeParam> future{};
    {
        InterruptiblePromise<TypeParam> promise{};

        auto expected_future = promise.GetInterruptibleFuture();
        ASSERT_TRUE(expected_future.has_value());
        future = std::move(expected_future.value());
    }

    score::cpp::stop_token stop_token{};
    auto expected_value = future.Get(stop_token);
    ASSERT_FALSE(expected_value.has_value());
    EXPECT_EQ(expected_value.error(), Error::kPromiseBroken);
}

TYPED_TEST(InterruptiblePromiseTest, CanSwap)
{
    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    InterruptiblePromise<TypeParam> other_promise{};

    this->promise_.Swap(other_promise);

    auto return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    ASSERT_TRUE(return_code.has_value());
    auto other_return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(other_promise);
    ASSERT_FALSE(other_return_code.has_value());
    ASSERT_EQ(other_return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(InterruptiblePromiseTest, CanSetValue)
{
    auto return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    ASSERT_TRUE(return_code.has_value());

    score::cpp::stop_token stop_token{};
    auto expected_actual_value = this->future_.Get(stop_token);
    InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_actual_value);
}

TYPED_TEST(InterruptiblePromiseTest, SettingValueTwiceReturnsPromiseAlreadySatisfiedError)
{
    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);

    auto return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    ASSERT_FALSE(return_code.has_value());
    EXPECT_EQ(return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(InterruptiblePromiseTest, SettingValueOnMovedFromPromiseFailesWithNoState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = InterruptiblePromiseTest<TypeParam>::SetPromise(moved_from_promise);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(InterruptiblePromiseTest, CanSetError)
{
    auto return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_TRUE(return_code.has_value());

    score::cpp::stop_token stop_token{};
    auto expected_error = this->future_.Get(stop_token);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(InterruptiblePromiseTest, SettingErrorTwiceReturnsPromiseAlreadySatisfiedError)
{
    this->promise_.SetError(Error::kFutureAlreadyRetrieved);

    auto return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(return_code.has_value());
    EXPECT_EQ(return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(InterruptiblePromiseTest, SettingErrorOnMovedFromPromiseFailesWithNoState)
{
    InterruptiblePromise<TypeParam> moved_from_promise{};
    InterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_from_promise.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(InterruptiblePromiseTest, SettingErrorAfterValueReturnsPromiseAlreadySatisfiedError)
{
    auto set_value_return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    ASSERT_TRUE(set_value_return_code.has_value());

    auto set_error_return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(set_error_return_code.has_value());
    EXPECT_EQ(set_error_return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(InterruptiblePromiseTest, SettingValueAfterErrorReturnsPromiseAlreadySatisfiedError)
{
    auto set_error_return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_TRUE(set_error_return_code.has_value());

    auto set_value_return_code = InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);
    ASSERT_FALSE(set_value_return_code.has_value());
    EXPECT_EQ(set_value_return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(InterruptiblePromiseTest, RetrievedFutureSharesStateWithPromise)
{
    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);

    score::cpp::stop_token stop_token{};
    auto expected_value = this->future_.Get(stop_token);
    InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
}

TYPED_TEST(InterruptiblePromiseTest, RetrievedFutureSharesStateWithPromiseInDifferentThreads)
{

    auto async_future = std::async([this]() noexcept {
        score::cpp::stop_token stop_token{};
        ASSERT_TRUE(this->future_.Valid());
        auto expected_value = this->future_.Get(stop_token);
        InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
        return;
    });

    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);

    async_future.wait();
}

TYPED_TEST(InterruptiblePromiseTest, RetrievedSharedFutureSharesStateWithPromiseInDifferentThreads)
{
    auto async_future = std::async([this]() noexcept {
        score::cpp::stop_token stop_token{};
        InterruptibleSharedFuture<TypeParam> shared_future = this->future_.Share();
        auto expected_value = shared_future.Get(stop_token);
        InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
        return;
    });

    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);

    async_future.wait();
}

TYPED_TEST(InterruptiblePromiseTest, RetrievedSharedFuturesSharesStateWithPromiseInDifferentThreads)
{
    InterruptibleSharedFuture<TypeParam> shared_future_1 = this->future_.Share();
    InterruptibleSharedFuture<TypeParam> shared_future_2{shared_future_1};

    auto async_future_1 = std::async([this, shared_future_1]() mutable noexcept {
        score::cpp::stop_token stop_token{};
        auto expected_value = shared_future_1.Get(stop_token);
        InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
        return;
    });

    auto async_future_2 = std::async([this, shared_future_2]() mutable noexcept {
        score::cpp::stop_token stop_token{};
        auto expected_value = shared_future_2.Get(stop_token);
        InterruptiblePromiseTest<TypeParam>::ExpectCorrectValue(expected_value);
        return;
    });

    InterruptiblePromiseTest<TypeParam>::SetPromise(this->promise_);

    async_future_1.wait();
    async_future_2.wait();
}

TYPED_TEST(InterruptiblePromiseTest, RetrievingFutureTwiceReturnsFutureAlreadyRetrievedError)
{
    InterruptiblePromise<TypeParam> promise{};
    auto expected_future = promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());

    auto expected_error = promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
