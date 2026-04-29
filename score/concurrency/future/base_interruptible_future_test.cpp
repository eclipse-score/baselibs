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
#include "score/concurrency/future/base_interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/future/test_types.h"

#include "score/stop_token.hpp"

#include "gtest/gtest.h"

#include <future>
#include <optional>

namespace score
{
namespace concurrency
{
namespace detail
{
namespace
{

constexpr std::chrono::milliseconds TIMEOUT{100};

template <typename T>
class BaseInterruptibleFutureTestBase : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto future_expected = promise_.GetInterruptibleFuture();
        ASSERT_TRUE(future_expected.has_value());
        this->future_ = BaseInterruptibleFuture<T>{std::move(future_expected).value()};
    }

  public:
    void PrepareOnAbortCallback()
    {
        promise_.OnAbort([this]() noexcept {
            invoked_++;
        });
    }

    void ExpectCallbackInvokedNTimes(std::uint32_t n)
    {
        EXPECT_EQ(this->invoked_, n);
    }

    void ExpectCallbackNotInvoked()
    {
        EXPECT_EQ(this->invoked_, 0);
    }

    std::optional<BaseInterruptibleFuture<T>> future_{};
    InterruptiblePromise<T> promise_{};
    std::uint32_t invoked_{0};
    score::cpp::stop_source stop_source_{};
};

template <typename T>
class BaseInterruptibleFutureTest : public BaseInterruptibleFutureTestBase<T>
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

    void PrepareScopedContinuationCallback()
    {
        safecpp::MoveOnlyScopedFunction<void(score::Result<T>&)> scoped_callback{scope_,
                                                                               [this](score::Result<T>&) noexcept {
                                                                                   this->invoked_++;
                                                                               }};
        this->future_->Then(std::move(scoped_callback));
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
    safecpp::Scope<> scope_{};
};

template <>
class BaseInterruptibleFutureTest<testing::MoveOnlyType> : public BaseInterruptibleFutureTestBase<testing::MoveOnlyType>
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

    void PrepareScopedContinuationCallback()
    {
        safecpp::MoveOnlyScopedFunction<void(score::Result<testing::MoveOnlyType>&)> scoped_callback{
            scope_, [this](score::Result<testing::MoveOnlyType>&) noexcept {
                this->invoked_++;
            }};
        this->future_->Then(std::move(scoped_callback));
    }

  private:
    int expected_value_{1};
    testing::MoveOnlyType value_{expected_value_};
    safecpp::Scope<> scope_{};
};

template <typename T>
class BaseInterruptibleFutureTest<T&> : public BaseInterruptibleFutureTestBase<T&>
{
  protected:
    void SetPromise(InterruptiblePromise<T&>& promise) noexcept
    {
        promise.SetValue(value_);
    }

    void ExpectCorrectValue(const score::Result<std::reference_wrapper<T>>& actual_value_expected) const noexcept
    {
        ASSERT_TRUE(actual_value_expected.has_value());
        EXPECT_EQ(actual_value_expected.value().get().GetValue(), expected_value_);
    }

    void PrepareScopedContinuationCallback()
    {
        safecpp::MoveOnlyScopedFunction<void(score::Result<std::reference_wrapper<T>>&)> scoped_callback{
            scope_, [this](score::Result<std::reference_wrapper<T>>&) noexcept {
                this->invoked_++;
            }};
        this->future_->Then(std::move(scoped_callback));
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
    safecpp::Scope<> scope_{};
};

template <>
class BaseInterruptibleFutureTest<void> : public BaseInterruptibleFutureTestBase<void>
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

    void PrepareScopedContinuationCallback()
    {
        safecpp::MoveOnlyScopedFunction<void(score::Result<void>&)> scoped_callback{scope_,
                                                                                  [this](score::Result<void>&) noexcept {
                                                                                      this->invoked_++;
                                                                                  }};
        this->future_->Then(std::move(scoped_callback));
    }

  private:
    safecpp::Scope<> scope_{};
};

using TypesUnderTest = ::testing::Types<testing::CopyAndMovableType,
                                        testing::CopyAndMovableType&,
                                        testing::CopyOnlyType,
                                        testing::CopyOnlyType&,
                                        testing::MoveOnlyType,
                                        testing::MoveOnlyType&,
                                        void>;
TYPED_TEST_SUITE(BaseInterruptibleFutureTest, TypesUnderTest, /*unused*/);

TYPED_TEST(BaseInterruptibleFutureTest, CanDefaultConstructInterruptibleFuture)
{
    EXPECT_NO_FATAL_FAILURE(BaseInterruptibleFuture<TypeParam> future{});
}

TYPED_TEST(BaseInterruptibleFutureTest, CanCopyConstructInterruptibleFuture)
{
    EXPECT_TRUE(std::is_copy_constructible<BaseInterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptibleFutureTest, CanCopyAssignInterruptibleFuture)
{
    EXPECT_TRUE(std::is_copy_assignable<BaseInterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptibleFutureTest, CanMoveConstructInterruptibleFuture)
{
    EXPECT_TRUE(std::is_move_constructible<BaseInterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptibleFutureTest, CanMoveAssignInterruptibleFuture)
{
    EXPECT_TRUE(std::is_move_assignable<BaseInterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(BaseInterruptibleFutureTest, Destruction)
{
    // Required to cover D0 destructor
    auto heap_future = std::make_unique<BaseInterruptibleFuture<TypeParam>>();
    heap_future.reset();

    // Required to cover D1 destructor
    {
        BaseInterruptibleFuture<TypeParam> stack_future{};
        (void)stack_future;
    }

    // Required to cover D2 destructor
    std::shared_ptr<BaseInterruptibleFuture<TypeParam>> shared_heap_future;
    {
        shared_heap_future = std::make_shared<BaseInterruptibleFuture<TypeParam>>();
    }
    shared_heap_future.reset();
}

TYPED_TEST(BaseInterruptibleFutureTest, DefaultConstructedInterruptibleFutureIsInvalid)
{
    BaseInterruptibleFuture<TypeParam> future{};
    EXPECT_FALSE(future.Valid());
}

// Force extra decision coverage on copy constructor/assignement:
TYPED_TEST(BaseInterruptibleFutureTest, CopyConstructionFromInvalidSource)
{
    BaseInterruptibleFuture<TypeParam> future{};
    EXPECT_FALSE(future.Valid());
    BaseInterruptibleFuture<TypeParam> future_copy{future};
    future_copy = future_copy;  // Force case where this == other.
    future_copy = future;       // Force case where this != other, but state is invalid.
}

// Force decision coverage on protected constructor, in case it would receive a nullptr
template <typename Value>
class TestOnlyBaseInterruptibleFuture : public ::score::concurrency::detail::BaseInterruptibleFuture<Value>
{
  public:
    TestOnlyBaseInterruptibleFuture(std::shared_ptr<InterruptibleState<Value>> state_ptr) noexcept
        : BaseInterruptibleFuture<Value>(state_ptr)
    {
    }
};

TYPED_TEST(BaseInterruptibleFutureTest, ConstructWithNullPointer)
{
    auto int_state = InterruptibleState<TypeParam>::Make();
    int_state.reset();
    TestOnlyBaseInterruptibleFuture<TypeParam> future{int_state};
    EXPECT_FALSE(future.Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, MovedFromFutureHasNoStateAfterMoveConstruction)
{
    BaseInterruptibleFuture<TypeParam> moved_to_future{std::move(this->future_.value())};
    EXPECT_FALSE(this->future_->Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, MovedFromFutureHasNoStateAfterMoveAssignment)
{
    BaseInterruptibleFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_.value());
    EXPECT_FALSE(this->future_->Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, MovedToFutureHasStateAfterMoveConstruction)
{
    BaseInterruptibleFuture<TypeParam> moved_to_future{std::move(this->future_.value())};
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, MovedToFutureHasStateAfterMoveAssignment)
{
    BaseInterruptibleFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_.value());
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, FutureIsValidWhenSharedStateExistsButPromiseIsBorken)
{
    this->promise_ = InterruptiblePromise<TypeParam>{};
    ASSERT_TRUE(this->future_->Valid());
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitReturnsOnlyAfterValueWasSet)
{
    score::cpp::stop_token stop_token{};

    auto async_future = std::async([this, &stop_token]() noexcept {
        return this->future_->Wait(stop_token);
    });

    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    auto expected = std::move(async_future.get());
    EXPECT_TRUE(expected.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitReturnsImmediatelyWhenNoState)
{
    score::cpp::stop_token stop_token{};
    BaseInterruptibleFuture<TypeParam> future{};

    auto expected = future.Wait(stop_token);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitReturnsImmediatelyWhenPromiseAlreadyBroken)
{
    score::cpp::stop_token stop_token{};
    std::optional<BaseInterruptibleFuture<TypeParam>> future{};
    {
        InterruptiblePromise<TypeParam> promise{};
        future = BaseInterruptibleFuture<TypeParam>{promise.GetInterruptibleFuture().value()};
    }

    auto expected = future->Wait(stop_token);
    ASSERT_TRUE(expected.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitForReturnsTimeoutErrorWhenValueWasNotSet)
{
    score::cpp::stop_token stop_token{};

    auto async_future = std::async([this, &stop_token]() noexcept {
        return this->future_->WaitFor(stop_token, TIMEOUT);
    });

    auto expected = std::move(async_future.get());
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kTimeout);
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitForReturnsImmediatelyWhenNoState)
{
    score::cpp::stop_token stop_token{};
    BaseInterruptibleFuture<TypeParam> future{};

    auto expected = future.WaitFor(stop_token, TIMEOUT);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitForReturnsImmediatelyWhenPromiseAlreadyBroken)
{
    score::cpp::stop_token stop_token{};
    std::optional<BaseInterruptibleFuture<TypeParam>> future{};
    {
        InterruptiblePromise<TypeParam> promise{};
        future = BaseInterruptibleFuture<TypeParam>{promise.GetInterruptibleFuture().value()};
    }

    auto expected = future->WaitFor(stop_token, TIMEOUT);
    ASSERT_TRUE(expected.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitForReturnsWhenValueIsSet)
{
    score::cpp::stop_token stop_token{};

    auto async_future = std::async([this, &stop_token]() noexcept {
        return this->future_->WaitFor(stop_token, TIMEOUT);
    });
    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    auto expected = std::move(async_future.get());
    EXPECT_TRUE(expected.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitUntilReturnsTimeoutErrorWhenValueWasNotSet)
{
    score::cpp::stop_token stop_token{};

    auto async_future = std::async([this, &stop_token]() noexcept {
        return this->future_->WaitUntil(stop_token, std::chrono::steady_clock::now() + TIMEOUT);
    });

    auto expected = std::move(async_future.get());
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kTimeout);
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitUntilReturnsImmediatelyWhenNoState)
{
    score::cpp::stop_token stop_token{};
    BaseInterruptibleFuture<TypeParam> future{};

    auto expected = future.WaitUntil(stop_token, std::chrono::steady_clock::now() + TIMEOUT);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kNoState);
}

TYPED_TEST(BaseInterruptibleFutureTest, WaitUntilReturnsImmediatelyWhenPromiseAlreadyBroken)
{
    score::cpp::stop_token stop_token{};
    std::optional<BaseInterruptibleFuture<TypeParam>> future{};
    {
        InterruptiblePromise<TypeParam> promise{};
        future = BaseInterruptibleFuture<TypeParam>{promise.GetInterruptibleFuture().value()};
    }

    auto expected = future->WaitUntil(stop_token, std::chrono::steady_clock::now() + TIMEOUT);
    ASSERT_TRUE(expected.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, NotAbortedOnCopyAssignment)
{
    this->PrepareOnAbortCallback();

    auto future = this->future_;

    this->ExpectCallbackNotInvoked();
}

TYPED_TEST(BaseInterruptibleFutureTest, NotAbortedOnCopyConstruction)
{
    this->PrepareOnAbortCallback();

    BaseInterruptibleFuture<TypeParam>{this->future_.value()};

    this->ExpectCallbackNotInvoked();
}

TYPED_TEST(BaseInterruptibleFutureTest, NotAbortedOnCopyBothDirections)
{
    this->PrepareOnAbortCallback();

    this->future_ = BaseInterruptibleFuture<TypeParam>{this->future_.value()};

    this->ExpectCallbackNotInvoked();
}

TYPED_TEST(BaseInterruptibleFutureTest, AbortedOnCopyAssignmentWithDifferentFuture)
{
    this->PrepareOnAbortCallback();

    this->future_ = BaseInterruptibleFuture<TypeParam>{};

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, AbortedOnMoveConstructionAfterDestruction)
{
    this->PrepareOnAbortCallback();

    {
        auto will_be_destroyed{std::move(this->future_.value())};
    }

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, AbortedOnMoveAssignmentAfterDestruction)
{
    this->PrepareOnAbortCallback();

    {
        auto will_be_destroyed = std::move(this->future_.value());
    }

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, AbortedAfterAllCopiesHaveBeenDestructed)
{
    this->PrepareOnAbortCallback();

    {
        BaseInterruptibleFuture<TypeParam> copy_will_be_destroyed{this->future_.value()};
        this->future_ = copy_will_be_destroyed;
        auto will_be_destroyed = std::move(this->future_.value());
    }

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, NotAbortedIfOneCopyIsStillAlive)
{
    this->PrepareOnAbortCallback();

    BaseInterruptibleFuture<TypeParam> copy_will_not_be_destroyed{this->future_.value()};
    {
        this->future_ = copy_will_not_be_destroyed;
        auto will_be_destroyed = std::move(this->future_.value());
    }

    this->ExpectCallbackNotInvoked();
}

TYPED_TEST(BaseInterruptibleFutureTest, ScopedContinuationExecutedOnceStateBecomesReady)
{
    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();

    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, ScopedContinuationExecutedStateAlreadyReady)
{
    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();

    this->ExpectCallbackInvokedNTimes(1);
}

TYPED_TEST(BaseInterruptibleFutureTest, MultipleScopedContinuationsExecutedOnceStateBecomesReady)
{
    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();
    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();

    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    this->ExpectCallbackInvokedNTimes(2);
}

TYPED_TEST(BaseInterruptibleFutureTest, MultipleScopedContinuationsExecutedWhenStateAlreadyReady)
{
    BaseInterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();
    BaseInterruptibleFutureTest<TypeParam>::PrepareScopedContinuationCallback();

    this->ExpectCallbackInvokedNTimes(2);
}

TYPED_TEST(BaseInterruptibleFutureTest, ScopedContinuationWithoutStateExecutesWithError)
{
    std::promise<void> promise{};
    BaseInterruptibleFuture<TypeParam> future_without_state{};

    const auto call_result =
        future_without_state.Then(typename InterruptibleState<TypeParam>::ScopedContinuationCallback{
            safecpp::Scope<>(), [&promise](auto& result) {
                ASSERT_FALSE(result.has_value());
                EXPECT_EQ(result.error(), Error::kNoState);
                promise.set_value();
            }});

    ASSERT_FALSE(call_result.has_value());
    EXPECT_EQ(call_result.error(), Error::kNoState);

    promise.get_future().wait();
}

TYPED_TEST(BaseInterruptibleFutureTest, CallAndIgnoreResultExecutesFunctionWithoutUsingReturnValue)
{
    std::atomic<bool> executed = false;

    auto test_function = [&executed](int value) noexcept {
        executed = true;
        return value * 2;
    };

    int test_in{10};
    EXPECT_NO_THROW(CallAndIgnoreResult(test_function, test_in));
    EXPECT_TRUE(executed);
}

TYPED_TEST(BaseInterruptibleFutureTest, CopyAssignmentSharesStateAndUnregistersOldFuture)
{
    InterruptiblePromise<TypeParam> promise1;
    auto future_expected1 = promise1.GetInterruptibleFuture();
    ASSERT_TRUE(future_expected1.has_value());
    BaseInterruptibleFuture<TypeParam> future1{std::move(future_expected1).value()};
    ASSERT_TRUE(future1.Valid());

    InterruptiblePromise<TypeParam> promise2;
    auto future_expected2 = promise2.GetInterruptibleFuture();
    ASSERT_TRUE(future_expected2.has_value());
    BaseInterruptibleFuture<TypeParam> future2{std::move(future_expected2).value()};
    ASSERT_TRUE(future2.Valid());

    future1 = future2;

    EXPECT_TRUE(future1.Valid());
    EXPECT_TRUE(future2.Valid());

    this->SetPromise(promise2);

    auto result1 = future1.Wait(this->stop_source_.get_token());
    auto result2 = future2.Wait(this->stop_source_.get_token());

    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
}

TYPED_TEST(BaseInterruptibleFutureTest, MoveAssignmentTransfersStateAndUnregistersOldFuture)
{
    InterruptiblePromise<TypeParam> promise1;
    auto future_expected1 = promise1.GetInterruptibleFuture();
    ASSERT_TRUE(future_expected1.has_value());
    BaseInterruptibleFuture<TypeParam> future1{std::move(future_expected1).value()};
    ASSERT_TRUE(future1.Valid());

    InterruptiblePromise<TypeParam> promise2;
    auto future_expected2 = promise2.GetInterruptibleFuture();
    ASSERT_TRUE(future_expected2.has_value());
    BaseInterruptibleFuture<TypeParam> future2{std::move(future_expected2).value()};
    ASSERT_TRUE(future2.Valid());

    future1 = std::move(future2);

    EXPECT_TRUE(future1.Valid());
    EXPECT_FALSE(future2.Valid());

    this->SetPromise(promise2);

    auto result1 = future1.Wait(this->stop_source_.get_token());
    auto result2 = future2.Wait(this->stop_source_.get_token());

    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result2.error(), Error::kNoState);
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
