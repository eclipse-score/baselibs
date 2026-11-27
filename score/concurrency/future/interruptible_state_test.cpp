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

#include <cstdint>
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

// The two values the setter tests hand to SetValue(). They differ so that a rejected second call can
// be shown to have left the first value in place, rather than to have overwritten it with an equal
// one.
constexpr int kFirstValue{1};
constexpr int kOtherValue{2};

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
        return this->state_->SetValue(first_value_);
    }

    bool SetOtherValueOnState()
    {
        return this->state_->SetValue(other_value_);
    }

    void ExpectHoldsFirstValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().GetValue(), kFirstValue);
    }

  private:
    T first_value_{kFirstValue};
    T other_value_{kOtherValue};
};

template <typename T>
class InterruptibleStateSetterTest<T&> : public InterruptibleStateSetterTestBase<T&>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue(first_value_);
    }

    bool SetOtherValueOnState()
    {
        return this->state_->SetValue(other_value_);
    }

    void ExpectHoldsFirstValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().get().GetValue(), kFirstValue);
    }

  private:
    T first_value_{kFirstValue};
    T other_value_{kOtherValue};
};

// MoveOnlyType and ThrowingMoveOnlyType both have a deleted copy constructor, so the const-reference overload of
// SetValue() is disabled for them and the value has to be handed over as an rvalue.
template <typename T>
class MoveOnlyInterruptibleStateSetterTest : public InterruptibleStateSetterTestBase<T>
{
  protected:
    bool SetValueOnState()
    {
        return this->state_->SetValue(std::move(first_value_));
    }

    bool SetOtherValueOnState()
    {
        return this->state_->SetValue(std::move(other_value_));
    }

    void ExpectHoldsFirstValue()
    {
        auto& result = this->state_->GetValue();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().GetValue(), kFirstValue);
    }

  private:
    T first_value_{kFirstValue};
    T other_value_{kOtherValue};
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

    // SetValue() carries no value for a void state, so there is no second value to tell apart here;
    // what the second call has to show is only that it is rejected.
    bool SetOtherValueOnState()
    {
        return this->state_->SetValue();
    }

    void ExpectHoldsFirstValue()
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
    this->ExpectHoldsFirstValue();
}

TYPED_TEST(InterruptibleStateSetterTest, SetValueASecondTimeReturnsFalseAndKeepsTheFirstValue)
{
    ASSERT_TRUE(this->SetValueOnState());

    EXPECT_FALSE(this->SetOtherValueOnState());
    this->ExpectHoldsFirstValue();
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
    this->ExpectHoldsFirstValue();
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

// SetValue() constructs the stored value in place from whatever it is handed.
// That is either an object of the value type, copied or moved from as before, or the arguments of a constructor.
// The cases below are the ones only the second form can reach.
// They also pin the behaviour that must not change now that the value category of the arguments is preserved.
template <typename State, typename AlwaysVoid, typename... Args>
struct IsSetValueViableImpl : std::false_type
{
};

template <typename State, typename... Args>
struct IsSetValueViableImpl<State,
                            decltype(static_cast<void>(std::declval<State&>().SetValue(std::declval<Args>()...))),
                            Args...> : std::true_type
{
};

template <typename State, typename... Args>
using IsSetValueViable = IsSetValueViableImpl<State, void, Args...>;

static_assert(IsSetValueViable<InterruptibleState<testing::NeitherCopyNorMoveType>, int>::value,
              "a value that can be neither copied nor moved must be settable from its constructor arguments");
static_assert(IsSetValueViable<InterruptibleState<testing::NeitherCopyNorMoveType>, int, int>::value,
              "several constructor arguments must be forwarded together");
static_assert(
    !IsSetValueViable<InterruptibleState<testing::NeitherCopyNorMoveType>, testing::NeitherCopyNorMoveType&&>::value,
    "an object of a type that cannot be moved still cannot be handed over by value; that is the language");
static_assert(IsSetValueViable<InterruptibleState<testing::CopyAndMovableType>, int>::value,
              "an explicit converting constructor must be usable, since the value is constructed, not converted");
static_assert(IsSetValueViable<InterruptibleState<testing::DefaultConstructibleType>>::value,
              "a default constructible value must be settable with no arguments");

TEST(InterruptibleStateInPlaceSetValueTest, ConstructsAValueThatCanBeNeitherCopiedNorMoved)
{
    // An int converts to this type implicitly, and still has to arrive here, because no typed overload exists for it.
    const auto state{InterruptibleState<testing::NeitherCopyNorMoveType>::Make()};

    EXPECT_TRUE(state->SetValue(kFirstValue));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, ForwardsSeveralConstructorArgumentsTogether)
{
    const auto state{InterruptibleState<testing::NeitherCopyNorMoveType>::Make()};

    EXPECT_TRUE(state->SetValue(kFirstValue, kOtherValue));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue + kOtherValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, ASecondCallReturnsFalseAndKeepsTheFirstValue)
{
    const auto state{InterruptibleState<testing::NeitherCopyNorMoveType>::Make()};
    ASSERT_TRUE(state->SetValue(kFirstValue));

    EXPECT_FALSE(state->SetValue(kOtherValue));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, AcceptsAnExplicitConvertingConstructor)
{
    // CopyAndMovableType's constructor from int is explicit, so no int converts to it implicitly.
    const auto state{InterruptibleState<testing::CopyAndMovableType>::Make()};

    EXPECT_TRUE(state->SetValue(kFirstValue));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, DefaultConstructsTheValueWhenGivenNoArguments)
{
    const auto state{InterruptibleState<testing::DefaultConstructibleType>::Make()};

    EXPECT_TRUE(state->SetValue());

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), testing::DefaultConstructibleType::kDefaultValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, HandingOverAnObjectMovesItExactlyOnce)
{
    // The contrast for the test below: without it, a counter that never counted would make that test pass vacuously.
    int move_count{0};
    const auto state{InterruptibleState<testing::MoveCountingType>::Make()};

    constexpr int kExactlyOnce{1};

    ASSERT_TRUE(state->SetValue(testing::MoveCountingType{kFirstValue, move_count}));

    EXPECT_EQ(move_count, kExactlyOnce);
}

TEST(InterruptibleStateInPlaceSetValueTest, HandingOverConstructorArgumentsMovesNothing)
{
    int move_count{0};
    const auto state{InterruptibleState<testing::MoveCountingType>::Make()};

    constexpr int kNotAtAll{0};

    ASSERT_TRUE(state->SetValue(kFirstValue, move_count));

    EXPECT_EQ(move_count, kNotAtAll);
    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue);
}

// SetValue() preserves the value category of its arguments, so a non-const lvalue reaches constructors as `Value&`.
// This forwarding constructor excludes its own type, so the copy constructor is still chosen.
TEST(InterruptibleStateInPlaceSetValueTest, CopiesFromANonConstLvalueWhenTheForwardingConstructorExcludesItsOwnType)
{
    const auto state{InterruptibleState<testing::ForwardingCtorExcludingSelfType>::Make()};
    testing::ForwardingCtorExcludingSelfType value{kFirstValue};
    ASSERT_FALSE(value.WasCopyConstructed());

    ASSERT_TRUE(state->SetValue(value));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasCopyConstructed());
}

// Handing SetValue() an existing object, or a braced-init-list naming one, must resolve as it did before.
// A braced-init-list has no type, so it cannot be deduced into a parameter pack.
// A non-const lvalue would otherwise be offered to overload resolution as `Value&`.
// The in-place overload has to stay out of both cases.
TEST(InterruptibleStateInPlaceSetValueTest, StillAcceptsABracedInitList)
{
    const auto state{InterruptibleState<testing::ListInitialisableType>::Make()};

    EXPECT_TRUE(state->SetValue({kFirstValue, kOtherValue}));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), kFirstValue + kOtherValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, StillAcceptsAnEmptyBracedInitList)
{
    const auto state{InterruptibleState<testing::DefaultConstructibleType>::Make()};

    EXPECT_TRUE(state->SetValue({}));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().GetValue(), testing::DefaultConstructibleType::kDefaultValue);
}

TEST(InterruptibleStateInPlaceSetValueTest, CopiesFromANonConstLvalueEvenWhenTheForwardingConstructorAcceptsItsOwnType)
{
    const auto state{InterruptibleState<testing::ForwardingCtorAcceptingSelfType>::Make()};
    testing::ForwardingCtorAcceptingSelfType value{kFirstValue};
    ASSERT_FALSE(value.WasCopyConstructed());

    ASSERT_TRUE(state->SetValue(value));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasCopyConstructed());
}

TEST(InterruptibleStateInPlaceSetValueTest, CopiesFromANonConstLvalueEvenWhenTheValueTypeIsConstQualified)
{
    // The argument has the unqualified type here, which is not Value itself.
    // It is still an object to copy, so the in-place overload has to leave it to the `const Value&` overload.
    const auto state{InterruptibleState<const testing::ForwardingCtorAcceptingSelfType>::Make()};
    testing::ForwardingCtorAcceptingSelfType value{kFirstValue};
    ASSERT_FALSE(value.WasCopyConstructed());

    ASSERT_TRUE(state->SetValue(value));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasCopyConstructed());
}

// More generally, every single-argument call that compiled before the in-place overload existed must resolve as before.
// The tests below cover the cases a forwarding reference would otherwise take over.
// An object derived from Value would reach a forwarding constructor instead of the copy constructor.
// An argument that converts to Value implicitly would reach an explicit constructor that matches it better.
// A bit-field would not compile, because it cannot bind to a forwarding reference.
TEST(InterruptibleStateInPlaceSetValueTest, CopiesFromADerivedLvalueEvenThoughTheForwardingConstructorWouldAcceptIt)
{
    const auto state{InterruptibleState<testing::ForwardingCtorExcludingSelfType>::Make()};
    testing::DerivedFromForwardingCtorExcludingSelfType value{kFirstValue};
    ASSERT_FALSE(value.WasCopyConstructed());

    ASSERT_TRUE(state->SetValue(value));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasCopyConstructed());
}

TEST(InterruptibleStateInPlaceSetValueTest, CopiesFromADerivedRvalueEvenThoughTheForwardingConstructorWouldAcceptIt)
{
    // The move constructor of this type is deleted, so a conversion to the type itself would select it and fail.
    // The argument still binds to the `const Value&` parameter directly, which is the copy.
    const auto state{InterruptibleState<testing::ForwardingCtorExcludingSelfType>::Make()};

    ASSERT_TRUE(state->SetValue(testing::DerivedFromForwardingCtorExcludingSelfType{kFirstValue}));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasCopyConstructed());
}

TEST(InterruptibleStateInPlaceSetValueTest, StillConvertsImplicitlyWhenAnExplicitConstructorWouldMatchBetter)
{
    const auto state{InterruptibleState<testing::ImplicitlyAndExplicitlyConstructibleType>::Make()};

    EXPECT_TRUE(state->SetValue(kFirstValue));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result.value().WasConvertedImplicitly());
}

TEST(InterruptibleStateInPlaceSetValueTest, StillAcceptsABitFieldOfAnotherType)
{
    struct WithBitField
    {
        // coverity[autosar_cpp14_a9_6_2_violation] a bit-field is what this test is about
        std::uint32_t field : 4U;
    };
    constexpr std::uint32_t kBitFieldValue{5U};
    // Not const on purpose: a const bit-field binds to a const reference through a temporary, which always worked.
    WithBitField holder{kBitFieldValue};
    const auto state{InterruptibleState<std::int64_t>::Make()};

    EXPECT_TRUE(state->SetValue(holder.field));

    auto& result{state->GetValue()};
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), static_cast<std::int64_t>(kBitFieldValue));
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
