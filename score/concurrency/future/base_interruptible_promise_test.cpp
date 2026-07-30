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
#include "score/concurrency/future/base_interruptible_promise.h"
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
namespace detail
{
namespace
{

template <typename T>
class BaseInterruptiblePromiseTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto expected_future = promise_.GetInterruptibleFuture();
        ASSERT_TRUE(expected_future.has_value());
        future_ = std::move(expected_future.value());
    }

  protected:
    BaseInterruptiblePromise<T> promise_{};
    InterruptibleFuture<T> future_{};
};

using TypesUnderTest = ::testing::Types<testing::CopyAndMovableType,
                                        testing::CopyAndMovableType&,
                                        testing::CopyOnlyType,
                                        testing::CopyOnlyType&,
                                        testing::MoveOnlyType,
                                        testing::MoveOnlyType&,
                                        void>;
TYPED_TEST_SUITE(BaseInterruptiblePromiseTest, TypesUnderTest, /*unused*/);

TYPED_TEST(BaseInterruptiblePromiseTest, CanDefaultConstruct)
{
    EXPECT_NO_FATAL_FAILURE(BaseInterruptiblePromise<TypeParam> promise{});
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanNotCopyConstruct)
{
    EXPECT_FALSE(std::is_copy_constructible<BaseInterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanNotCopyAssign)
{
    EXPECT_FALSE(std::is_copy_assignable<BaseInterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanMoveConstruct)
{
    EXPECT_TRUE(std::is_move_constructible<BaseInterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanMoveAssign)
{
    EXPECT_TRUE(std::is_move_assignable<BaseInterruptiblePromise<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptiblePromiseTest, Destruction)
{

    auto heap_base_promise = std::make_unique<detail::BaseInterruptiblePromise<TypeParam>>();
    heap_base_promise.reset();

    {
        BaseInterruptiblePromise<TypeParam> stack_promise{};
        (void)stack_promise;
    }

    std::shared_ptr<BaseInterruptiblePromise<TypeParam>> shared_heap_base_promise;
    {
        shared_heap_base_promise = std::make_shared<BaseInterruptiblePromise<TypeParam>>();
    }
    shared_heap_base_promise.reset();
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveConstructingFromInvalidatesState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    BaseInterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_from_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssigningFromInvalidatesState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_error = moved_from_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveConstructingToMovesState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.SetError(Error::kPromiseAlreadySatisfied);
    BaseInterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_future = moved_to_promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());
    auto future = std::move(expected_future.value());

    score::cpp::stop_token stop_token{};
    auto expected_error = future.Get(stop_token);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveConstructingAlsoMovesFutureRetrievalState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.GetInterruptibleFuture();
    BaseInterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_to_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssignmentToMovesState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.SetError(Error::kPromiseAlreadySatisfied);
    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_future = moved_to_promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());
    auto future = std::move(expected_future.value());

    score::cpp::stop_token stop_token{};
    auto expected_error = future.Get(stop_token);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssignmentAlsoMovesFutureRetrievalState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.GetInterruptibleFuture();
    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_error = moved_to_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

// Force decision coverage on move operator (see also MoveAssignmentForceInvalidState):
TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssignmentToSelf)
{
    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    auto* promise_ptr = &moved_to_promise;
    moved_to_promise = std::move(*promise_ptr);
    ASSERT_TRUE(moved_to_promise.GetInterruptibleFuture().has_value());
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssignmentForceInvalidState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);
    // At this point moved_from_promise has been moved. To force the condition
    // on `operator=` where `HasState() == false`, we will move assign to it:
    BaseInterruptiblePromise<TypeParam> third_promise{};
    third_promise.GetInterruptibleFuture();
    moved_from_promise = std::move(third_promise);
    ASSERT_FALSE(moved_from_promise.GetInterruptibleFuture().has_value());
}

TYPED_TEST(BaseInterruptiblePromiseTest, MoveAssignmentWhenStateIsInvalid)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    moved_from_promise.GetInterruptibleFuture();

    BaseInterruptiblePromise<TypeParam> moved_to_promise{};
    moved_to_promise = std::move(moved_from_promise);

    auto expected_error = moved_to_promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(BaseInterruptiblePromiseTest, DestructionBreaksPromise)
{
    InterruptibleFuture<TypeParam> future{};
    {
        BaseInterruptiblePromise<TypeParam> promise{};

        auto expected_future = promise.GetInterruptibleFuture();
        ASSERT_TRUE(expected_future.has_value());
        future = std::move(expected_future.value());
    }

    score::cpp::stop_token stop_token{};
    auto expected_value = future.Get(stop_token);
    ASSERT_FALSE(expected_value.has_value());
    EXPECT_EQ(expected_value.error(), Error::kPromiseBroken);
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanSwap)
{
    this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    BaseInterruptiblePromise<TypeParam> other_promise{};

    this->promise_.Swap(other_promise);

    auto return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_TRUE(return_code.has_value());
    auto other_return_code = other_promise.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(other_return_code.has_value());
    ASSERT_EQ(other_return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(BaseInterruptiblePromiseTest, CanSetError)
{
    auto return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_TRUE(return_code.has_value());

    score::cpp::stop_token stop_token{};
    auto expected_error = this->future_.Get(stop_token);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(BaseInterruptiblePromiseTest, SettingErrorTwiceReturnsPromiseAlreadySatisfiedError)
{
    this->promise_.SetError(Error::kFutureAlreadyRetrieved);

    auto return_code = this->promise_.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(return_code.has_value());
    EXPECT_EQ(return_code.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(BaseInterruptiblePromiseTest, SettingErrorOnMovedFromPromiseFailesWithNoState)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    BaseInterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_from_promise.SetError(Error::kFutureAlreadyRetrieved);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptiblePromiseTest, RetrievedFutureSharesStateWithPromise)
{
    this->promise_.SetError(Error::kPromiseAlreadySatisfied);

    score::cpp::stop_token stop_token{};
    auto expected_error = this->future_.Get(stop_token);
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
}

TYPED_TEST(BaseInterruptiblePromiseTest, RetrievedFutureSharesStateWithPromiseInDifferentThreads)
{

    auto async_future = std::async([this]() noexcept {
        score::cpp::stop_token stop_token{};
        ASSERT_TRUE(this->future_.Valid());
        auto expected_error = this->future_.Get(stop_token);
        ASSERT_FALSE(expected_error.has_value());
        EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
        return;
    });

    this->promise_.SetError(Error::kPromiseAlreadySatisfied);

    async_future.wait();
}

TYPED_TEST(BaseInterruptiblePromiseTest, RetrievedSharedFutureSharesStateWithPromiseInDifferentThreads)
{
    auto async_future = std::async([this]() noexcept {
        score::cpp::stop_token stop_token{};
        InterruptibleSharedFuture<TypeParam> shared_future = this->future_.Share();
        auto expected_error = shared_future.Get(stop_token);
        ASSERT_FALSE(expected_error.has_value());
        EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
        return;
    });

    this->promise_.SetError(Error::kPromiseAlreadySatisfied);

    async_future.wait();
}

TYPED_TEST(BaseInterruptiblePromiseTest, RetrievedSharedFuturesSharesStateWithPromiseInDifferentThreads)
{
    InterruptibleSharedFuture<TypeParam> shared_future_1 = this->future_.Share();
    InterruptibleSharedFuture<TypeParam> shared_future_2{shared_future_1};

    auto async_future_1 = std::async([shared_future_1]() mutable noexcept {
        score::cpp::stop_token stop_token{};
        auto expected_error = shared_future_1.Get(stop_token);
        ASSERT_FALSE(expected_error.has_value());
        EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
        return;
    });

    auto async_future_2 = std::async([shared_future_2]() mutable noexcept {
        score::cpp::stop_token stop_token{};
        auto expected_error = shared_future_2.Get(stop_token);
        ASSERT_FALSE(expected_error.has_value());
        EXPECT_EQ(expected_error.error(), Error::kPromiseAlreadySatisfied);
        return;
    });

    this->promise_.SetError(Error::kPromiseAlreadySatisfied);

    async_future_1.wait();
    async_future_2.wait();
}

TYPED_TEST(BaseInterruptiblePromiseTest, RetrievingFutureTwiceReturnsFutureAlreadyRetrievedError)
{
    BaseInterruptiblePromise<TypeParam> promise{};
    auto expected_future = promise.GetInterruptibleFuture();
    ASSERT_TRUE(expected_future.has_value());

    auto expected_error = promise.GetInterruptibleFuture();
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kFutureAlreadyRetrieved);
}

TYPED_TEST(BaseInterruptiblePromiseTest, AbortionCallbackNotStoredOnInvalidPromise)
{
    BaseInterruptiblePromise<TypeParam> moved_from_promise{};
    BaseInterruptiblePromise<TypeParam> moved_to_promise{std::move(moved_from_promise)};

    auto expected_error = moved_from_promise.OnAbort([]() noexcept {});
    ASSERT_FALSE(expected_error.has_value());
    EXPECT_EQ(expected_error.error(), Error::kNoState);
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
