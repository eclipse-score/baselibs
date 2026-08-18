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
#include "score/language/safecpp/scoped_function/details/scope_state.h"

#include "score/language/safecpp/scoped_function/details/function_wrapper.h"

#include <score/jthread.hpp>
#include <score/stop_token.hpp>

#include <gtest/gtest.h>

#include <future>

namespace score::safecpp::details
{
namespace
{

class ScopeStateTest : public ::testing::Test
{
  protected:
    ScopeState state_{};
};

TEST_F(ScopeStateTest, CanConstructAndDestruct)
{
    ScopeState{};
}

TEST_F(ScopeStateTest, InvokeIfNotExpiredDoesNotCallFunctionIfExpired)
{
    state_.Expire();

    bool called{false};
    auto lambda = [&called]() -> score::cpp::blank {
        called = true;
        return {};
    };
    FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
    const auto result = state_.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_FALSE(called);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ScopeStateTest, InvokeIfNotExpiredCallsFunctionOnlyIfNotYetExpiredWithStopToken)
{
    score::cpp::stop_source stop_source{};
    ScopeState state{stop_source.get_token()};

    std::int32_t called{0};
    auto lambda = [&called]() -> score::cpp::blank {
        called++;
        return {};
    };
    FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
    const auto result1 = state.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_EQ(called, 1);
    EXPECT_TRUE(result1.has_value());

    stop_source.request_stop();

    const auto result2 = state.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_EQ(called, 1);
    EXPECT_FALSE(result2.has_value());
}

TEST_F(ScopeStateTest, InvokeIfNotExpiredCallsFunctionIfNotExpired)
{
    bool called{false};
    auto lambda = [&called]() -> score::cpp::blank {
        called = true;
        return {};
    };
    FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
    const auto result = state_.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_TRUE(called);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ScopeStateTest, InvokeIfNotExpiredReturnsEmptyOptionalIfExpired)
{
    state_.Expire();

    bool called{false};
    auto lambda = [&called]() -> score::cpp::blank {
        called = true;
        return {};
    };
    FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
    const score::cpp::optional<score::cpp::blank> result = state_.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ScopeStateTest, InvokeIfNotExpiredReturnsFilledOptionalIfNotExpired)
{
    bool called{false};
    auto lambda = [&called]() -> score::cpp::blank {
        called = true;
        return {};
    };
    FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
    const score::cpp::optional<score::cpp::blank> result = state_.InvokeIfNotExpired(wrapped_lambda);
    EXPECT_TRUE(result.has_value());
}

TEST_F(ScopeStateTest, FunctionInvocationFinishesBeforeExpiration)
{
    std::promise<void> in_invocation_promise{};
    std::promise<void> after_expiration_promise{};

    score::cpp::jthread invoker{
        [this, &in_invocation_promise, &after_expiration_promise](const score::cpp::stop_token&) {
            auto lambda = [&in_invocation_promise, &after_expiration_promise]() -> score::cpp::blank {
                in_invocation_promise.set_value();
                auto wait_result = after_expiration_promise.get_future().wait_for(std::chrono::milliseconds{10});
                EXPECT_EQ(wait_result, std::future_status::timeout);
                return {};
            };
            FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
            score::cpp::ignore = state_.InvokeIfNotExpired(wrapped_lambda);
        }};
    score::cpp::jthread expirator{
        [this, &in_invocation_promise, &after_expiration_promise](const score::cpp::stop_token&) {
            in_invocation_promise.get_future().wait();
            state_.Expire();
            after_expiration_promise.set_value();
        }};
}

TEST_F(ScopeStateTest, FunctionInvocationFinishesBeforeExpirationWithStopToken)
{
    std::promise<void> in_invocation_promise{};
    std::promise<void> after_expiration_promise{};

    score::cpp::stop_source stop_source{};
    ScopeState state{stop_source.get_token()};

    score::cpp::jthread invoker{
        [&state, &in_invocation_promise, &after_expiration_promise](const score::cpp::stop_token&) {
            auto lambda = [&in_invocation_promise, &after_expiration_promise]() -> score::cpp::blank {
                in_invocation_promise.set_value();
                auto wait_result = after_expiration_promise.get_future().wait_for(std::chrono::milliseconds{10});
                EXPECT_EQ(wait_result, std::future_status::timeout);
                return {};
            };
            FunctionWrapperImpl<decltype(lambda), score::cpp::blank()> wrapped_lambda{lambda};
            score::cpp::ignore = state.InvokeIfNotExpired(wrapped_lambda);
        }};
    score::cpp::jthread expirator{
        [&stop_source, &in_invocation_promise, &after_expiration_promise](const score::cpp::stop_token&) {
            in_invocation_promise.get_future().wait();
            stop_source.request_stop();
            after_expiration_promise.set_value();
        }};
}

}  // namespace
}  // namespace score::safecpp::details
