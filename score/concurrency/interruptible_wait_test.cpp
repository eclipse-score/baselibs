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
#include "score/concurrency/interruptible_wait.h"

#include "gtest/gtest.h"

#include <chrono>
#include <future>
#include <thread>

namespace score
{
namespace concurrency
{
namespace
{

using namespace std::chrono_literals;

TEST(InterruptibleWait, wait_until_exit_requested_exits_on_stop_requsted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__interruptible_wait");
    RecordProperty("Description", "Check that wait_until_stop_requested() blocks until a stop is requested.");

    // Given stop_source
    std::promise<void> promise{};
    auto f = promise.get_future();
    score::cpp::stop_source stop_source{};

    // When thread is conditionally waiting on token
    std::thread waiting_thread{[token = stop_source.get_token(), promise = std::move(promise)]() mutable {
        wait_until_stop_requested(token);
        promise.set_value();
    }};

    // Then wait_until_stop_requested exits on stop_request

    // In case of succeess on next line, it does not guarantee that it will not unblock
    // before stop_requested on the stop_source. However if this is failing in means that there is a bug,
    // because it was unblocked before the stop request and that is for sure bug in implementation.
    ASSERT_EQ(f.wait_for(50ms), std::future_status::timeout);
    stop_source.request_stop();
    f.wait();

    waiting_thread.join();
}

TEST(InterruptibleWait, wait_for_returns_true_on_stop_requsted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__interruptible_wait");
    RecordProperty("Description", "Check that wait_for() returns true early if a stop is requested during the wait.");

    // Given stop_source and timeout
    std::promise<void> promise{};
    auto f = promise.get_future();
    score::cpp::stop_source stop_source{};
    // Given timeout is only for testing purposes, this could be any time bigger than test framework timeout.
    constexpr auto timeout = 1h;

    // When thread is conditionally waiting on token or expired timeout
    std::thread waiting_thread{
        [token = stop_source.get_token(), timeout = timeout, promise = std::move(promise)]() mutable {
            ASSERT_TRUE(wait_for(token, timeout));
            promise.set_value();
        }};

    // Then wait_for exits on stop_request

    // In case of succeess on next line, it does not guarantee that it will not unblock
    // before stop_requested on the stop_source. However if this is failing in means that there is a bug,
    // because it was unblocked before the stop request and that is for sure bug in implementation.
    ASSERT_EQ(f.wait_for(50ms), std::future_status::timeout);
    stop_source.request_stop();
    // We can't test how much time wait_for was really waiting, instead we are testing that waiting stops.
    f.wait();

    waiting_thread.join();
}

TEST(InterruptibleWait, wait_for_returns_false_on_not_stop_requsted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__interruptible_wait");
    RecordProperty("Description",
                   "Check that wait_for() returns false after the timeout elapsed without a stop request.");

    // Given stop_source and timeout
    std::promise<void> promise{};
    auto f = promise.get_future();
    score::cpp::stop_source stop_source{};
    // Given timeout is only for testing purposes
    constexpr auto timeout = 50ms;

    // When thread is conditionally waiting on token or expired timeout
    std::thread waiting_thread{
        [token = stop_source.get_token(), timeout = timeout, promise = std::move(promise)]() mutable {
            ASSERT_FALSE(wait_for(token, timeout));
            promise.set_value();
        }};

    // Then wait_for stops blocking after expired timeout.

    // We can't test that wait_for was waiting exactly specified time, since that depends on OS scheduler,
    // instead we are testing that waiting stops.
    f.wait();

    waiting_thread.join();
}

TEST(InterruptibleWait, wait_until_returns_true_on_stop_requsted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__interruptible_wait");
    RecordProperty("Description", "Check that wait_until() returns true early if a stop is requested during the wait.");

    // Given stop_source and time_point
    std::promise<void> promise{};
    auto f = promise.get_future();
    score::cpp::stop_source stop_source{};
    // Given timeout is only for testing purposes, it could be any value
    constexpr auto time = 1h;
    auto time_point = std::chrono::steady_clock::now() + time;

    // When thread is conditionally waiting on token or expired timeout
    std::thread waiting_thread{
        [token = stop_source.get_token(), time_point = std::move(time_point), promise = std::move(promise)]() mutable {
            ASSERT_TRUE(wait_until(token, time_point));
            promise.set_value();
        }};

    // Then wait_until exits on stop_request

    // In case of succeess on next line, it does not guarantee that it will not unblock
    // before stop_requested on the stop_source. However if this is failing in means that there is a bug,
    // because it was unblocked before the stop request and that is for sure bug in implementation.
    ASSERT_EQ(f.wait_for(50ms), std::future_status::timeout);
    stop_source.request_stop();
    // We can't test how much time wait_until was really waiting before interuption, instead we are testing that waiting
    // stops.
    f.wait();

    waiting_thread.join();
}

TEST(InterruptibleWait, wait_until_returns_false_on_not_stop_requsted)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("PartiallyVerifies", "comp_req__concurrency__interruptible_wait");
    RecordProperty("Description",
                   "Check that wait_until() returns false after the time point was reached without a stop request.");

    // Given stop_source and time_point
    std::promise<void> promise{};
    auto f = promise.get_future();
    score::cpp::stop_source stop_source{};
    // Given timeout is only for testing purposes, it could be any value
    constexpr auto time = 100ms;
    auto time_point = std::chrono::steady_clock::now() + time;

    // When thread is conditionally waiting on token or expired timeout
    std::thread waiting_thread{
        [token = stop_source.get_token(), time_point = std::move(time_point), promise = std::move(promise)]() mutable {
            ASSERT_FALSE(wait_until(token, time_point));
            promise.set_value();
        }};

    // Then wait_until stops blocking after time_point being reached.

    // We can't test that wait_until was waiting exactly until specified time_point, since that depends on OS scheduler,
    // instead we are testing that waiting stops.
    f.wait();

    waiting_thread.join();
}

}  // namespace
}  // namespace concurrency
}  // namespace score
