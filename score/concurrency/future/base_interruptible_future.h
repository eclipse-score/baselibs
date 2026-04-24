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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_FUTURE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_FUTURE_H

#include "score/concurrency/future/error.h"
#include "score/concurrency/future/interruptible_state.h"

#include "score/expected.hpp"

#include <memory>

namespace score
{
namespace concurrency
{
namespace detail
{

// Helper functions to call a function ignoring the function result
// filter all the functions with non-void result
template <typename Func, typename... Args>
auto CallAndIgnoreResult(Func& func, Args&&... args) noexcept(noexcept(func(std::forward<Args>(args)...)))
    -> std::enable_if_t<!std::is_same_v<void, std::invoke_result_t<Func, Args...>>, void>
{
    score::cpp::ignore = func(std::forward<Args>(args)...);
}
// filter all the functions with void result
template <typename Func, typename... Args>
// coverity[autosar_cpp14_a13_3_1_violation] deliberately done for filtering purpose
auto CallAndIgnoreResult(Func& func, Args&&... args) noexcept(noexcept(func(std::forward<Args>(args)...)))
    -> std::enable_if_t<std::is_same_v<void, std::invoke_result_t<Func, Args...>>, void>
{
    func(std::forward<Args>(args)...);
}

/**
 * Base class for interruptible futures that provides the waiting algorithms and contains the state.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class BaseInterruptibleFuture
{
  public:
    constexpr BaseInterruptibleFuture() noexcept : BaseInterruptibleFuture{nullptr} {}

    // We keep constructors to retain coverage of the class. (explicit use in tests)
    // Slicing is prevented by this class residing in the namespace detail,
    // which indicates it is not intended for direct use

    // coverity[autosar_cpp14_a12_8_6_violation] see above
    BaseInterruptibleFuture(const BaseInterruptibleFuture& other) noexcept
    {
        this->state_ptr_ = other.state_ptr_;
        if (this->state_ptr_ != nullptr)
        {
            state_ptr_->RegisterFuture();
        }
    };

    // intentional side effects
    // RegisterFuture() and UnregisterFuture() calls properly maintain reference counts
    // and potentially trigger callbacks
    // coverity[autosar_cpp14_a6_2_1_violation]
    // coverity[autosar_cpp14_a12_8_6_violation] see above
    BaseInterruptibleFuture& operator=(const BaseInterruptibleFuture& other) noexcept
    {
        if (this != &other)
        {
            if (other.state_ptr_ != nullptr)
            {
                other.state_ptr_->RegisterFuture();
            }
            if (this->state_ptr_ != nullptr)
            {
                state_ptr_->UnregisterFuture();
            }
            this->state_ptr_ = other.state_ptr_;
        }
        return *this;
    };

    // coverity[autosar_cpp14_a12_8_6_violation] see above
    BaseInterruptibleFuture(BaseInterruptibleFuture&& other) noexcept = default;

    // intentional side effects
    // the UnregisterFuture() call handles proper cleanup of the previous state, potentially triggering abort callbacks
    // coverity[autosar_cpp14_a6_2_1_violation]
    // coverity[autosar_cpp14_a12_8_6_violation] see above
    BaseInterruptibleFuture& operator=(BaseInterruptibleFuture&& other) noexcept
    {
        if (this->state_ptr_ != nullptr)
        {
            state_ptr_->UnregisterFuture();
        }
        this->state_ptr_ = std::move(other.state_ptr_);
        return *this;
    };

    // Suppress "AUTOSAR C++14 M3-2-2" rule finding. This rule states: "The One Definition Rule shall not be
    // violated.".
    // Templates are allowed to have multiple definitions across translation units as long as they are identical.
    template <typename DerivedInterruptibleFuture,
              typename = std::enable_if_t<std::is_base_of<BaseInterruptibleFuture, DerivedInterruptibleFuture>::value>>
    // coverity[autosar_cpp14_a12_8_6_violation] see above
    // coverity[autosar_cpp14_m3_2_2_violation]
    // coverity[autosar_cpp14_a7_1_8_violation] false-positive; there are no non-type specs placed before type specs
    explicit BaseInterruptibleFuture(DerivedInterruptibleFuture&& derived) noexcept
    {
        // coverity[autosar_cpp14_m6_2_1_violation] sub-expression is here to fix another coverity finding
        score::cpp::ignore = this->operator=(std::forward<DerivedInterruptibleFuture>(derived));
    }

    virtual ~BaseInterruptibleFuture()
    {
        if (state_ptr_ != nullptr)
        {
            state_ptr_->UnregisterFuture();
        }
    };

    /**
     * Returns whether the *this has an associated shared state.
     *
     * \returns true if there is an associated shared state
     */
    bool Valid() const noexcept
    {
        return state_ptr_ != nullptr;
    }

    /**
     * Waits for the shared state to become ready. Calls to this function block indefinitely until the state becomes
     * ready. To abort prematurely, the stop token may be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \returns Blank value on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     */
    score::cpp::expected_blank<Error> Wait(const score::cpp::stop_token& stop_token) const noexcept
    {
        if (state_ptr_ != nullptr)
        {
            return state_ptr_->Wait(stop_token);
        }
        return score::cpp::make_unexpected(Error::kNoState);
    }

    /**
     * Waits for the shared state to become ready or the specified relative time to pass. Calls to this function block
     * indefinitely until the state becomes ready. To abort prematurely, the stop token may be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \param rel_time Relative time span to pass until a timeout is triggered
     * \returns Blank value on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     * - the relative time passed
     */
    template <class Representation, class Period>
    score::cpp::expected_blank<Error> WaitFor(const score::cpp::stop_token& stop_token,
                                       const std::chrono::duration<Representation, Period>& rel_time) const noexcept
    {
        if (state_ptr_ != nullptr)
        {
            return state_ptr_->WaitFor(stop_token, rel_time);
        }
        return score::cpp::make_unexpected(Error::kNoState);
    }

    /**
     * Waits for the shared state to become ready or a specified absolute time to be reached. Calls to this function
     * block indefinitely until the state becomes ready. To abort prematurely, the stop token may be set.
     *
     * \param stop_token Token to abort the wait prematurely.
     * \param abs_time Absolute time to reach until a timeout is triggered
     * \returns Blank value on success or an Error if:
     * - no shared state is associated
     * - the token was set
     * - the promise was broken
     * - the absolute time is reached
     */
    template <class Clock, class Duration>
    score::cpp::expected_blank<Error> WaitUntil(const score::cpp::stop_token& stop_token,
                                         const std::chrono::time_point<Clock, Duration>& abs_time) const noexcept
    {
        if (state_ptr_ != nullptr)
        {
            return state_ptr_->WaitUntil(stop_token, abs_time);
        }
        return score::cpp::make_unexpected(Error::kNoState);
    }

    /**
     * Registers a continuation callback to be invoked by the thread fulfilling the callback.
     * \note If the future instance goes out of scope before the callback completes, the implementation will wait for
     *       the callback to finish before destructing the future.
     *
     * \param callback The callback to be invoked.
     * \returns An empty value on success or an Error.
     */
    score::cpp::expected_blank<Error> Then(typename InterruptibleState<Value>::ScopedContinuationCallback&& callback)
    {
        if (state_ptr_ == nullptr)
        {
            CallContinuationWithoutState(
                std::forward<typename InterruptibleState<Value>::ScopedContinuationCallback>(callback));
            return score::cpp::make_unexpected(Error::kNoState);
        }

        state_ptr_->AddContinuationCallback(
            std::forward<typename InterruptibleState<Value>::ScopedContinuationCallback>(callback));
        return {};
    }

  protected:
    explicit BaseInterruptibleFuture(std::shared_ptr<InterruptibleState<Value>> state_ptr) noexcept
        : state_ptr_{std::move(state_ptr)}
    {
        if (state_ptr_ != nullptr)
        {
            state_ptr_->RegisterFuture();
        }
    }

    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::shared_ptr<InterruptibleState<Value>> StealState() noexcept
    {

        // intentional usage; may generate more instantiations, but is harmless
        // coverity[autosar_cpp14_a5_1_7_violation]
        std::shared_ptr<InterruptibleState<Value>> stolen_state_ptr{nullptr};
        stolen_state_ptr.swap(state_ptr_);
        return stolen_state_ptr;
    }

    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::shared_ptr<InterruptibleState<Value>> AccessState() const noexcept
    {
        return state_ptr_;
    }

  private:
    template <typename Callable,
              typename LocalValue = Value,
              std::enable_if_t<(!std::is_void<LocalValue>::value) && (!std::is_lvalue_reference<LocalValue>::value),
                               bool> = true>
    static void CallContinuationWithoutState(Callable callback)
    {
        auto error = score::MakeUnexpected<Value>(Error::kNoState);
        CallAndIgnoreResult(callback, error);
    }

    template <typename Callable,
              typename LocalValue = Value,
              std::enable_if_t<(!std::is_void<std::remove_reference_t<LocalValue>>::value) &&
                                   std::is_lvalue_reference<LocalValue>::value,
                               bool> = true>
    static void CallContinuationWithoutState(Callable callback)
    {
        auto error = score::MakeUnexpected<std::reference_wrapper<std::remove_reference_t<LocalValue>>>(Error::kNoState);
        CallAndIgnoreResult(callback, error);
    }

    template <typename Callable,
              typename LocalValue = Value,
              std::enable_if_t<std::is_void<LocalValue>::value, bool> = true>
    static void CallContinuationWithoutState(Callable callback)
    {
        auto error = score::MakeUnexpected<void>(Error::kNoState);
        CallAndIgnoreResult(callback, error);
    }

    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::shared_ptr<InterruptibleState<Value>> state_ptr_;
};

}  // namespace detail
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_FUTURE_H
