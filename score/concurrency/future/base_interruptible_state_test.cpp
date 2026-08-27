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
#include "score/concurrency/future/base_interruptible_state.h"
#include "score/concurrency/interruptible_wait.h"

#include "score/stop_token.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <future>

namespace score
{
namespace concurrency
{
namespace detail
{
namespace
{

class StubInterruptibleConditionalVariable
{
  public:
    MOCK_METHOD(void, notify_one, (), (noexcept));
    MOCK_METHOD(void, notify_all, (), (noexcept));

    MOCK_METHOD(bool, InternalWait, (), (noexcept));

    template <class Lockable>
    bool InternalWaitWrapper(Lockable& lock) noexcept
    {
        lock.unlock();
        bool result = InternalWait();
        lock.lock();
        return result;
    }

    template <class Lockable, class Predicate>
    bool wait(Lockable& lock, const score::cpp::stop_token&, Predicate)
    {
        return InternalWaitWrapper(lock);
    }

    template <class Lockable, class Clock, class Duration, class Predicate>
    bool wait_until(Lockable& lock,
                    const score::cpp::stop_token&,
                    const std::chrono::time_point<Clock, Duration>&,
                    Predicate)
    {
        return InternalWaitWrapper(lock);
    }

    template <class Lockable, class Rep, class Period, class Predicate>
    bool wait_for(Lockable& lock, const score::cpp::stop_token&, const std::chrono::duration<Rep, Period>&, Predicate)
    {
        return InternalWaitWrapper(lock);
    }
};

class StubLockable
{
  public:
    MOCK_METHOD(void, OnLocked, (), ());

    void lock()
    {
        internal_lock_.lock();
        OnLocked();
    }

    void unlock()
    {
        internal_lock_.unlock();
    }

    std::mutex& GetLockable()
    {
        return internal_lock_;
    };

  private:
    std::mutex internal_lock_{};
};

class BaseInterruptibleStateTest : public ::testing::Test
{
  protected:
    ::testing::NiceMock<StubLockable> mock_ready_lockable_{};
    ::testing::NiceMock<StubLockable> mock_callback_lockable_{};
    ::testing::NiceMock<StubInterruptibleConditionalVariable> mock_interruptible_condition_variable_{};
    score::concurrency::detail::BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                                       ::testing::NiceMock<StubInterruptibleConditionalVariable>&>
        unit_{mock_ready_lockable_, mock_callback_lockable_, mock_interruptible_condition_variable_};
    bool invoked_{false};

    void InstallCallback()
    {
        unit_.WithOnAbort([this]() noexcept {
            invoked_ = true;
        });
    }

    void TestWaitReturnsWhenStateReady(
        score::cpp::callback<score::cpp::expected_blank<Error>(
            BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                   ::testing::NiceMock<StubInterruptibleConditionalVariable>&>&)> strategy) noexcept
    {
        std::mutex mutex_1{};
        std::condition_variable condition_variable_1{};
        bool condition_1{false};

        std::mutex mutex_2{};
        std::condition_variable condition_variable_2{};
        bool condition_2{false};

        ::testing::NiceMock<StubInterruptibleConditionalVariable> interruptible_condition_variable{};
        ON_CALL(interruptible_condition_variable, InternalWait())
            .WillByDefault(
                [&mutex_1, &condition_variable_1, &condition_1, &mutex_2, &condition_variable_2, &condition_2]() {
                    std::unique_lock<std::mutex> cb_lock_2{mutex_2};
                    {
                        std::lock_guard<std::mutex> cb_lock_1{mutex_1};
                        condition_1 = true;
                        condition_variable_1.notify_all();
                    }
                    condition_variable_2.wait(cb_lock_2, [&condition_2]() {
                        return condition_2;
                    });

                    return true;
                });
        auto interruptible_state =
            std::make_shared<BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                                    ::testing::NiceMock<StubInterruptibleConditionalVariable>&>>(
                mock_ready_lockable_, mock_callback_lockable_, interruptible_condition_variable);

        std::future<score::cpp::expected_blank<Error>> async_future{};
        {
            std::unique_lock<std::mutex> lock_1{mutex_1};

            async_future = std::async([&interruptible_state, &strategy]() noexcept {
                return strategy(*interruptible_state);
            });

            condition_variable_1.wait(lock_1, [&condition_1]() {
                return condition_1;
            });
            interruptible_state->MakeReady();
        }

        {
            std::lock_guard<std::mutex> lock_2{mutex_2};
            condition_2 = true;
            condition_variable_2.notify_all();
        }

        EXPECT_TRUE(async_future.get().has_value());
    }
};

TEST_F(BaseInterruptibleStateTest, Destruction)
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
    auto heap_base_state_mock_cv =
        std::make_unique<BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                                ::testing::NiceMock<StubInterruptibleConditionalVariable>&>>(
            mock_ready_lockable_, mock_callback_lockable_, mock_interruptible_condition_variable_);
    heap_base_state_mock_cv.reset();

    auto heap_base_state_real_cv =
        std::make_unique<BaseInterruptibleState<std::mutex, InterruptibleConditionalVariable>>();
    heap_base_state_real_cv.reset();

    // Required to cover D1 destructors
    {
        BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                               ::testing::NiceMock<StubInterruptibleConditionalVariable>&>
            stack_base_state_mock_cv{
                mock_ready_lockable_, mock_callback_lockable_, mock_interruptible_condition_variable_};
        (void)stack_base_state_mock_cv;
    }

    {
        BaseInterruptibleState<std::mutex, InterruptibleConditionalVariable> stack_base_state_real_cv{};
        (void)stack_base_state_real_cv;
    }

    // Required to cover D2 destructor
    std::shared_ptr<BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                           ::testing::NiceMock<StubInterruptibleConditionalVariable>&>>
        shared_heap_base_state_mock_cv;
    {
        shared_heap_base_state_mock_cv =
            std::make_shared<BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                                    ::testing::NiceMock<StubInterruptibleConditionalVariable>&>>(
                mock_ready_lockable_, mock_callback_lockable_, mock_interruptible_condition_variable_);
    }
    shared_heap_base_state_mock_cv.reset();

    std::shared_ptr<BaseInterruptibleState<std::mutex, InterruptibleConditionalVariable>>
        shared_heap_base_state_real_cv;
    {
        shared_heap_base_state_real_cv =
            std::make_shared<BaseInterruptibleState<std::mutex, InterruptibleConditionalVariable>>();
    }
    shared_heap_base_state_real_cv.reset();
}

TEST_F(BaseInterruptibleStateTest, WaitReturnsSuccessfullyWhenStateBecomesReadyDuringWait)
{
    TestWaitReturnsWhenStateReady(
        [](BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                  ::testing::NiceMock<StubInterruptibleConditionalVariable>&>& state) noexcept {
            score::cpp::stop_token stop_token{};
            return state.Wait(stop_token);
        });
}

TEST_F(BaseInterruptibleStateTest, WaitReturnsSuccessfullyWhenStateBecomesReadyAtThreadExit)
{
    TestWaitReturnsWhenStateReady(
        [](BaseInterruptibleState<::testing::NiceMock<StubLockable>&,
                                  ::testing::NiceMock<StubInterruptibleConditionalVariable>&>& state) noexcept {
            score::cpp::stop_token stop_token{};
            return state.Wait(stop_token);
        });
}

TEST_F(BaseInterruptibleStateTest, WaitReturnsWhenStateBecomesReadyRightBeforeInteralWaitIsCalled)
{
    bool triggered{false};
    ON_CALL(mock_ready_lockable_, OnLocked()).WillByDefault([this, &triggered]() {
        if (!triggered)
        {
            triggered = true;
            mock_ready_lockable_.GetLockable().unlock();
            unit_.MakeReady();
            mock_ready_lockable_.GetLockable().lock();
        }
    });

    score::cpp::stop_token stop_token{};
    unit_.Wait(stop_token);
}

TEST_F(BaseInterruptibleStateTest, NotReadyWillNotInvokeCallback)
{
    // Given a shared state, with no futures registered

    // When installing the callback
    InstallCallback();

    // Then the callback is not invoked
    EXPECT_FALSE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, ReadyWithoutFutureWillNotInvokeCallback)
{
    // Given a shared state that was marked ready
    unit_.MakeReady();

    // When installing a callback
    InstallCallback();

    // Then the callback is not invoked
    EXPECT_FALSE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, ReadyWithFutureWillNotInvokeCallback)
{
    // Given a shared state with a future and a promise that was marked ready
    unit_.RegisterFuture();
    unit_.MakeReady();

    // When installing a callback
    InstallCallback();

    // Then the callback is not invoked
    EXPECT_FALSE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, ReadyWithFutureDestructionWillNotInvokeCallback)
{
    // Given a shared state with installed callback
    InstallCallback();

    // When creating a future, setting the promise and destroying the future
    unit_.RegisterFuture();
    unit_.MakeReady();
    unit_.UnregisterFuture();

    // Then the callback shall not be invoked
    EXPECT_FALSE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, ReadyWithFutureDestructionWillInvokeCallback)
{
    // Given a shared state with installed callback
    InstallCallback();

    // When creating a future and destroying the future (promise is not set)
    unit_.RegisterFuture();
    unit_.UnregisterFuture();

    // Then the callback shall not be invoked
    EXPECT_TRUE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, TerminateOnMismatchOfReferenceCounts)
{
    // Give a shared state without callback

    // When registering a future and unregistering it
    unit_.RegisterFuture();
    unit_.UnregisterFuture();

    // Terminate shall happen on additional unregister
    EXPECT_EXIT(unit_.UnregisterFuture(), ::testing::KilledBySignal(SIGABRT), "");
}

TEST_F(BaseInterruptibleStateTest, UnregisterInDifferentThreads)
{
    InstallCallback();

    unit_.RegisterFuture();

    auto async_future = std::async([this]() noexcept {
        unit_.RegisterFuture();
        unit_.UnregisterFuture();
    });

    unit_.UnregisterFuture();

    async_future.wait();

    EXPECT_TRUE(invoked_);
}

TEST_F(BaseInterruptibleStateTest, RegisterInDifferentThreads)
{
    InstallCallback();

    auto async_future = std::async([this]() noexcept {
        unit_.RegisterFuture();
    });
    auto async_future2 = std::async([this]() noexcept {
        unit_.RegisterFuture();
    });

    unit_.RegisterFuture();

    async_future.wait();
    async_future2.wait();
}

TEST_F(BaseInterruptibleStateTest, InstallCallbackAndRegisterUnregisterInDifferentThreads)
{
    auto async_future = std::async([this]() noexcept {
        unit_.RegisterFuture();
        unit_.UnregisterFuture();
    });

    auto async_future2 = std::async([this]() noexcept {
        InstallCallback();
    });

    async_future.wait();
    async_future2.wait();
}

}  // namespace
}  // namespace detail
}  // namespace concurrency
}  // namespace score
