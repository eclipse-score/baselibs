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
#include "score/concurrency/periodic_task.h"

#include "score/concurrency/clock.h"
#include "score/concurrency/interruptible_conditional_variable_mock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdint>
#include <future>
#include <memory>
#include <utility>

namespace score
{
namespace concurrency
{
namespace
{

class Invokable
{
  public:
    MOCK_METHOD(void,
                Invoke,
                (score::cpp::stop_token, const score::concurrency::testing::SteadyClock::time_point),
                (const));
    MOCK_METHOD(void,
                Invoke,
                (score::cpp::stop_token, const score::concurrency::testing::SteadyClock::time_point, int, double),
                (const));
};

class PeriodicTaskTest : public ::testing::Test
{
  protected:
    Invokable invokable_{};
    score::concurrency::testing::SteadyClock::time_point first_execution_{
        score::concurrency::testing::SteadyClock::now()};
    score::concurrency::testing::SteadyClock::duration interval_{std::chrono::milliseconds{100}};
    std::promise<void> promise_{};
    score::cpp::stop_source stop_source_{};
};

TEST_F(PeriodicTaskTest, ConstructionAndDestruction)
{
    auto unique_task = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept {});
    unique_task.reset();
}

TEST_F(PeriodicTaskTest, ConstructionAndDestructionOnHeapWithBasePointer)
{
    score::cpp::pmr::unique_ptr<Task> unique_task = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept {});
    unique_task.reset();
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningVoid)
{
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) -> void {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([this]() {
        score::concurrency::testing::SteadyClock::modify_time(interval_);
    }));
    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_ + interval_))
        .WillOnce(::testing::InvokeWithoutArgs([&unit]() {
            unit->GetStopSource().request_stop();
        }));

    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningNonVoid)
{
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) -> std::uint32_t {
            invokable_.Invoke(std::move(stop_token), time_point);
            return 2;
        });

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([this]() {
        score::concurrency::testing::SteadyClock::modify_time(interval_);
    }));
    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_ + interval_))
        .WillOnce(::testing::InvokeWithoutArgs([&unit]() {
            unit->GetStopSource().request_stop();
        }));

    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackReturningBoolUntilReturnsFalse)
{
    bool return_value{true};
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this, &return_value](score::cpp::stop_token stop_token,
                              const score::concurrency::testing::SteadyClock::time_point time_point) -> bool {
            invokable_.Invoke(std::move(stop_token), time_point);
            return return_value;
        });

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([this]() {
        score::concurrency::testing::SteadyClock::modify_time(interval_);
    }));
    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_ + interval_))
        .WillOnce(::testing::InvokeWithoutArgs([&return_value]() {
            return_value = false;
        }));

    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, ExecutesCallbackWithCorrectAttributes)
{
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([&unit]() {
        unit->GetStopSource().request_stop();
    }));

    (*unit)(unit->GetStopSource().get_token());
}

TEST_F(PeriodicTaskTest, WillRunMultipleIterations)
{
    testing::InterruptibleConditionalVariableMock mock_cv{};
    // Given a manually created PeriodicTask with a void callback
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    ::testing::InSequence sequence{};
    // First iteration
    auto expected_time_point = first_execution_;
    EXPECT_CALL(mock_cv, wait_until(::testing::_, unit->GetStopSource().get_token(), expected_time_point, ::testing::_))
        .WillOnce(::testing::InvokeWithoutArgs([this]() {
            score::concurrency::testing::SteadyClock::modify_time(interval_);
            return true;
        }));
    EXPECT_CALL(invokable_, Invoke(::testing::_, expected_time_point));

    // Second iteration
    expected_time_point += interval_;
    EXPECT_CALL(mock_cv, wait_until(::testing::_, unit->GetStopSource().get_token(), expected_time_point, ::testing::_))
        .WillOnce(::testing::InvokeWithoutArgs([this]() {
            score::concurrency::testing::SteadyClock::modify_time(interval_);
            return true;
        }));
    EXPECT_CALL(invokable_, Invoke(::testing::_, expected_time_point));

    // Third iteration
    expected_time_point += interval_;
    EXPECT_CALL(mock_cv,
                wait_until(::testing::_, unit->GetStopSource().get_token(), expected_time_point, ::testing::_));
    EXPECT_CALL(invokable_, Invoke(::testing::_, expected_time_point)).WillOnce(::testing::InvokeWithoutArgs([&unit]() {
        unit->GetStopSource().request_stop();
    }));

    // When executing the function call operator
    (*unit)(unit->GetStopSource().get_token(), mock_cv);
}

TEST_F(PeriodicTaskTest, WillInvokeAtLeastOnce)
{
    testing::InterruptibleConditionalVariableMock mock_cv{};
    // Given a manually created PeriodicTask with a void callback
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    // That is already stopped
    unit->GetStopSource().request_stop();

    // Expect to be invoked once
    EXPECT_CALL(invokable_, Invoke(::testing::_, score::concurrency::testing::SteadyClock::now()));

    // When executing the function call operator
    (*unit)(unit->GetStopSource().get_token(), mock_cv);
}

TEST_F(PeriodicTaskTest, DoNotWaitIfStartExecutionSetToPast)
{
    score::concurrency::testing::SteadyClock::modify_time(interval_);

    testing::InterruptibleConditionalVariableMock mock_cv{};
    // Given a manually created PeriodicTask with a void callback
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](score::cpp::stop_token stop_token,
               const score::concurrency::testing::SteadyClock::time_point time_point) {
            invokable_.Invoke(std::move(stop_token), time_point);
        });

    // That is already stopped
    unit->GetStopSource().request_stop();

    // Expect to be invoked once
    EXPECT_CALL(invokable_, Invoke(::testing::_, ::testing::_));

    // Expect the task to skip waiting since the start execution time is in the past
    EXPECT_CALL(mock_cv, wait_until(::testing::_, ::testing::_, ::testing::_, ::testing::_)).Times(0);

    // When executing the function call operator
    (*unit)(unit->GetStopSource().get_token(), mock_cv);
}

TEST_F(PeriodicTaskTest, ExecutesCallbackWithParameter)
{
    std::int32_t observer{0};
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this, &observer](score::cpp::stop_token stop_token,
                          const score::concurrency::testing::SteadyClock::time_point time_point,
                          const std::int32_t a) -> void {
            observer = a;
            invokable_.Invoke(std::move(stop_token), time_point);
        },
        42);

    EXPECT_CALL(invokable_, Invoke(::testing::_, first_execution_)).WillOnce(::testing::InvokeWithoutArgs([&unit]() {
        unit->GetStopSource().request_stop();
    }));

    (*unit)(unit->GetStopSource().get_token());

    EXPECT_EQ(observer, 42);
}

TEST_F(PeriodicTaskTest, MakeWithTaskResultLeadsToAssociatedTaskAndTaskResult)
{
    // Given a task created using the make_* function
    auto unit = PeriodicTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept -> void {
        });

    // When executing the function call operator
    unit.second->GetStopSource().request_stop();
    (*unit.second)(unit.second->GetStopSource().get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

TEST_F(PeriodicTaskTest, WillTerminateWhenCalculationOfTimePointForNextExecutionWouldOverflow)
{
    first_execution_ = score::concurrency::testing::SteadyClock::time_point::max() - std::chrono::milliseconds{1};
    score::concurrency::testing::SteadyClock::modify_time(first_execution_ -
                                                          score::concurrency::testing::SteadyClock::now());
    auto unit = PeriodicTaskFactory::Make<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [](const score::cpp::stop_token&, const score::concurrency::testing::SteadyClock::time_point) noexcept -> void {
        });

    EXPECT_EXIT((*unit)(unit->GetStopSource().get_token()), ::testing::KilledBySignal(SIGABRT), "");
}

TEST_F(PeriodicTaskTest, MakeWithTaskResultCapturesAllArguments)
{
    int arg1 = 42;
    double arg2 = 3.14;

    EXPECT_CALL(invokable_, Invoke(stop_source_.get_token(), first_execution_, arg1, arg2)).Times(1);

    auto unit = PeriodicTaskFactory::MakeWithTaskResult<score::concurrency::testing::SteadyClock>(
        score::cpp::pmr::get_default_resource(),
        first_execution_,
        interval_,
        [this](const score::cpp::stop_token stop,
               const score::concurrency::testing::SteadyClock::time_point time,
               int param1,
               double param2) {
            invokable_.Invoke(stop, time, param1, param2);
        },
        arg1,
        arg2);

    // When executing the function call operator
    stop_source_.request_stop();
    (*unit.second)(stop_source_.get_token());

    // That the future is valid and can be retrieved
    ASSERT_TRUE(unit.first.Valid());
    EXPECT_TRUE(unit.first.Get());
}

}  // namespace
}  // namespace concurrency
}  // namespace score
