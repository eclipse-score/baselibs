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
#ifndef SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_PROMISE_H
#define SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_PROMISE_H

#include "score/concurrency/future/error.h"
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_state.h"

#include "score/expected.hpp"

#include <memory>

namespace score
{
namespace concurrency
{
namespace detail
{

/**
 * Base for interruptible promise used to provide state to InterruptibleFuture or InterruptibleSharedFuture. Close
 * reimplementation of std::promise.
 *
 * \tparam Value The type of the values stored in the shared state
 */
template <typename Value>
class BaseInterruptiblePromise
{
  public:
    /**
     * Constructs a BaseInterruptiblePromise with an associated shared state.
     */
    BaseInterruptiblePromise() : state_{InterruptibleState<Value>::Make()}, future_retrieved_{false} {}

    /**
     * Constructs a BaseInterruptiblePromise with the shared state of another BaseInterruptiblePromise.
     *
     * \param other Other InterruptiblePromise to move the shared state from
     */

    // it is used in tests to verify move behavior; the tests access BaseInterruptiblePromise directly
    // without introducing object slicing issues
    // coverity[autosar_cpp14_a12_8_6_violation]
    BaseInterruptiblePromise(BaseInterruptiblePromise<Value>&& other) noexcept
        : state_{std::move(other.state_)}, future_retrieved_{std::move(other.future_retrieved_)}
    {
        other.state_.reset();
    }

    /**
     * Moves the shared state from another BaseInterruptiblePromise to *this and sets the previous shared state of *this
     * to broken (if it was not already set previously). If the future was already retrieved from the other
     * BaseInterruptiblePromise, it can not be retrieved a second time from *this.
     *
     * \param other Other InterruptiblePromise to move the shared state from
     * \return A reference to *this
     */
    // it is used in tests to verify move behavior; the tests access BaseInterruptiblePromise directly
    // without introducing object slicing issues
    // coverity[autosar_cpp14_a12_8_6_violation]
    // coverity[autosar_cpp14_a6_2_1_violation] intentional side effects; see above
    BaseInterruptiblePromise<Value>& operator=(BaseInterruptiblePromise<Value>&& other) noexcept
    {
        if (state_ != other.state_)
        {
            if (HasState())
            {
                score::cpp::ignore = state_->SetError(score::concurrency::Error::kPromiseBroken);
            }
            state_ = std::move(other.state_);
            other.state_.reset();

            future_retrieved_ = other.future_retrieved_;
        }
        return *this;
    };

    BaseInterruptiblePromise(const BaseInterruptiblePromise<Value>&) noexcept = delete;

    BaseInterruptiblePromise<Value>& operator=(const BaseInterruptiblePromise<Value>&) noexcept = delete;

    /**
     * Destructs the promise and sets the shared state to broken if it was not set previously.
     */
    virtual ~BaseInterruptiblePromise() noexcept
    {
        if (HasState())
        {
            score::cpp::ignore = state_->SetError(score::concurrency::Error::kPromiseBroken);
        }
    }

    /**
     * Exchanges the shared state of *this and other
     *
     * \param other InterruptiblePromise to switch shared state with
     */
    void Swap(BaseInterruptiblePromise<Value>& other) noexcept
    {
        std::swap(state_, other.state_);
        std::swap(future_retrieved_, other.future_retrieved_);
    }

    /**
     * Returns an interruptible future once, associated with the same shared state as *this.
     *
     * Calls to this function do NOT introduce data races with calls to SetValue() or SetError().
     *
     * \returns The interruptible future or an Error if:
     * - no shared state is associated to *this
     * - GetInterruptibleFuture() was called once before.
     */
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    score::cpp::expected<InterruptibleFuture<Value>, Error> GetInterruptibleFuture() noexcept
    {
        if (future_retrieved_)
        {
            return score::cpp::make_unexpected(Error::kFutureAlreadyRetrieved);
        }

        future_retrieved_ = true;

        if (state_ == nullptr)
        {
            return score::cpp::make_unexpected(Error::kNoState);
        }

        // intentional usage; may generate more instantiations, but is harmless
        // coverity[autosar_cpp14_a5_1_7_violation]
        return {InterruptibleFuture<Value>(state_)};
    }

    /**
     * Stores the error in the shared state and makes the state ready.
     *
     * Calls to this function do NOT introduce data races with calls to GetInterruptibleFuture() or SetError().
     *
     * \param error Error to store in the shared state
     * \returns Blank expected on success or with Error if:
     * - no shared state is associated
     * - shared state already stores a value or error
     */
    score::cpp::expected_blank<Error> SetError(score::result::Error error) noexcept
    {
        if (!HasState())
        {
            return score::cpp::make_unexpected(Error::kNoState);
        }

        const bool result = BaseInterruptiblePromise<Value>::state_->SetError(error);
        if (!result)
        {
            return score::cpp::make_unexpected(Error::kPromiseAlreadySatisfied);
        }
        return {};
    }

    /**
     * Stores a callback in shared state, that is invoked if all futures have been destroyed for this promise (only if
     * promise was not already satisfied aka ready).
     *
     * \param on_abort The callback to be executed
     * \return An error if no shared state is available
     */
    score::cpp::expected_blank<Error> OnAbort(score::cpp::callback<void()> on_abort) noexcept
    {
        if (!HasState())
        {
            return score::cpp::make_unexpected(Error::kNoState);
        }

        state_->WithOnAbort(std::move(on_abort));

        return {};
    }

  protected:
    bool HasState() const noexcept
    {
        return state_ != nullptr;
    }

    template <typename StrategyCallback, typename... OptValue>
    score::cpp::expected_blank<Error> SetValueInternalWithStrategy(const StrategyCallback& strategy,
                                                                   OptValue&&... value) noexcept
    {
        if (!this->HasState())
        {
            return score::cpp::make_unexpected(Error::kNoState);
        }

        const bool result = strategy(*(this->state_), std::forward<OptValue>(value)...);
        if (!result)
        {
            return score::cpp::make_unexpected(Error::kPromiseAlreadySatisfied);
        }
        return {};
    }

  private:
    // intentional usage; may generate more instantiations, but is harmless
    // coverity[autosar_cpp14_a5_1_7_violation]
    std::shared_ptr<InterruptibleState<Value>> state_;
    bool future_retrieved_;
};

}  // namespace detail
}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_FUTURE_BASE_INTERRUPTIBLE_PROMISE_H
