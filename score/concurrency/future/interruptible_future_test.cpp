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
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/future/interruptible_shared_future.h"
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
class InterruptibleFutureTestBase : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto future_expected = promise_.GetInterruptibleFuture();
        ASSERT_TRUE(future_expected.has_value());
        this->future_ = std::move(future_expected.value());
    }

    InterruptiblePromise<T> promise_{};
    InterruptibleFuture<T> future_{};
};

template <typename T>
class InterruptibleFutureTest : public InterruptibleFutureTestBase<T>
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

template <>
class InterruptibleFutureTest<testing::MoveOnlyType> : public InterruptibleFutureTestBase<testing::MoveOnlyType>
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

template <typename T>
class InterruptibleFutureTest<T&> : public InterruptibleFutureTestBase<T&>
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

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <>
class InterruptibleFutureTest<void> : public InterruptibleFutureTestBase<void>
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
TYPED_TEST_SUITE(InterruptibleFutureTest, TypesUnderTest, /*unused*/);

TYPED_TEST(InterruptibleFutureTest, CanDefaultConstructInterruptibleFuture)
{
    EXPECT_NO_FATAL_FAILURE(InterruptibleFuture<TypeParam> future{});
}

TYPED_TEST(InterruptibleFutureTest, CanNotCopyConstructInterruptibleFuture)
{
    EXPECT_FALSE(std::is_copy_constructible<InterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleFutureTest, CanNotCopyAssignInterruptibleFuture)
{
    EXPECT_FALSE(std::is_copy_assignable<InterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleFutureTest, CanMoveConstructInterruptibleFuture)
{
    EXPECT_TRUE(std::is_move_constructible<InterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleFutureTest, CanMoveAssignInterruptibleFuture)
{
    EXPECT_TRUE(std::is_move_assignable<InterruptibleFuture<TypeParam>>::value);
}

TYPED_TEST(InterruptibleFutureTest, Destruction)
{
    {
        InterruptibleFuture<TypeParam> stack_future{};
        (void)stack_future;
    }

    auto heap_future = std::make_unique<InterruptibleFuture<TypeParam>>();
    heap_future.reset();

    std::unique_ptr<detail::BaseInterruptibleFuture<TypeParam>> heap_base_future =
        std::make_unique<TestOnlyBaseInterruptibleFuture<TypeParam>>();
    heap_base_future.reset();
}

TYPED_TEST(InterruptibleFutureTest, DefaultConstructedInterruptibleFutureIsInvalid)
{
    InterruptibleFuture<TypeParam> future{};
    EXPECT_FALSE(future.Valid());
}

TYPED_TEST(InterruptibleFutureTest, MovedFromFutureHasNoStateAfterMoveConstruction)
{
    InterruptibleFuture<TypeParam> moved_to_future{std::move(this->future_)};
    EXPECT_FALSE(this->future_.Valid());
}

TYPED_TEST(InterruptibleFutureTest, MovedFromFutureHasNoStateAfterMoveAssignment)
{
    InterruptibleFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_);
    EXPECT_FALSE(this->future_.Valid());
}

TYPED_TEST(InterruptibleFutureTest, MovedToFutureHasStateAfterMoveConstruction)
{
    InterruptibleFuture<TypeParam> moved_to_future{std::move(this->future_)};
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(InterruptibleFutureTest, MovedToFutureHasStateAfterMoveAssignment)
{
    InterruptibleFuture<TypeParam> moved_to_future{};
    moved_to_future = std::move(this->future_);
    EXPECT_TRUE(moved_to_future.Valid());
}

TYPED_TEST(InterruptibleFutureTest, FutureIsValidWhenSharedStateExistsButPromiseIsBorken)
{
    this->promise_ = InterruptiblePromise<TypeParam>{};
    ASSERT_TRUE(this->future_.Valid());
}

TYPED_TEST(InterruptibleFutureTest, FutureIsInvalidAfterSharing)
{
    score::cpp::stop_token stop_token{};

    auto shared_future = this->future_.Share();
    ASSERT_FALSE(this->future_.Valid());
}

TYPED_TEST(InterruptibleFutureTest, CanShareFuture)
{
    score::cpp::stop_token stop_token{};

    auto shared_future = this->future_.Share();
    InterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    auto expected_value = shared_future.Get(stop_token);
    ASSERT_TRUE(expected_value.has_value());
    InterruptibleFutureTest<TypeParam>::ExpectCorrectValue(expected_value);
}

TYPED_TEST(InterruptibleFutureTest, GetReturnsErrorWhenStopRequested)
{
    score::cpp::stop_source stop_source{};
    auto stop_token = stop_source.get_token();
    stop_source.request_stop();

    auto expected = this->future_.Get(stop_token);
    ASSERT_FALSE(expected.has_value());
    EXPECT_EQ(expected.error(), Error::kStopRequested);
}

TYPED_TEST(InterruptibleFutureTest, NoDeadlockWhenTriggeringContinuationsOnFulfilledPromise)
{
    // Given a Future with a fulfilled promise
    InterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    // When a continuation attaches another continuation
    bool marker = false;
    safecpp::Scope scope{};
    this->future_.Then({scope, [this, &scope, &marker](const auto&) {
                            this->future_.Then({scope, [&marker](const auto&) {
                                                    marker = true;
                                                }});
                        }});

    // Then both continuations are triggered
    EXPECT_TRUE(marker);
}

TYPED_TEST(InterruptibleFutureTest, NoDeadlockWhenTriggeringContinuationsWhenFulfillingPromise)
{
    // Given a Future with a unfulfilled promise

    // When a continuation attaches another continuation
    bool marker = false;
    safecpp::Scope scope{};
    this->future_.Then({scope, [this, &scope, &marker](const auto&) {
                            this->future_.Then({scope, [&marker](const auto&) {
                                                    marker = true;
                                                }});
                        }});
    // and promise is fulfilled
    InterruptibleFutureTest<TypeParam>::SetPromise(this->promise_);

    // Then both continuations are triggered
    EXPECT_TRUE(marker);
}

}  // namespace
}  // namespace concurrency
}  // namespace score
