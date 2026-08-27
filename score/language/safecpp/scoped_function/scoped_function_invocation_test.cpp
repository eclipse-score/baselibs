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
#include "score/language/safecpp/scoped_function/copyable_scoped_function.h"
#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"

#include "score/language/safecpp/scoped_function/details/instrumented_memory_resource.h"
#include "score/language/safecpp/scoped_function/details/parametrization_helper.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include <score/jthread.hpp>
#include <score/memory.hpp>
#include <score/optional.hpp>

#include <gtest/gtest.h>

#include <future>

namespace score::safecpp
{
namespace
{

template <class T>
class ScopedFunctionInvocationTest : public ::testing::Test
{
};

using Types =
    ::testing::Types<ParametrizationHelper<CopyableScopedFunction>, ParametrizationHelper<MoveOnlyScopedFunction>>;
TYPED_TEST_SUITE(ScopedFunctionInvocationTest, Types, /* unused */);

TYPED_TEST(ScopedFunctionInvocationTest, CanCallVoidFunctionFromObject)
{
    bool called{false};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, [&called]() noexcept {
                                called = true;
                            }};

    score::cpp::optional<score::cpp::blank> was_executed = function();

    EXPECT_TRUE(called);
    EXPECT_TRUE(was_executed.has_value());
}

TYPED_TEST(ScopedFunctionInvocationTest, CanCallConstVoidFunctionFromConstObject)
{
    bool called{false};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void() const>;
    const ScopedFunction function{scope, [&called]() noexcept {
                                      called = true;
                                  }};

    score::cpp::optional<score::cpp::blank> was_executed = function();

    EXPECT_TRUE(called);
    EXPECT_TRUE(was_executed.has_value());
}

TYPED_TEST(ScopedFunctionInvocationTest, CanUseWithReturnTypeReference)
{
    bool called{false};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<bool&()>;
    ScopedFunction function{scope, [&called]() noexcept -> bool& {
                                return called;
                            }};

    score::cpp::optional<std::reference_wrapper<bool>> expected_reference = function();
    ASSERT_TRUE(expected_reference.has_value());
    bool& reference = expected_reference.value().get();
    reference = true;

    EXPECT_TRUE(called);
}

TYPED_TEST(ScopedFunctionInvocationTest, CanUseWithReturnTypeInt)
{
    std::int32_t expected_value{19};

    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<std::int32_t()>;
    ScopedFunction function{scope, [expected_value]() noexcept {
                                return expected_value;
                            }};

    score::cpp::optional<std::int32_t> actual_value = function();
    ASSERT_TRUE(actual_value.has_value());
    EXPECT_EQ(actual_value.value(), expected_value);
}

TYPED_TEST(ScopedFunctionInvocationTest, CanUseWithFunctionWithArguments)
{
    std::uint32_t number{0U};
    Scope<> scope{};
    using ScopedFunction = typename TypeParam::template Type<void(std::uint32_t)>;
    ScopedFunction function{scope, [&number](std::uint32_t new_number) noexcept {
                                number = new_number;
                            }};

    std::uint32_t expected_number{15U};
    function(expected_number);

    EXPECT_EQ(number, expected_number);
}

TYPED_TEST(ScopedFunctionInvocationTest, CanSimultaneouslyInvokeFunction)
{
    std::mutex mutex{};
    std::condition_variable cv{};
    std::int32_t counter{0};
    constexpr auto num_threads{2};

    Scope<> scope{};
    std::promise<void> wait_promise{};
    auto future = wait_promise.get_future().share();
    using ScopedFunction = typename TypeParam::template Type<void()>;
    ScopedFunction function{scope, [&mutex, &cv, &counter, future]() {
                                {
                                    std::unique_lock<std::mutex> lock{mutex};
                                    counter++;
                                    cv.notify_all();
                                    cv.wait(lock, [&counter]() {
                                        return counter == num_threads;
                                    });
                                }
                                future.wait();
                            }};

    auto function_invoker = [&function]() {
        function();
    };

    score::cpp::jthread thread_1{function_invoker};

    score::cpp::jthread thread_2{function_invoker};

    wait_promise.set_value();
}

TYPED_TEST(ScopedFunctionInvocationTest, CallOperatorIsNotConstIfTemplateNotConst)
{
    using ScopedFunction = typename TypeParam::template Type<void()>;
    static_assert(std::is_same_v<decltype(&ScopedFunction::operator()),
                                 score::cpp::optional<score::cpp::blank> (
                                     details::Invoker<details::ScopedFunctionInvoker,
                                                      details::CallOperatorInterface,
                                                      ScopedFunction,
                                                      score::cpp::optional<score::cpp::blank>()>::*)()>,
                  "Function must not be marked const");
}

TYPED_TEST(ScopedFunctionInvocationTest, CallOperatorIsConstIfFunctionIsConst)
{
    using ScopedFunction = typename TypeParam::template Type<void() const>;
    static_assert(std::is_same_v<decltype(&ScopedFunction::operator()),
                                 score::cpp::optional<score::cpp::blank> (
                                     details::Invoker<details::ScopedFunctionInvoker,
                                                      details::CallOperatorInterface,
                                                      ScopedFunction,
                                                      score::cpp::optional<score::cpp::blank>() const>::*)() const>,
                  "Function must be marked const");
}

TYPED_TEST(ScopedFunctionInvocationTest, CallOperatorIsNoExceptIfFunctionIsNoExcept)
{
    using ScopedFunction = typename TypeParam::template Type<void() noexcept>;
    static_assert(
        std::is_same_v<decltype(&ScopedFunction::operator()),
                       score::cpp::optional<score::cpp::blank> (
                           details::Invoker<details::ScopedFunctionInvoker,
                                            details::CallOperatorInterface,
                                            ScopedFunction,
                                            score::cpp::optional<score::cpp::blank>() noexcept>::*)() noexcept>,
        "Function must be marked const");
}

}  // namespace
}  // namespace score::safecpp
