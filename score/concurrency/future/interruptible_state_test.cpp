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
#include "score/concurrency/future/interruptible_state.h"

#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/future/test_types.h"

#include "score/stop_token.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <future>
#include <memory>
#include <type_traits>
#include <utility>

namespace score
{
namespace concurrency
{
namespace detail
{
namespace
{

TEST(InterruptibleStateTest, Destruction)
{
    /*
     * BaseInterruptibleState inherits from std::enable_shared_from_this. Thus, it should never (!) be allocated on the
     * stack or via a unique_ptr on the heap!
     *
     * Because GCC emits symbols for all constructors and destructors defined in the Itanium C++ ABI independent of
     * their usage, we need to test these to achieve the required function coverage (100%). Hence, for this test, we
     * ignore above requirement and construct on stack, and on heap with unique pointers.
     *
     * Specifically, we use unique pointers because constructing a shared pointer using std::make_shared will not (!)
     * call the deleting destructor (D0) of BaseInterruptibleState. Instead, it will call the deleting destructor of
     * std::shared_ptr and that will in turn call the complete object destructor (D1) of BaseInterruptibleState.
     */

    // Required to cover D0 destructor
    auto heap_base_state = std::make_unique<InterruptibleState<void>>();
    heap_base_state.reset();

    // Required to cover D1 destructors
    {
        InterruptibleState<void> stack_base_state{};
        (void)stack_base_state;
    }

    // Required to cover D2 destructor
    std::shared_ptr<InterruptibleState<void>> shared_heap_base_state;
    {
        shared_heap_base_state = std::make_shared<InterruptibleState<void>>();
    }
    shared_heap_base_state.reset();
}

/*
 * The remainder of this file characterizes InterruptibleState<>::SetValue() and SetError(): which value types they
 * accept, and what the state holds afterwards. These tests describe the behaviour that exists today rather than a
 * behaviour being introduced, so that a change to the way the result is placed into value_ can be shown to preserve
 * the current API instead of narrowing it.
 */

// The value is stored as a score::Result<Value>, which inherits the special member functions of Value. A Value that
// can be constructed but not assigned therefore yields a Result that can be constructed but not assigned, which is
// why the setters construct a fresh object over value_ rather than assigning one. An implementation that assigned
// instead would stop compiling for the value type asserted on below.
static_assert(std::is_copy_constructible<testing::NonAssignableType>::value,
              "precondition: the non-assignable test type is still copy constructible");
static_assert(std::is_move_constructible<testing::NonAssignableType>::value,
              "precondition: the non-assignable test type is still move constructible");
static_assert(!std::is_copy_assignable<testing::NonAssignableType>::value,
              "precondition: the non-assignable test type must not be copy assignable");
static_assert(!std::is_move_assignable<testing::NonAssignableType>::value,
              "precondition: the non-assignable test type must not be move assignable");

static_assert(std::is_copy_constructible<score::Result<testing::NonAssignableType>>::value,
              "the stored result must remain copy constructible for a non-assignable value type");
static_assert(std::is_move_constructible<score::Result<testing::NonAssignableType>>::value,
              "the stored result must remain move constructible for a non-assignable value type");
static_assert(!std::is_copy_assignable<score::Result<testing::NonAssignableType>>::value,
              "assigning the stored result is not available for a non-assignable value type");
static_assert(!std::is_move_assignable<score::Result<testing::NonAssignableType>>::value,
              "assigning the stored result is not available for a non-assignable value type");

// The other edge of the accepted set. Constructing the stored result places no exception requirement on the value
// type, whereas score::Result<T>::emplace() is constrained on std::is_nothrow_constructible. A value type whose move
// constructor may throw is therefore accepted by the setters today, and would stop being accepted by an
// implementation that emplaced into value_ rather than constructing over it.
static_assert(std::is_move_constructible<testing::ThrowingMoveOnlyType>::value,
              "precondition: the throwing-move test type is still move constructible");
static_assert(!std::is_nothrow_move_constructible<testing::ThrowingMoveOnlyType>::value,
              "precondition: the throwing-move test type must not be nothrow move constructible");
static_assert(
    std::is_constructible<score::Result<testing::ThrowingMoveOnlyType>, testing::ThrowingMoveOnlyType&&>::value,
    "the stored result must remain constructible from a value whose move constructor may throw");

template <typename T>
class InterruptibleStateSetterTestBase : public ::testing::Test
{
  protected:
    void ExpectHoldsError(const Error expected_error)
    {
        auto& result = state_->GetValue();
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), expected_error);
    }

    std::shared_ptr<InterruptibleState<T>> state_{InterruptibleState<T>::Make()};
};

template <typename T>
class InterruptibleStateSetterTest : public InterruptibleStateSetterTestBase<T>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue(value_);
    }

    void ExpectHoldsExpectedValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <typename T>
class InterruptibleStateSetterTest<T&> : public InterruptibleStateSetterTestBase<T&>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue(value_);
    }

    void ExpectHoldsExpectedValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().get().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

// MoveOnlyType and ThrowingMoveOnlyType both have a deleted copy constructor, so the const-reference overload of
// SetValue() is disabled for them and the value has to be handed over as an rvalue.
template <typename T>
class MoveOnlyInterruptibleStateSetterTest : public InterruptibleStateSetterTestBase<T>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue(std::move(value_));
    }

    void ExpectHoldsExpectedValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().GetValue(), expected_value_);
    }

  private:
    int expected_value_{1};
    T value_{expected_value_};
};

template <>
class InterruptibleStateSetterTest<testing::MoveOnlyType>
    : public MoveOnlyInterruptibleStateSetterTest<testing::MoveOnlyType>
{
};

template <>
class InterruptibleStateSetterTest<testing::ThrowingMoveOnlyType>
    : public MoveOnlyInterruptibleStateSetterTest<testing::ThrowingMoveOnlyType>
{
};

template <>
class InterruptibleStateSetterTest<void> : public InterruptibleStateSetterTestBase<void>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue();
    }

    void ExpectHoldsExpectedValue()
    {
        EXPECT_TRUE(this->state_->GetValue().has_value());
    }
};

using SetterTypesUnderTest = ::testing::Types<testing::CopyAndMovableType,
                                              testing::CopyAndMovableType&,
                                              testing::CopyOnlyType,
                                              testing::CopyOnlyType&,
                                              testing::MoveOnlyType,
                                              testing::MoveOnlyType&,
                                              testing::NonAssignableType,
                                              testing::NonAssignableType&,
                                              testing::ThrowingMoveOnlyType,
                                              void>;
TYPED_TEST_SUITE(InterruptibleStateSetterTest, SetterTypesUnderTest, /*unused*/);

TYPED_TEST(InterruptibleStateSetterTest, SetValueStoresTheValueAndReturnsTrue)
{
    EXPECT_TRUE(this->SetValueOnState());
    this->ExpectHoldsExpectedValue();
}

TYPED_TEST(InterruptibleStateSetterTest, SetValueASecondTimeReturnsFalseAndKeepsTheFirstValue)
{
    ASSERT_TRUE(this->SetValueOnState());

    EXPECT_FALSE(this->SetValueOnState());
    this->ExpectHoldsExpectedValue();
}

TYPED_TEST(InterruptibleStateSetterTest, SetErrorStoresTheErrorAndReturnsTrue)
{
    EXPECT_TRUE(this->state_->SetError(Error::kPromiseBroken));
    this->ExpectHoldsError(Error::kPromiseBroken);
}

TYPED_TEST(InterruptibleStateSetterTest, SetErrorASecondTimeReturnsFalseAndKeepsTheFirstError)
{
    ASSERT_TRUE(this->state_->SetError(Error::kPromiseBroken));

    EXPECT_FALSE(this->state_->SetError(Error::kStopRequested));
    this->ExpectHoldsError(Error::kPromiseBroken);
}

TYPED_TEST(InterruptibleStateSetterTest, SetErrorAfterSetValueReturnsFalseAndKeepsTheValue)
{
    ASSERT_TRUE(this->SetValueOnState());

    EXPECT_FALSE(this->state_->SetError(Error::kPromiseBroken));
    this->ExpectHoldsExpectedValue();
}

TYPED_TEST(InterruptibleStateSetterTest, SetValueAfterSetErrorReturnsFalseAndKeepsTheError)
{
    ASSERT_TRUE(this->state_->SetError(Error::kPromiseBroken));

    EXPECT_FALSE(this->SetValueOnState());
    this->ExpectHoldsError(Error::kPromiseBroken);
}

// The two states below start out differently, and both shapes are relied upon elsewhere: a valued state reports the
// kUnset error until a setter runs, whereas InterruptibleState<void> default-constructs a score::Result<void> that
// already holds a value, so readiness rather than the stored result is what marks it as unset.
TEST(InterruptibleStateInitialResultTest, ValuedStateStartsWithTheUnsetError)
{
    const auto state = InterruptibleState<testing::CopyAndMovableType>::Make();

    auto& result = state->GetValue();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::kUnset);
}

TEST(InterruptibleStateInitialResultTest, VoidStateStartsWithAValue)
{
    const auto state = InterruptibleState<void>::Make();

    EXPECT_TRUE(state->GetValue().has_value());
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
