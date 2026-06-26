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
#ifndef SCORE_LIB_CONCURRENCY_SHARED_TASK_RESULT_H
#define SCORE_LIB_CONCURRENCY_SHARED_TASK_RESULT_H

#include "score/assert.hpp"
#include "score/stop_token.hpp"
#include "score/utility.hpp"

#include "score/concurrency/future/interruptible_shared_future.h"
#include "score/concurrency/task_result_base.h"

namespace score
{
namespace concurrency
{

/// \brief The SharedTaskResult provides a mechanism to either access the result of an asynchronous running `Task` or
/// gives the user the possibility to request an aborting of the running Task, in cases where the result is no longer
/// required.
///
/// SharedTaskResult is constructed by submitting a Task to an `Executor`.
///
/// \details In order to do so, SharedTaskResult acts as a facade around InterruptibleSharedFuture and std::stop_source,
///          SharedTaskResult can be copied into another SharedTaskResult.
///          also it can be constructed from TaskResul
///
/// \tparam T

template <typename T>
class SharedTaskResult : public TaskResultBase
{
  public:
    SharedTaskResult() noexcept : TaskResultBase{} {}

    /// \brief Constructor to create a SharedTaskResult associated with a Task.
    ///
    /// \note While this should only be used to create a pair of Task and TaskResult, we still make this public. This
    /// has the added benefit that any custom task deriving from Task can also create a TaskResult
    SharedTaskResult(concurrency::InterruptibleSharedFuture<T> future, score::cpp::stop_source stop_source) noexcept
        : TaskResultBase{std::move(stop_source)}, future_{std::move(future)}
    {
    }

    SharedTaskResult(const SharedTaskResult&) = default;
    SharedTaskResult(SharedTaskResult&&) noexcept = default;
    SharedTaskResult& operator=(const SharedTaskResult&) = default;
    SharedTaskResult& operator=(SharedTaskResult&&) noexcept = default;

    ~SharedTaskResult() noexcept override = default;

    /// \brief Checks if the future refers to a shared state.
    /// \return true if *this refers to a shared state, otherwise false.
    bool Valid() noexcept
    {
        return future_.Valid();
    }

    /// \brief The get member function waits until the future has a valid result and (depending on which template is
    /// used) retrieves it. It effectively calls wait() in order to wait for the result.
    /// \return The value stored in the shared state
    auto Get() noexcept
    {
        // We use a dummy stop source here because we intentionally want to wait for the result even if a stop is
        // requested via the real stop source.This wait is always finite, as any task that we wait for has a stop token
        // that will be eventually set.
        return future_.Get(dummy_stop_source_.get_token());
    }

    /// \brief Then is a Continuation for SharedTaskResult
    /// \return The value stored in the shared state
    auto Then(typename InterruptibleState<T>::ScopedContinuationCallback callback) noexcept
    {
        return future_.Then(std::forward<typename InterruptibleState<T>::ScopedContinuationCallback>(callback));
    }

    /// \brief Then is a Continuation for SharedTaskResult
    /// \return The value stored in the shared state
    template <typename Callback,
              std::enable_if_t<std::is_constructible_v<typename InterruptibleState<T>::ScopedContinuationCallback,
                                                       safecpp::Scope<>,
                                                       Callback>,
                               bool> = true>
    [[deprecated("SPP_DEPRECATION: Use overload with scoped function instead. (Ticket-141243")]] score::cpp::
        expected_blank<Error>
        Then(Callback callback) noexcept
    {
        return future_.Then(std::forward<Callback>(callback));
    }

    /// \brief Blocks until the result becomes available. valid() == true after the call.
    /// The behavior is undefined if valid() == false before the call to this function.
    auto Wait() noexcept
    {
        // We use a dummy stop source here because we intentionally want to wait for the result even if a stop is
        // requested via the real stop source.This wait is always finite, as any task that we wait for has a stop token
        // that will be eventually set.
        return future_.Wait(dummy_stop_source_.get_token());
    }

    /// \brief Waits for the result to become available. Blocks until specified timeout_duration has elapsed or the
    /// result becomes available. The return value identifies the state of
    /// the result.
    ///
    /// \details This function may block for longer than timeout_duration due to scheduling or resource contention
    /// delays.
    ///
    /// The standard recommends that a steady clock is used to measure the duration. If an implementation uses a system
    /// clock instead, the wait time may also be sensitive to clock adjustments.
    ///
    /// The behavior is undefined if valid() is false before the call to this function.
    ///
    /// \param timeout_time maximum duration to block for
    /// \returns Blank value on success or an Error if:
    /// - no shared state is associated
    /// - the promise was broken
    /// - the absolute time is reached
    template <class Rep, class Period>
    score::cpp::expected_blank<Error> WaitFor(const std::chrono::duration<Rep, Period>& timeout_duration) const noexcept
    {
        // We use a dummy stop source here because we intentionally want to wait for the result even if a stop is
        // requested via the real stop source.This wait is always finite, as any task that we wait for has a stop token
        // that will be eventually set.
        return future_.WaitFor(dummy_stop_source_.get_token(), timeout_duration);
    }

    /// \brief wait_until waits for a result to become available. It blocks until specified timeout_time has been
    /// reached or the result becomes available, whichever comes first. The return value indicates
    /// why wait_until returned.
    ///
    /// The behavior is undefined if valid() is false before the call to this function, or Clock does not meet the Clock
    /// requirements.
    /// \param timeout_time maximum time point to block until
    /// \return The shared std::future_status
    /// \returns Blank value on success or an Error if:
    /// - no shared state is associated
    /// - the promise was broken
    /// - the absolute time is reached
    template <class Clock, class Duration>
    score::cpp::expected_blank<Error> WaitUntil(
        const std::chrono::time_point<Clock, Duration>& timeout_time) const noexcept
    {
        // We use a dummy stop source here because we intentionally want to wait for the result even if a stop is
        // requested via the real stop source.This wait is always finite, as any task that we wait for has a stop token
        // that will be eventually set.
        return future_.WaitUntil(dummy_stop_source_.get_token(), timeout_time);
    }

  private:
    concurrency::InterruptibleSharedFuture<T> future_;
    score::cpp::stop_source dummy_stop_source_;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_SHARED_TASK_RESULT_H
