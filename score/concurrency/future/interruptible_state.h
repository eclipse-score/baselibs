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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_STATE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_STATE_H

#include "score/language/safecpp/scoped_function/move_only_scoped_function.h"
#include "score/language/safecpp/scoped_function/scope.h"

#include "score/concurrency/future/base_interruptible_state.h"
#include "score/concurrency/future/error.h"
#include "score/concurrency/interruptible_wait.h"
#include "score/result/result.h"

#include "score/callback.hpp"
#include "score/expected.hpp"

#include <atomic>
#include <mutex>
#include <vector>

namespace score
{
namespace concurrency
{
namespace detail
{
using TypedBaseInterruptibleState =
    score::concurrency::detail::BaseInterruptibleState<std::mutex, score::concurrency::InterruptibleConditionalVariable>;

template <class>
struct IsScoped : std::false_type
{
};

template <class T>
struct IsScoped<safecpp::MoveOnlyScopedFunction<T>> : std::true_type
{
};

template <class T>
struct IsScoped<safecpp::CopyableScopedFunction<T>> : std::true_type
{
};

}  // namespace detail

template <typename Value>
class InterruptibleState final : public score::concurrency::detail::TypedBaseInterruptibleState
{
  public:
    using ScopedContinuationCallback = safecpp::MoveOnlyScopedFunction<void(score::Result<Value>&)>;

    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    static std::shared_ptr<InterruptibleState> Make()
    {
        return std::make_shared<InterruptibleState>();
    }

    template <
        typename V = Value,
        std::enable_if_t<std::is_move_constructible<V>::value && !std::is_lvalue_reference<V>::value, bool> = true>
    bool SetValue(Value&& value)
    {
        if ((this->TestAndMarkValueAsSet()) == true)
        {
            return false;
        }

        // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
        new (&value_) score::Result<Value>{std::move(value)};

        MakeReady();
        TriggerContinuations();
        return true;
    }

    template <typename V = Value, std::enable_if_t<std::is_copy_constructible<V>::value, bool> = true>
    // && !std::is_lvalue_reference<V> in move overload prevents call ambiguitiy if
    // Value is both copy and move constructible
    // coverity[autosar_cpp14_a13_3_1_violation]
    bool SetValue(const Value& value)
    {
        if ((this->TestAndMarkValueAsSet()) == true)
        {
            return false;
        }

        // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
        new (&value_) score::Result<Value>{value};

        MakeReady();
        TriggerContinuations();
        return true;
    }

    bool SetError(score::result::Error error)
    {
        if ((this->TestAndMarkValueAsSet()) == true)
        {
            return false;
        }

        // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
        new (&value_) score::Result<Value>{MakeUnexpected<Value>(error)};

        MakeReady();
        TriggerContinuations();
        return true;
    }

    // Suppress "AUTOSAR C++14 A9-3-1" rule finding: "Member functions shall not return non-const “raw” pointers or
    // references to private or protected data owned by the class."
    // Intended by implementation, caller e.i. InterruptibleSharedFuture requires a reference.
    // coverity[autosar_cpp14_a9_3_1_violation]
    score::Result<Value>& GetValue() noexcept
    {
        return value_;
    }

    void AddContinuationCallback(ScopedContinuationCallback callback)
    {
        RegisterFuture();
        std::unique_lock<std::mutex> lock{continuation_callback_mutex_};
        if (triggered_)
        {
            lock.unlock();
            score::cpp::ignore = callback(value_);
        }
        else
        {
            continuation_callbacks_.push_back(std::move(callback));
        }
    }

    const safecpp::Scope<>& GetScope() const noexcept
    {
        return scope_;
    }

  private:
    // false-positive: the method is used in SetError and SetValue
    // coverity[autosar_cpp14_a0_1_3_violation]
    void TriggerContinuations()
    {
        {
            // not dead code: Lock guard ensures thread-safe
            // iteration over continuation_callbacks_ during callback execution
            // coverity[autosar_cpp14_m0_1_3_violation]
            // coverity[autosar_cpp14_m0_1_9_violation]
            std::lock_guard<std::mutex> lock{continuation_callback_mutex_};
            triggered_ = true;
        }
        for (auto& callback : continuation_callbacks_)
        {
            score::cpp::ignore = callback(value_);
        }
        continuation_callbacks_.clear();
    }

    using score::concurrency::detail::TypedBaseInterruptibleState::BaseInterruptibleState;
    score::Result<Value> value_{MakeUnexpected(Error::kUnset)};

    std::mutex continuation_callback_mutex_{};
    safecpp::Scope<> scope_{};
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::vector<ScopedContinuationCallback> continuation_callbacks_{};
    bool triggered_{false};
};

template <typename Value>
class InterruptibleState<Value&> final : public score::concurrency::detail::TypedBaseInterruptibleState
{
  public:
    using ScopedContinuationCallback =
        safecpp::MoveOnlyScopedFunction<void(score::Result<std::reference_wrapper<Value>>&)>;

    static std::shared_ptr<InterruptibleState> Make()
    {
        return std::make_shared<InterruptibleState>();
    }

    bool SetValue(Value& value)
    {
        if ((this->TestAndMarkValueAsSet()) == true)
        {
            return false;
        }

        // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
        new (&value_) score::Result<std::reference_wrapper<Value>>{std::ref(value)};

        MakeReady();
        TriggerContinuations();
        return true;
    }

    bool SetError(score::result::Error error)
    {
        if ((this->TestAndMarkValueAsSet()) == true)
        {
            return false;
        }

        // Use the constructor instead of assignment operator to circumvent issue with types that are not assignable
        // NOLINTNEXTLINE(score-no-dynamic-raw-memory): Non-assignable types workaround
        new (&value_) score::Result<std::reference_wrapper<Value>>{MakeUnexpected<std::reference_wrapper<Value>>(error)};

        MakeReady();
        TriggerContinuations();
        return true;
    }

    score::Result<std::reference_wrapper<Value>>& GetValue() noexcept
    {
        return value_;
    }

    void AddContinuationCallback(ScopedContinuationCallback callback)
    {
        RegisterFuture();
        std::unique_lock<std::mutex> lock{continuation_callback_mutex_};
        if (triggered_)
        {
            lock.unlock();
            score::cpp::ignore = callback(value_);
        }
        else
        {
            continuation_callbacks_.push_back(std::move(callback));
        }
    }

    const safecpp::Scope<>& GetScope() const noexcept
    {
        return scope_;
    }

  private:
    // false-positive: the method is used in SetError and SetValue
    // coverity[autosar_cpp14_a0_1_3_violation]
    void TriggerContinuations()
    {
        {
            // not dead code: Lock guard ensures thread-safe
            // iteration over continuation_callbacks_ during callback execution
            // coverity[autosar_cpp14_m0_1_3_violation]
            // coverity[autosar_cpp14_m0_1_9_violation]
            std::lock_guard<std::mutex> lock{continuation_callback_mutex_};
            triggered_ = true;
        }
        for (auto& callback : continuation_callbacks_)
        {
            score::cpp::ignore = callback(value_);
        }
        continuation_callbacks_.clear();
    }

    using score::concurrency::detail::TypedBaseInterruptibleState::BaseInterruptibleState;
    score::Result<std::reference_wrapper<Value>> value_{MakeUnexpected(Error::kUnset)};

    std::mutex continuation_callback_mutex_{};
    safecpp::Scope<> scope_{};
    std::vector<ScopedContinuationCallback> continuation_callbacks_{};
    bool triggered_{false};
};

template <>
class InterruptibleState<void> final : public score::concurrency::detail::TypedBaseInterruptibleState
{
  public:
    using ScopedContinuationCallback = safecpp::MoveOnlyScopedFunction<void(score::ResultBlank&)>;

    static std::shared_ptr<InterruptibleState> Make()
    {
        return std::make_shared<InterruptibleState>();
    }

    bool SetValue();

    bool SetError(score::result::Error error);

    score::ResultBlank& GetValue() noexcept;

    void AddContinuationCallback(ScopedContinuationCallback callback);

  private:
    void TriggerContinuations();

    using score::concurrency::detail::TypedBaseInterruptibleState::BaseInterruptibleState;
    score::ResultBlank value_{};

    std::mutex continuation_callback_mutex_{};
    safecpp::Scope<> scope_{};
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::vector<ScopedContinuationCallback> continuation_callbacks_{};
    bool triggered_{false};
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_INTERRUPTIBLE_STATE_H
