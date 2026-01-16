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
#ifndef SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_DELAYED_TASK_H
#define SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_DELAYED_TASK_H

#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/interruptible_wait.h"
#include "score/concurrency/task_result.h"
#include "score/concurrency/timed_executor/timed_task.h"

#include <score/callback.hpp>
#include <score/memory.hpp>
#include <score/pmr.hpp>

#include <future>

namespace score
{
namespace concurrency
{
class DelayedTaskFactory;

namespace detail
{
/**
 * Base of all delayed tasks. Splitting DelayedTask over multiple classes allows us to expose the call-operator taking
 * a condition variable without forcing users to know the type of the callable. This is important for testing when the
 * tester wants to make sure that the periodic task was called with the correct timing.
 * @tparam Clock The clock used
 */
template <typename Clock>
class DelayedTaskBase : public TimedTask<Clock>
{
  protected:
    // Guard to secure the constructor against public to force people to use DelayedTaskFactory.
    // We need this to still enable the constructor for score::cpp::pmr::make_unique()
    class ConstructionGuard
    {
      public:
        constexpr explicit ConstructionGuard() noexcept = default;
    };

  public:
    /**
     * \brief Constructs a DelayedTask. Not for public use, instead use the DelayedTaskFactory.
     *
     * \param first_execution The point of time the first execution shall occur
     */
    explicit DelayedTaskBase(ConstructionGuard, typename Clock::time_point first_execution)
        : TimedTask<Clock>(), execution_time_point_{first_execution}, stop_source_{}
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
        CallCallable(token);
        score::cpp::ignore = stop_source_.request_stop();
    }

    [[nodiscard]] std::optional<std::chrono::time_point<Clock>> GetNextExecutionPoint() const noexcept override
    {
        if (stop_source_.stop_requested())
        {
            return {};
        }
        return execution_time_point_;
    }

    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }

  protected:
    virtual void CallCallable(const score::cpp::stop_token& token) = 0;

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
class DelayedTask final : public detail::DelayedTaskBase<Clock>
{
  public:
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
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
          callable_{std::forward<CallableType>(callable)}
    {
    }

  private:
    void CallCallable(const score::cpp::stop_token& token) override
    {
        const auto time_point = this->GetNextExecutionPoint();
        if (time_point.has_value())
        {
            // coverity[autosar_cpp14_m6_4_1_violation] false-positive
            // coverity[autosar_cpp14_a7_1_8_violation] false-positive
            if constexpr (std::is_same_v<void, ReturnType>)
            {
                callable_(token, time_point.value());
                score::cpp::ignore = promise_.SetValue();
            }
            else
            {
                score::cpp::ignore = promise_.SetValue(callable_(token, time_point.value()));
            }
        }
    }

    concurrency::InterruptiblePromise<ReturnType> promise_;
    // due to lambda usage number of template instantiation increases
    // leading to slightly longer compilation time
    // but doesnt affect the performance and keeps the code simple
    // coverity[autosar_cpp14_a5_1_7_violation]
    CallableType callable_;
};

class DelayedTaskFactory
{
  private:
    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    using ResultType =
        std::invoke_result_t<CallableType && (score::cpp::stop_token, typename Clock::time_point, ArgumentTypes&&...)>;

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
    static auto Make(score::cpp::pmr::memory_resource* memory_resource,
                     typename Clock::time_point execution_time_point,
                     CallableType&& callable,
                     ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<ResultType<Clock, CallableType, ArgumentTypes...>> promise{};

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
    static auto MakeWithTaskResult(score::cpp::pmr::memory_resource* memory_resource,
                                   typename Clock::time_point execution_time_point,
                                   CallableType&& callable,
                                   ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<ResultType<Clock, CallableType, ArgumentTypes...>> promise{};

        auto future = promise.GetInterruptibleFuture().value();

        auto task = InternalMake<Clock>(
            memory_resource,
            std::move(promise),
            execution_time_point,
            Wrap<Clock>(std::forward<decltype(callable)>(callable), std::forward<decltype(arguments)>(arguments)...));

        TaskResult<ResultType<Clock, CallableType, ArgumentTypes...>> task_result{std::move(future),
                                                                                  task->GetStopSource()};
        return std::make_pair(std::move(task_result), std::move(task));
    }

  private:
    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    static auto Wrap(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-positive: already forwarded
        return [callable = std::forward<decltype(callable)>(callable),
                // LCOV_EXCL_START Already tested via MakeWithTaskResultCapturesAllArguments
                tuple = std::make_tuple(std::forward<decltype(arguments)>(arguments)...)](
                   // LCOV_EXCL_STOP
                   const score::cpp::stop_token& token,
                   const typename Clock::time_point intended_execution) mutable -> auto {
            return std::apply(std::forward<CallableType>(callable),
                              std::tuple_cat(std::tie(token), std::tie(intended_execution), tuple));
        };
    }

    template <class Clock, class CallableType, class... ArgumentTypes>
    static auto InternalMake(
        score::cpp::pmr::memory_resource* memory_resource,
        concurrency::InterruptiblePromise<ResultType<Clock, CallableType, ArgumentTypes...>>&& promise,
        typename Clock::time_point execution_time_point,
        CallableType&& callable,
        ArgumentTypes&&... arguments) noexcept
    {
        auto wrapped_callable =
            Wrap<Clock>(std::forward<CallableType>(callable), std::forward<decltype(arguments)>(arguments)...);

        // due to lambda usage number of template instantiation increases
        // leading to slightly longer compilation time
        // but doesnt affect the performance and keeps the code simple
        // coverity[autosar_cpp14_a5_1_7_violation]
        using DelayedTaskType =
            DelayedTask<Clock, decltype(wrapped_callable), ResultType<Clock, CallableType, ArgumentTypes...>>;
        auto task = score::cpp::pmr::make_unique<DelayedTaskType>(memory_resource,
                                                           typename DelayedTaskType::ConstructionGuard{},
                                                           std::forward<decltype(promise)>(promise),
                                                           execution_time_point,
                                                           std::move(wrapped_callable));

        return task;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_DELAYED_TASK_H
