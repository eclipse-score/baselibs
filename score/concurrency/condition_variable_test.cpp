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
#include "score/concurrency/condition_variable.h"

#include "score/concurrency/clock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <future>
#include <mutex>
#include <thread>

namespace score
{
namespace concurrency
{

class FakeMutex
{
  public:
    virtual ~FakeMutex() = default;

    virtual void lock()
    {
        mtx_.lock();
    }
    virtual void unlock()
    {
        mtx_.unlock();
    }

  private:
    std::mutex mtx_;
};

class MockMutex : public FakeMutex
{
  public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
};

template <class TMutex>
class ConditionVariableMockTempl
{
  public:
    MOCK_METHOD(void, notify_all, ());
    MOCK_METHOD(void, notify_one, ());
    MOCK_METHOD(void, wait, (std::unique_lock<TMutex>&));
    MOCK_METHOD(std::cv_status,
                wait_until,
                (std::unique_lock<TMutex>&, const std::chrono::time_point<std::chrono::steady_clock>&));
};

using ConditionVariableMock = ConditionVariableMockTempl<MockMutex>;

class InterruptibleConditionalVariableBasicTest : public ::testing::Test
{
  protected:
    ::testing::NiceMock<MockMutex> test_mtx_;
    std::unique_lock<MockMutex> lock_{test_mtx_};
    score::cpp::stop_source stop_source_{};
    score::cpp::stop_token token = stop_source_.get_token();
    ::testing::NiceMock<ConditionVariableMock> mock_cv_;

    void SetUp() override {}
};

TEST_F(InterruptibleConditionalVariableBasicTest, construct_with_non_ref_template_params)
{
    InterruptibleConditionalVariableBasic<MockMutex, ConditionVariableMock> instance{};
}

TEST_F(InterruptibleConditionalVariableBasicTest, construct_with_ref_template_params)
{
    InterruptibleConditionalVariableBasic<MockMutex&, ConditionVariableMock&> instance_ref{test_mtx_, mock_cv_};
}

TEST_F(InterruptibleConditionalVariableBasicTest, wait_until_early_return_on_stop_token_requested)
{
    InterruptibleConditionalVariableBasic<MockMutex&, ConditionVariableMock&> cv{test_mtx_, mock_cv_};

    const auto result =
        cv.wait_until(lock_, stop_source_.get_token(), std::chrono::steady_clock::now(), [this]() -> bool {
            stop_source_.request_stop();
            return false;
        });
    EXPECT_FALSE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest, wait_until_early_return_on_already_gone_timeout)
{
    InterruptibleConditionalVariableBasic<MockMutex&, ConditionVariableMock&> cv{test_mtx_, mock_cv_};

    const auto result = cv.wait_until(lock_,
                                      stop_source_.get_token(),
                                      std::chrono::steady_clock::now() - std::chrono::milliseconds{42},
                                      []() -> bool {
                                          return false;
                                      });
    EXPECT_FALSE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest, cv_test_max_time)
{
    auto timemax = std::chrono::time_point<std::chrono::steady_clock>::max();
    bool stop_waiting = false;

    ON_CALL(mock_cv_, wait(::testing::_)).WillByDefault(::testing::InvokeWithoutArgs([&stop_waiting]() {
        stop_waiting = true;
    }));
    EXPECT_CALL(mock_cv_, wait(::testing::_)).Times(1);

    InterruptibleConditionalVariableBasic<MockMutex&, ConditionVariableMock&> cv{test_mtx_, mock_cv_};

    cv.wait_until(lock_, token, timemax, [&stop_waiting]() -> bool {
        return stop_waiting;
    });
}

TEST_F(InterruptibleConditionalVariableBasicTest, NoEarlySpuriousWakeups)
{
    InterruptibleConditionalVariable unit;

    score::concurrency::testing::SteadyClock::time_point time_point{score::concurrency::testing::SteadyClock::now() +
                                                                    std::chrono::milliseconds{5}};

    std::future<void> future = std::async([&unit, this, time_point]() {
        std::unique_lock<MockMutex> internal_lock{test_mtx_};
        unit.wait_until(internal_lock, stop_source_.get_token(), time_point, []() {
            return false;
        });
    });

    auto status = future.wait_for(std::chrono::milliseconds{10});
    EXPECT_NE(status, std::future_status::ready);

    score::concurrency::testing::SteadyClock::modify_time(std::chrono::milliseconds{5});
    future.wait();
}

TEST_F(InterruptibleConditionalVariableBasicTest, destruction_will_wait_for_all_waiters)
{
    std::mutex helper_mtx;
    std::unique_lock<std::mutex> helper_lock{helper_mtx};
    std::condition_variable helper_cv{};
    std::thread waiting_thread{};
    bool wait_unlock{false};
    std::atomic<bool> destructed{false};

    {
        ::testing::NiceMock<ConditionVariableMock> mock_cv{};
        EXPECT_CALL(mock_cv, wait_until(::testing::_, ::testing::_))
            .WillRepeatedly([&destructed, &wait_unlock, &helper_mtx, &helper_cv](
                                std::unique_lock<MockMutex>&,
                                const std::chrono::time_point<std::chrono::steady_clock>& time_point) {
                std::unique_lock<std::mutex> lock{helper_mtx};
                wait_unlock = true;
                if (lock.owns_lock())
                {
                    lock.unlock();
                }
                helper_cv.notify_all();
                std::this_thread::yield();
                std::this_thread::sleep_until(time_point);
                if (destructed)
                {
                    []() {
                        GTEST_FAIL();
                    }();
                }
                return std::cv_status::timeout;
            });
        using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
        CvType cv(test_mtx_, mock_cv);

        waiting_thread = std::thread{[&cv, &wait_unlock]() {
            std::mutex mtx;
            std::unique_lock<std::mutex> lock{mtx};
            score::cpp::stop_source source{};
            cv.wait_for(lock, source.get_token(), std::chrono::milliseconds{100}, [&wait_unlock]() {
                return wait_unlock;
            });
        }};

        helper_cv.wait(helper_lock, [&wait_unlock]() {
            return wait_unlock;
        });
    }
    destructed = true;
    waiting_thread.join();
}

TEST_F(InterruptibleConditionalVariableBasicTest, NotifyOneInvokesNotifyOneOnLockedInternalMutex)
{
    EXPECT_CALL(test_mtx_, lock()).Times(1);
    EXPECT_CALL(mock_cv_, notify_one()).Times(1);
    // the mutex is locked once in the test setup and hence checking if unlock is called twice.
    EXPECT_CALL(test_mtx_, unlock()).Times(2);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    MockMutex internal_mutex;
    CvType cv(internal_mutex, mock_cv_);

    std::unique_lock<MockMutex> lock{test_mtx_};
    cv.notify_one();
}

TEST_F(InterruptibleConditionalVariableBasicTest, NotifyAllInvokesNotifyAllOnLockedInternalMutex)
{
    EXPECT_CALL(test_mtx_, lock()).Times(1);
    EXPECT_CALL(mock_cv_, notify_all()).Times(1);
    // the mutex is locked once in the test setup and hence checking if unlock is called twice.
    EXPECT_CALL(test_mtx_, unlock()).Times(2);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    MockMutex internal_mutex;
    CvType cv(internal_mutex, mock_cv_);

    std::unique_lock<MockMutex> lock{test_mtx_};
    cv.notify_all();
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitNotifiesAllWaitingThreadsIfStopIsRequestedDuringWait)
{
    bool stop_waiting = false;

    EXPECT_CALL(mock_cv_, wait(::testing::_))
        .WillOnce(::testing::Invoke([this, stop_waiting](std::unique_lock<MockMutex>& received_lock) {
            received_lock.unlock();
            stop_source_.request_stop();
            return stop_waiting;
        }));

    EXPECT_CALL(mock_cv_, notify_all()).Times(1);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    bool result = cv.wait(lock_, token, [] {
        return false;
    });
    EXPECT_FALSE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitUntilNotifiesAllWaitingThreadsIfStopIsRequestedDuringWait)
{
    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);

    EXPECT_CALL(mock_cv_, wait_until(::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([this](std::unique_lock<MockMutex>& received_lock, const auto&) {
            received_lock.unlock();
            stop_source_.request_stop();
            return std::cv_status::no_timeout;
        }));

    EXPECT_CALL(mock_cv_, notify_all()).Times(1);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    std::cv_status result = cv.wait_until(lock_, token, abs_time);
    EXPECT_EQ(result, std::cv_status::no_timeout);
}

TEST_F(InterruptibleConditionalVariableBasicTest,
       WaitUntilWithPredicateNotifiesAllWaitingThreadsIfStopIsRequestedDuringWait)
{
    bool stop_waiting = false;
    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

    EXPECT_CALL(mock_cv_, wait_until(::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([this](std::unique_lock<MockMutex>& received_lock, const auto&) {
            received_lock.unlock();
            stop_source_.request_stop();
            return std::cv_status::no_timeout;
        }));

    EXPECT_CALL(mock_cv_, notify_all()).Times(1);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    bool result = cv.wait_until(lock_, token, abs_time, [&stop_waiting] {
        return stop_waiting;
    });
    EXPECT_FALSE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitImmediatelyReturnsTrueWithPredicate)
{
    ::testing::MockFunction<bool(void)> wait_stop_callback;
    EXPECT_CALL(wait_stop_callback, Call())
        .Times(2)
        // On the first invocation, the predicate returns False, causing the wait_until to enter the loop.
        .WillOnce(::testing::Return(false))
        // On 2nd invocation, the predicate returns True, causing the wait_until to exit the loop and return True.
        .WillOnce(::testing::Return(true));

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    bool result = cv.wait(lock_, token, wait_stop_callback.AsStdFunction());
    EXPECT_TRUE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitUntilImmediatelyReturnsTrueWithPredicate)
{
    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);

    ::testing::MockFunction<bool(void)> wait_stop_callback;
    EXPECT_CALL(wait_stop_callback, Call())
        .Times(2)
        // On the first invocation, the predicate returns False, causing the wait_until to enter the loop.
        .WillOnce(::testing::Return(false))
        // On 2nd invocation, the predicate returns True, causing the wait_until to exit the loop and return True.
        .WillOnce(::testing::Return(true));

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    bool result = cv.wait_until(lock_, token, abs_time, wait_stop_callback.AsStdFunction());
    EXPECT_TRUE(result);
}

TEST_F(InterruptibleConditionalVariableBasicTest,
       WaitUntilReturnsNoTimeoutWithoutWaitingWhenStopRequestedBeforeWaitStarts)
{
    stop_source_.request_stop();
    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);

    EXPECT_CALL(mock_cv_, wait_until(::testing::_, ::testing::_)).Times(0);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    std::cv_status result = cv.wait_until(lock_, token, abs_time);
    EXPECT_EQ(result, std::cv_status::no_timeout);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitUntilReturnsTimeoutForImmediateTimeout)
{
    auto past_time = std::chrono::steady_clock::now() - std::chrono::seconds(1);

    EXPECT_CALL(mock_cv_, wait_until(::testing::_, ::testing::_)).Times(0);

    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType cv(test_mtx_, mock_cv_);
    std::cv_status result = cv.wait_until(lock_, token, past_time);
    EXPECT_EQ(result, std::cv_status::timeout);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitUntilImmediatelyReturnIfStopIsRequesteBeforeLocking)
{
    ON_CALL(test_mtx_, lock()).WillByDefault(::testing::Invoke([this]() {
        stop_source_.request_stop();
    }));

    EXPECT_CALL(mock_cv_, wait_until(::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([this](std::unique_lock<MockMutex>& received_lock, const auto&) {
            received_lock.unlock();
            stop_source_.request_stop();
            return std::cv_status::no_timeout;
        }));

    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);
    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    MockMutex internal_mutex;
    CvType cv(internal_mutex, mock_cv_);
    std::cv_status result = cv.wait_until(lock_, token, abs_time);

    EXPECT_EQ(result, std::cv_status::no_timeout);
}

TEST_F(InterruptibleConditionalVariableBasicTest, WaitUntilImmediatelyReturnIfStopIsRequesteAfterLocking)
{
    // This test verifies that the conditional variable lock is immediately released in wait_until_impl method
    // when the stop token is requested, ensuring proper handling of stop requests.
    std::unique_lock lock{test_mtx_};

    MockMutex internal_mtx;
    using CvType = InterruptibleConditionalVariableBasic<MockMutex&, ::testing::NiceMock<ConditionVariableMock>&>;
    CvType unit_cv{internal_mtx, mock_cv_};
    const auto abs_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);

    EXPECT_CALL(internal_mtx, lock())
        .WillOnce(::testing::Invoke([this]() {
            this->stop_source_.request_stop();
        }))
        .WillOnce(::testing::Return());
    EXPECT_CALL(internal_mtx, unlock()).Times(2);

    std::cv_status result = unit_cv.wait_until(lock, token, abs_time);

    EXPECT_EQ(result, std::cv_status::no_timeout);
}

}  // namespace concurrency
}  // namespace score
