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
#ifndef SCORE_LIB_CONCURRENCY_DELAYED_TASK_H
#define SCORE_LIB_CONCURRENCY_DELAYED_TASK_H

#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/interruptible_wait.h"
#include "score/concurrency/task.h"
#include "score/concurrency/task_result.h"

#include <score/callback.hpp>
#include <score/memory.hpp>
#include <score/pmr.hpp>

#include <future>

namespace score
{
namespace concurrency
{
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: Tolerated, this is a forward declaration.
// coverity[autosar_cpp14_m3_2_3_violation]
class DelayedTaskFactory;

namespace detail
{
/**
 * Base of all delayed tasks. Splitting DelayedTask over multiple classes allows us to expose the call-operator taking
 * a condition variable without forcing users to know the type of the callable. This is important for testing when the
 * tester wants to make sure that the periodic task was called with the correct timing. We also need this to share
 * common functionality between versions considering return value of the callable and the one not.
 * @tparam Clock The clock used
 */
template <typename Clock>
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class DelayedTaskBase : public Task
{
  protected:
    // Guard to secure the constructor against public to force people to use DelayedTaskFactory.
    // We need this to still enable the constructor for score::cpp::pmr::make_unique()
    struct ConstructionGuard
    {
    };

  public:
    /**
     * \brief Constructs a DelayedTask. Not for public use, instead use the DelayedTaskFactory.
     *
     * \param first_execution The point of time the first execution shall occur
     */
    explicit DelayedTaskBase(ConstructionGuard, typename Clock::time_point first_execution)
        : Task(), execution_time_point_{first_execution}, stop_source_{}
    {
    }

    ~DelayedTaskBase() override = default;
    DelayedTaskBase(const DelayedTaskBase&) = delete;
    DelayedTaskBase& operator=(const DelayedTaskBase&) = delete;

  protected:
    DelayedTaskBase(DelayedTaskBase&&) noexcept = default;
    DelayedTaskBase& operator=(DelayedTaskBase&&) noexcept = default;

  public:
    /**
     * \brief Calls underlying Callable
     *
     * \param executor The executor calling this function
     * \param token indicates whether the underlying task shall be stopped
     */
    void operator()(const score::cpp::stop_token token) override
    {
        InterruptibleConditionalVariable cv{};
        (*this)(token, cv);
    }

    /**
     * \brief Calls underlying Callable
     *
     * \note Only exposed for testing purposes
     *
     * \tparam CV The type of the condition variable to be used
     * \param token indicates whether the underlying task shall be stopped
     * \param cv The condition variable to use for waiting
     */
    template <typename CV>
    void operator()(const score::cpp::stop_token token, CV& cv)
    {
        {
            std::mutex mutex{};
            std::unique_lock<std::mutex> lock{mutex};
            score::cpp::ignore = cv.wait_until(lock, token, execution_time_point_, []() noexcept {
                return false;
            });
        }

        CallCallable(token);
    }

    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }

  protected:
    virtual void CallCallable(const score::cpp::stop_token& token) = 0;

    typename Clock::time_point GetExecutionTimePoint() const noexcept
    {
        return execution_time_point_;
    }

  private:
    typename Clock::time_point execution_time_point_;
    score::cpp::stop_source stop_source_;
};
}  // namespace detail

/**
 * A delayed task that is executed once
 * \tparam Clock The clock to use for waiting
 * \tparam CallableType The type of the callable to execute
 * \tparam ReturnType The type of the return value of the callable
 */
template <typename Clock, typename CallableType, typename ReturnType>
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class DelayedTask final : public detail::DelayedTaskBase<Clock>
{
  public:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The "DelayedTask" instance must be created via "DelayedTaskFactory" factory instead of direct constructor call.
    // Thus, access to ConstructionGuard requires.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class DelayedTaskFactory;

    /**
     * \brief Constructs a DelayedTask. Not for public use, instead use the DelayedTaskFactory.
     *
     * \param construction_guard Guard forcing construction via factory
     * \param promise The promise which shall be set once the task is finished
     * \param first_execution The point of time the first execution shall occur
     * \param callable The callable of the task
     */
    explicit DelayedTask(typename detail::DelayedTaskBase<Clock>::ConstructionGuard construction_guard,
                         concurrency::InterruptiblePromise<ReturnType>&& promise,
                         typename Clock::time_point first_execution,
                         CallableType&& callable)
        : detail::DelayedTaskBase<Clock>(std::move(construction_guard), first_execution),
          promise_{std::move(promise)},
          callable_{std::forward(callable)}
    {
    }

  private:
    void CallCallable(const score::cpp::stop_token& token) override
    {
        promise_.set_value(callable_(token, this->GetExecutionTimePoint()));
    }

    concurrency::InterruptiblePromise<ReturnType> promise_;
    CallableType callable_;
};

/**
 * A delayed task that is executed once
 * \tparam Clock The clock to use for waiting
 * \tparam CallableType The type of the callable to execute
 */
template <typename Clock, typename CallableType>
class DelayedTask<Clock, CallableType, void> final : public detail::DelayedTaskBase<Clock>
{
  public:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The "DelayedTask" instance must be created via "DelayedTaskFactory" factory instead of direct constructor call.
    // Thus, access to ConstructionGuard requires.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class DelayedTaskFactory;

    /**
     * \brief Constructs a DelayedTask. Not for public use, instead use the DelayedTaskFactory.
     *
     * \param construction_guard Guard forcing construction via factory
     * \param promise The promise which shall be set once the task is finished
     * \param first_execution The point of time the first execution shall occur
     * \param callable The callable of the task
     */
    explicit DelayedTask(typename detail::DelayedTaskBase<Clock>::ConstructionGuard construction_guard,
                         concurrency::InterruptiblePromise<void>&& promise,
                         typename Clock::time_point first_execution,
                         CallableType&& callable)
        : detail::DelayedTaskBase<Clock>(std::move(construction_guard), first_execution),
          promise_{std::move(promise)},
          callable_{std::forward<decltype(callable)>(callable)}
    {
    }

  private:
    void CallCallable(const score::cpp::stop_token& token) override
    {
        callable_(token, this->GetExecutionTimePoint());
        score::cpp::ignore = promise_.SetValue();
    }

    concurrency::InterruptiblePromise<void> promise_;
    // due to lambda usage number of template instantiation increases
    // leading to slightly longer compilation time
    // but doesnt affect the performance and keeps the code simple
    // coverity[autosar_cpp14_a5_1_7_violation]
    CallableType callable_;
};

// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class DelayedTaskFactory
{
  private:
    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    using result_type =
        std::invoke_result_t<CallableType, score::cpp::stop_token, typename Clock::time_point, ArgumentTypes&&...>;

  public:
    /**
     * \brief Helper function to construct a DelayedTask.
     *
     * \tparam Clock The clock to use for scheduling
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param execution_time_point The point of time the execution shall occur
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A DelayedTask constructed from the provided Callable
     */
    template <class Clock, class CallableType, class... ArgumentTypes>
    [[deprecated(
        "SPP_DEPRECATION: Performance Problem! Please use \'concurrency/timed_executor/delayed_task.h\'")]] static auto
    Make(score::cpp::pmr::memory_resource* memory_resource,
         typename Clock::time_point execution_time_point,
         CallableType&& callable,
         ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<result_type<Clock, CallableType, ArgumentTypes...>> promise{};

        return InternalMake<Clock>(memory_resource,
                                   std::move(promise),
                                   execution_time_point,
                                   std::forward<decltype(callable)>(callable),
                                   std::forward<decltype(arguments)>(arguments)...);
    }

    /**
     * \brief Helper function to construct a DelayedTask.
     *
     * \tparam Clock The clock to use for scheduling
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param memory_resource The memory resource for allocation of the task
     * \param execution_time_point The point of time the execution shall occur
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A tuple of TaskResult and DelayedTask constructed from the provided Callable
     */
    template <class Clock, class CallableType, class... ArgumentTypes>
    [[deprecated(
        "SPP_DEPRECATION: Performance Problem! Please use \'concurrency/timed_executor/delayed_task.h\'")]] static auto
    MakeWithTaskResult(score::cpp::pmr::memory_resource* memory_resource,
                       typename Clock::time_point execution_time_point,
                       CallableType&& callable,
                       ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<result_type<Clock, CallableType, ArgumentTypes...>> promise{};
        auto future = promise.GetInterruptibleFuture().value();

        auto task = InternalMake<Clock>(
            memory_resource,
            std::move(promise),
            execution_time_point,
            Wrap<Clock>(std::forward<decltype(callable)>(callable), std::forward<decltype(arguments)>(arguments)...));

        TaskResult<void> task_result{std::move(future), task->GetStopSource()};
        return std::make_pair(std::move(task_result), std::move(task));
    }

  private:
    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    static auto Wrap(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-poisitve
        return [callable = std::forward<decltype(callable)>(callable),
                // LCOV_EXCL_START Already tested via MakeWithTaskResultCapturesAllArguments
                tuple = std::make_tuple(std::forward<decltype(arguments)>(arguments)...)](
                   // LCOV_EXCL_STOP
                   const score::cpp::stop_token& token,
                   const typename Clock::time_point intended_execution) mutable {
            return score::cpp::apply(std::forward<CallableType>(callable),
                              std::tuple_cat(std::tie(token), std::tie(intended_execution), tuple));
        };
    }

    template <class Clock, class CallableType, class... ArgumentTypes>
    static auto InternalMake(score::cpp::pmr::memory_resource* memory_resource,
                             concurrency::InterruptiblePromise<void>&& promise,
                             typename Clock::time_point execution_time_point,
                             CallableType&& callable,
                             ArgumentTypes&&... arguments)
    {
        auto wrapped_callable =
            Wrap<Clock>(std::forward<decltype(callable)>(callable), std::forward<decltype(arguments)>(arguments)...);
        /* KW_SUPPRESS_END:MISRA.VAR.NEEDS.CONST:Moving from variable below */
        // due to lambda usage number of template instantiation increases
        // leading to slightly longer compilation time
        // but doesnt affect the performance and keeps the code simple
        // coverity[autosar_cpp14_a5_1_7_violation]
        using DelayedTaskType =
            DelayedTask<Clock, decltype(wrapped_callable), result_type<Clock, CallableType, ArgumentTypes...>>;
        auto task = score::cpp::pmr::make_unique<DelayedTaskType>(memory_resource,
                                                           typename DelayedTaskType::ConstructionGuard{},
                                                           std::forward<decltype(promise)>(promise),
                                                           execution_time_point,
                                                           std::forward<decltype(wrapped_callable)>(wrapped_callable));

        return task;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_DELAYED_TASK_H
