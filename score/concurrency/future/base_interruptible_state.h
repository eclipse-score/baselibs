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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_STATE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_STATE_H

#include "score/concurrency/future/error.h"
#include "score/result/result.h"

#include "score/callback.hpp"
#include "score/expected.hpp"
#include "score/stop_token.hpp"

#include <atomic>
#include <mutex>
#include <optional>
#include <type_traits>

namespace score
{
namespace concurrency
{
namespace detail
{

template <typename Lockable, typename CV>
class BaseInterruptibleState
{
  public:
    // It is not feasible to create a delegating constructor without generating an unnecessary default object
    // for the constructor that accepts references.
    // coverity[autosar_cpp14_a12_1_5_violation]
    BaseInterruptibleState()
        : ready_{false},
          ready_mutex_{},
          ready_conditional_variable_{},
          value_set_{false},
          callback_mutex_{},
          on_abort_{},
          reference_count_{0}
    {
    }

    explicit BaseInterruptibleState(Lockable& ready_mutex, Lockable& callback_mutex, CV& cv) noexcept
        : ready_{false},
          ready_mutex_{ready_mutex},
          ready_conditional_variable_{cv},
          value_set_{false},
          callback_mutex_{callback_mutex},
          on_abort_{},
          reference_count_{0}
    {
    }

    BaseInterruptibleState(const BaseInterruptibleState&) = delete;

    BaseInterruptibleState& operator=(const BaseInterruptibleState&) = delete;

    BaseInterruptibleState(BaseInterruptibleState&&) = delete;

    BaseInterruptibleState& operator=(BaseInterruptibleState&&) noexcept = delete;

    virtual ~BaseInterruptibleState() = default;

    void MakeReady() noexcept
    {
        [[maybe_unused]] std::lock_guard<std::remove_reference_t<Lockable>> lock{ready_mutex_};
        ready_ = true;
        ActualSetStateReady();
    }

    score::cpp::expected_blank<Error> Wait(const score::cpp::stop_token& stop_token) noexcept
    {
        return WaitByStrategy(
            [this, &stop_token](auto& lock, auto predicate) {
                return ready_conditional_variable_.wait(lock, stop_token, std::move(predicate));
            },
            stop_token);
    }

    template <class Rep, class Period>
    score::cpp::expected_blank<Error> WaitFor(const score::cpp::stop_token& stop_token,
                                              const std::chrono::duration<Rep, Period>& rel_time) noexcept
    {
        return WaitByStrategy(
            [this, &stop_token, &rel_time](auto& lock, auto predicate) {
                return ready_conditional_variable_.wait_for(lock, stop_token, rel_time, std::move(predicate));
            },
            stop_token);
    }

    template <class Clock, class Duration>
    score::cpp::expected_blank<Error> WaitUntil(const score::cpp::stop_token& stop_token,
                                                const std::chrono::time_point<Clock, Duration>& abs_time) noexcept
    {
        return WaitByStrategy(
            [this, &stop_token, &abs_time](auto& lock, auto predicate) {
                return ready_conditional_variable_.wait_until(lock, stop_token, abs_time, std::move(predicate));
            },
            stop_token);
    }

    /// @brief On future construction ref-count is increased
    void RegisterFuture() noexcept
    {
        // Justification for coverage exclusion below:
        // Termination happens when an overflow occurs in the std::atomic<std::int32_t>-typed counter. This occurs when
        // 2147483647 futures are already registered to one shared state and an additional future shall be registered.
        // Such a high number of futures is unlikely to ever share a single state, since a future is only relevant to
        // somebody that wants to consume the promise. A promise relevant to 2147483648 parties is highly unlikely.
        // Nevertheless, we catch such overflows by checking for negative numbers in the counter that is ensured to only
        // ever be positive in normal operation. The stdlib specifies that overflows of std::atomic<std::int32_t> are
        // not UB. In later C++ versions they further clarify that operations wrap around. Hence, a post-check whether
        // the incrementation resulted in a negative number will catch overflows.
        static_assert(std::is_same<std::atomic<std::int32_t>, decltype(reference_count_)>::value, "");

        // The rule against increment and decrement operators mixed with others on the same line seems to be more
        // applicable to arithmetic operators. Here the only other operator is the member access operator (i.e. the dot
        // operator) and it in no way confusing to combine it with the decriment operator. This rule is also absent from
        // the new MISRA 2023 standart, thus it is in general reasonable to deviate from it.
        // coverity[autosar_cpp14_m5_2_10_violation]
        const auto new_reference_count = ++reference_count_;
        // GCOV_EXCL_START: Decision 'true' is not taken. Forcing an overflow is not feasible.
        if (new_reference_count < 0)
        // GCOV_EXCL_STOP
        {
            // LCOV_EXCL_START
            // Could only be covered by forcing an overflow which is not feasible because of CI load
            std::terminate();
            // LCOV_EXCL_STOP
        }
    }

    /// @brief On future destruction ref-count is decreased and in case the callback invoked
    ///
    /// @note If the setting of the promise and the destruction of the future happens in parallel it is undefined if the
    /// callback is invoked or not. This is ok, since the promise-user, already set the value anyhow, so he would not
    /// benefit from being notified on abortion, because he is already setting the value (no calculation can be
    /// aborted).
    void UnregisterFuture() noexcept
    {
        const std::int32_t reference_count_before_decrement = reference_count_.fetch_sub(1);
        // Underflow of atomics is not UB. We can check for an underflow by looking at the prior value after the
        // operation.
        if (reference_count_before_decrement == 0)
        {
            std::terminate();
        }

        const std::int32_t reference_count = reference_count_before_decrement - 1;

        if (reference_count == 0)
        {
            if (ready_ == false)
            {
                [[maybe_unused]] std::lock_guard<Lockable> lock{callback_mutex_};
                if (on_abort_.has_value())
                {
                    (*on_abort_)();
                }
            }
        }
    }

    /// @brief Enables promise to register a on_abort callback
    void WithOnAbort(score::cpp::callback<void()> on_abort) noexcept
    {
        // not dead code: Lock guard ensures thread-safe
        // modification of on_abort_ which is later accessed in UnregisterFuture() under same mutex
        // coverity[autosar_cpp14_m0_1_9_violation]
        [[maybe_unused]] std::lock_guard<Lockable> lock{callback_mutex_};
        on_abort_ = std::move(on_abort);
    }

  protected:
    bool TestAndMarkValueAsSet() noexcept
    {
        return value_set_.exchange(true);
    }
    bool TestIfValueIsSet() const noexcept
    {
        return value_set_;
    }

  private:
    void ActualSetStateReady() noexcept
    {
        ready_ = true;
        ready_conditional_variable_.notify_all();
    }

    // false-positive: the method is used in Wait, WaitFor, WaitUntil
    // coverity[autosar_cpp14_a0_1_3_violation]
    score::cpp::expected_blank<Error> WaitByStrategy(
        score::cpp::callback<bool(std::unique_lock<std::remove_reference_t<Lockable>>&, score::cpp::callback<bool()>)>
            wait_strategy,
        const score::cpp::stop_token& stop_token) noexcept
    {
        if (ready_ == true)
        {
            return {};
        }

        // coverity[autosar_cpp14_a0_1_1_violation] clang-tidy requires variables to be inited
        bool wait_succeeded{};
        {
            std::unique_lock<std::remove_reference_t<Lockable>> lock{ready_mutex_};
            if (ready_ == true)
            {
                return {};
            }
            wait_succeeded = wait_strategy(lock, [this]() noexcept -> bool {
                return this->ready_;
            });
        }

        if (stop_token.stop_requested() == true)
        {
            return score::cpp::make_unexpected(Error::kStopRequested);
        }
        if (!wait_succeeded)
        {
            return score::cpp::make_unexpected(Error::kTimeout);
        }
        return {};
    }

    // Please note that we still must lock ready_mutex_ when handling the state_conditional_variable even though ready_
    // is atomic. Whenever the conditional variable is involved (wait, notify), treat ready_ as if it is non-atomic.
    std::atomic<bool> ready_;
    Lockable ready_mutex_;
    CV ready_conditional_variable_;

    std::atomic<bool> value_set_;

    Lockable callback_mutex_;
    std::optional<score::cpp::callback<void()>> on_abort_;
    std::atomic<std::int32_t> reference_count_;
};

}  // namespace detail
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_STATE_H
