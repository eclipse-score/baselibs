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

#include "score/language/safecpp/scoped_function/details/scoped_function_invoker.h"
#include "score/language/safecpp/scoped_function/details/scoped_function_invoker_test_scoped_function.h"

#include <score/jthread.hpp>
#include <score/memory.hpp>
#include <score/optional.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <type_traits>

namespace score::safecpp::details
{
namespace
{

template <class T>
class ScopedFunctionInvokerTest : public ::testing::Test
{
  public:
    using ScopedFunctionInvoker = typename T::first_type;
    using NoExcept = typename T::second_type;
    testing::MockFunction<std::uint8_t()> mock_function{};
    static constexpr std::uint8_t kValue{43U};
};

template <class NoExcept>
using IntermediateType =
    std::pair<details::ScopedFunctionInvoker<NoExcept::value, score::cpp::optional<std::uint8_t>()>, NoExcept>;

using MyTypes = ::testing::Types<IntermediateType<std::true_type>, IntermediateType<std::false_type>>;

TYPED_TEST_SUITE(ScopedFunctionInvokerTest, MyTypes, /* unused */);

TYPED_TEST(ScopedFunctionInvokerTest, CanCallCallableIfScopeAndCallableOk)
{
    // Given a valid ScopedFunctionInvokerTestScopedFunction which was constructed from a scope, a lambda function and
    // an ActualInvoker which invokes the function
    const std::shared_ptr<details::ScopeState> scope_state{std::make_shared<details::ScopeState>()};

    ScopedFunctionInvokerTestScopedFunction<std::uint8_t()> function{scope_state, this->mock_function.AsStdFunction()};

    auto invoker = ScopedFunctionInvokerTest<TypeParam>::ScopedFunctionInvoker::template ActualInvoker<
        ScopedFunctionInvokerTestScopedFunction<std::uint8_t()>>();

    // Expected one call to the function which returns kValue
    EXPECT_CALL(this->mock_function, Call()).WillOnce(testing::Return(this->kValue));

    // When the mock is invoked
    const score::cpp::optional<std::uint8_t> was_executed = invoker(function);

    // Then the return value should have the value which was passed into the function
    ASSERT_TRUE(was_executed.has_value());
    EXPECT_EQ(was_executed.value(), this->kValue);
}

TYPED_TEST(ScopedFunctionInvokerTest, CanNotCallCallableIfScopeEmpty)
{
    // Given a ScopedFunctionInvokerTestScopedFunction which was constructed from an empty scope, a lambda function and
    // an ActualInvoker which invokes the function
    const std::shared_ptr<details::ScopeState> scope_state = nullptr;

    ScopedFunctionInvokerTestScopedFunction<std::uint8_t()> function{scope_state, this->mock_function.AsStdFunction()};

    auto invoker = ScopedFunctionInvokerTest<TypeParam>::ScopedFunctionInvoker::template ActualInvoker<
        ScopedFunctionInvokerTestScopedFunction<std::uint8_t()>>();

    // Expected no call to the mock function
    EXPECT_CALL(this->mock_function, Call()).Times(0);

    // When the mock is invoked
    const score::cpp::optional<std::uint8_t> was_executed = invoker(function);

    // Then there should be an empty result
    ASSERT_FALSE(was_executed.has_value());
}

TYPED_TEST(ScopedFunctionInvokerTest, CanNotCallCallableIfCallableEmpty)
{
    // Given a ScopedFunctionInvokerTestScopedFunction which was constructed from a scope, a an empty lambda function
    // and an ActualInvoker which invokes the function
    const std::shared_ptr<details::ScopeState> scope_state{std::make_shared<details::ScopeState>()};

    ScopedFunctionInvokerTestScopedFunction<std::uint8_t()> function{
        scope_state,
        details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<std::uint8_t()>,
                                                  std::allocator<std::uint8_t>>{}};

    auto invoker = ScopedFunctionInvokerTest<TypeParam>::ScopedFunctionInvoker::template ActualInvoker<
        ScopedFunctionInvokerTestScopedFunction<std::uint8_t()>>();

    // Expected no call to the mock function
    EXPECT_CALL(this->mock_function, Call()).Times(0);

    // When the mock is invoked
    const score::cpp::optional<std::uint8_t> was_executed = invoker(function);

    // Then there should be an empty result
    ASSERT_FALSE(was_executed.has_value());
}

TYPED_TEST(ScopedFunctionInvokerTest, CanNotCallCallableIfCallableAndScopeEmpty)
{
    // Given a ScopedFunctionInvokerTestScopedFunction which was constructed from an empty scope, a an empty lambda
    // function and an ActualInvoker which invokes the function
    const std::shared_ptr<details::ScopeState> scope_state = nullptr;

    ScopedFunctionInvokerTestScopedFunction<std::uint8_t()> function{
        scope_state,
        details::AllocatorAwareTypeErasurePointer<details::FunctionWrapper<std::uint8_t()>,
                                                  std::allocator<std::uint8_t>>{}};

    auto invoker = ScopedFunctionInvokerTest<TypeParam>::ScopedFunctionInvoker::template ActualInvoker<
        ScopedFunctionInvokerTestScopedFunction<std::uint8_t()>>();

    // Expected no call to the mock function
    EXPECT_CALL(this->mock_function, Call()).Times(0);

    // When the mock is invoked
    const score::cpp::optional<std::uint8_t> was_executed = invoker(function);

    // Then there should be an empty result
    ASSERT_FALSE(was_executed.has_value());
}

TYPED_TEST(ScopedFunctionInvokerTest, ReturnTypeIsSameAsInvokerTypeNoExcept)
{
    // Given an actual callable type for the invocer and an expected return type
    constexpr bool kNoExcept = ScopedFunctionInvokerTest<TypeParam>::NoExcept::value;

    using CallableType = std::uint8_t(std::int32_t);
    using WrappedCallableType = score::cpp::optional<std::uint8_t>(std::int32_t);

    using ActualCallableType =
        typename details::ScopedFunctionInvoker<kNoExcept, WrappedCallableType>::template CallableType<
            ScopedFunctionInvokerTestScopedFunction<CallableType>>;

    using ExpectedCallableType = score::cpp::optional<std::uint8_t> (*)(
        ScopedFunctionInvokerTestScopedFunction<CallableType>&, std::int32_t) noexcept(kNoExcept);

    // When comparing the Callable types
    // Then they should be equal
    static_assert(std::is_same_v<ActualCallableType, ExpectedCallableType>);

    // When getting the returntype and comparing it to the expected callable type
    // Then they should be equal
    using ReturnType = std::invoke_result_t<
        decltype(details::ScopedFunctionInvoker<kNoExcept, WrappedCallableType>::template ActualInvoker<
                 ScopedFunctionInvokerTestScopedFunction<CallableType>>)>;

    static_assert(std::is_same_v<ReturnType, ExpectedCallableType>);
}

}  // namespace

}  // namespace score::safecpp::details
