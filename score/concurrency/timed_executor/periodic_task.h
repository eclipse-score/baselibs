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
#ifndef SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_PERIODIC_TASK_H
#define SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_PERIODIC_TASK_H

#include "score/concurrency/future/error.h"
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
namespace detail
{

/**
 * Base of all periodic tasks. Splitting PeriodicTask over two classes allows us to expose the call-operator taking
 * a condition variable without forcing users to know the type of the callable. This is important for testing when the
 * tester wants to make sure that the periodic task was called with the correct timing.
 * @tparam Clock (must fulfill https://en.cppreference.com/w/cpp/named_req/Clock)
 */
template <class Clock>
class PeriodicTaskBase : public TimedTask<Clock>
{
  public:
    /**
     * \brief Constructs a PeriodicTask. Not for public use, instead use the PeriodicTaskFactory.
     *
     * \param first_execution The point of time the first execution shall occur
     * \param interval The period of the executions following the first one
     * \param wrapped_callable The callable of the task
     */
    PeriodicTaskBase(concurrency::InterruptiblePromise<void>&& promise,
                     typename Clock::time_point first_execution,
                     typename Clock::duration interval)
        : TimedTask<Clock>(),
          cv_{},
          mutex_{},
          promise_{std::move(promise)},
          execution_time_point_{std::move(first_execution)},
          interval_{std::move(interval)},
          stop_source_{}
    {
    }

    ~PeriodicTaskBase() override = default;
    PeriodicTaskBase(const PeriodicTaskBase&) = delete;
    PeriodicTaskBase& operator=(const PeriodicTaskBase&) = delete;

    PeriodicTaskBase(PeriodicTaskBase&&) noexcept = default;
    PeriodicTaskBase& operator=(PeriodicTaskBase&&) noexcept = default;

    /**
     * \brief Calls underlying Callable
     * and sets promise on the last run
     *
     * \param executor The executor calling this function
     * \param token indicates whether the underlying task shall be stopped
     */
    void operator()(const score::cpp::stop_token token) override
    {
        if (not CallCallable(token))
        {
            score::cpp::ignore = stop_source_.request_stop();
            // on last execution we set our promise
            score::cpp::ignore = promise_.SetValue();
            return;
        }

        if (Clock::time_point::max() - interval_ < execution_time_point_)
        {
            // Overflow of time counter impending. We can not log this because logging depends on
            // lib/concurrency. Because there is nothing else we could do to save this situation, simply
            // terminate.
            std::terminate();
        }
        execution_time_point_ += interval_;
    }

    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }

    [[nodiscard]] std::optional<std::chrono::time_point<Clock>> GetNextExecutionPoint() const noexcept override
    {
        if (stop_source_.stop_requested())
        {
            return {};
        }
        return execution_time_point_;
    }

  protected:
    virtual bool CallCallable(const score::cpp::stop_token& token) = 0;

  private:
    InterruptibleConditionalVariable cv_;
    std::mutex mutex_;
    concurrency::InterruptiblePromise<void> promise_;
    typename Clock::time_point execution_time_point_;
    typename Clock::duration interval_;
    score::cpp::stop_source stop_source_;
};
}  // namespace detail

template <class Clock, class CallableType>
class PeriodicTask final : public detail::PeriodicTaskBase<Clock>
{
  private:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Guard to secure the constructor against public to force people to use PeriodicTaskFactory.
    // We need this to still enable the constructor for score::cpp::pmr::make_unique()
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class PeriodicTaskFactory;
    class ConstructionGuard
    {
      public:
        constexpr explicit ConstructionGuard() noexcept = default;
    };

  public:
    /**
     * \brief Constructs a PeriodicTask. Not for public use, instead use the PeriodicTaskFactory.
     *
     * \param promise The promise which shall be set once the task is finished
     * \param first_execution The point of time the first execution shall occur
     * \param interval The period of the executions following the first one
     * \param wrapped_callable The callable of the task
     */
    explicit PeriodicTask(ConstructionGuard,
                          concurrency::InterruptiblePromise<void>&& promise,
                          typename Clock::time_point first_execution,
                          typename Clock::duration interval,
                          CallableType&& callable)
        : detail::PeriodicTaskBase<Clock>{std::move(promise), first_execution, interval}, callable_{std::move(callable)}
    {
    }

  private:
    bool CallCallable(const score::cpp::stop_token& token) override
    {
        const auto time_point = this->GetNextExecutionPoint();
        if (time_point.has_value())
        {
            return callable_(token, time_point.value());
        }
        return false;
    }

    // due to lambda usage number of template instantiation increases
    // leading to slightly longer compilation time
    // but doesnt affect the performance and keeps the code simple
    // coverity[autosar_cpp14_a5_1_7_violation]
    CallableType callable_;
};

class PeriodicTaskFactory
{
  public:
    /**
     * \brief Helper function to construct a PeriodicTask.
     *
     * When providing a callable with return type bool, the execution of the task will stop after the callable returned
     * `false`. If a callable with any other return type is provided, the periodic task will continue indefinitely until
     * the stop token was set.
     *
     * \tparam Clock The clock (must fulfill https://en.cppreference.com/w/cpp/named_req/Clock)
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param memory_resource The memory resource to use internally
     * \param first_execution The point of time the first execution shall occur
     * \param interval The period of the executions following the first one
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A PeriodicTask constructed from the provided Callable
     */
    template <class Clock, class CallableType, class... ArgumentTypes>

    static auto Make(score::cpp::pmr::memory_resource* memory_resource,
                     typename Clock::time_point first_execution,
                     typename Clock::duration interval,
                     CallableType&& callable,
                     ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<void> promise{};

        return InternalMake<Clock>(memory_resource,
                                   std::move(promise),
                                   first_execution,
                                   interval,
                                   std::forward<decltype(callable)>(callable),
                                   std::forward<decltype(arguments)>(arguments)...);
    }

    /**
     * \brief Helper function to construct a PeriodicTask with TaskResult.
     *
     * \tparam Clock The clock (must fulfill https://en.cppreference.com/w/cpp/named_req/Clock)
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param memory_resource The memory resource to use internally
     * \param first_execution The point of time the first execution shall occur
     * \param interval The period of the executions following the first one
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A tuple of TaskResult and PeriodicTask constructed from the provided Callable
     */
    template <class Clock, class CallableType, class... ArgumentTypes>
    static auto MakeWithTaskResult(score::cpp::pmr::memory_resource* memory_resource,
                                   typename Clock::time_point first_execution,
                                   typename Clock::duration interval,
                                   CallableType&& callable,
                                   ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<void> promise{};
        auto future = promise.GetInterruptibleFuture().value();

        auto task = InternalMake<Clock>(memory_resource,
                                        std::move(promise),
                                        first_execution,
                                        interval,
                                        std::forward<decltype(callable)>(callable),
                                        std::forward<decltype(arguments)>(arguments)...);

        TaskResult<void> task_result{std::move(future), task->GetStopSource()};
        return std::make_pair(std::move(task_result), std::move(task));
    }

  private:
    template <typename CallableType, typename Clock>
    // coverity[autosar_cpp14_a0_1_3_violation] false-positive: is used
    constexpr static bool IsReturnOfBoolType()
    {
        return std::is_same<std::invoke_result_t<CallableType(const score::cpp::stop_token&, const typename Clock::time_point)>,
                            bool>::value;
    }

    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    static auto WrapParameters(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-positive: already forwarded
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

    template <typename Clock,
              typename CallableType,
              typename std::enable_if_t<IsReturnOfBoolType<CallableType, Clock>(), bool> = true>
    static auto WrapReturnValue(CallableType&& callable)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-positive: already forwarded
        return [callable = std::forward<decltype(callable)>(callable)](
                   const score::cpp::stop_token& token, const typename Clock::time_point intended_execution) mutable noexcept {
            return score::cpp::apply(std::forward<CallableType>(callable),
                              std::tuple_cat(std::tie(token), std::tie(intended_execution)));
        };
    }

    template <typename Clock,
              typename CallableType,
              typename std::enable_if_t<!IsReturnOfBoolType<CallableType, Clock>(), bool> = true>
    // overload resolution is unambiguous here due to enable_if constraints
    // coverity[autosar_cpp14_a13_3_1_violation]
    static auto WrapReturnValue(CallableType&& callable)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-positive: already forwarded
        return [callable = std::forward<decltype(callable)>(callable)](
                   const score::cpp::stop_token& token, const typename Clock::time_point intended_execution) mutable {
            // coverity[autosar_cpp14_a0_1_2_violation] false-positive
            score::cpp::apply(std::forward<CallableType>(callable),
                       std::tuple_cat(std::tie(token), std::tie(intended_execution)));
            return true;
        };
    }

    template <class Clock, class CallableType, class... ArgumentTypes>
    static auto InternalMake(score::cpp::pmr::memory_resource* memory_resource,
                             concurrency::InterruptiblePromise<void>&& promise,
                             typename Clock::time_point first_execution,
                             typename Clock::duration interval,
                             CallableType&& callable,
                             ArgumentTypes&&... arguments)
    {
        auto wrapped_callable = WrapReturnValue<Clock>(WrapParameters<Clock>(
            std::forward<decltype(callable)>(callable), std::forward<decltype(arguments)>(arguments)...));

        auto task = score::cpp::pmr::make_unique<PeriodicTask<Clock, decltype(wrapped_callable)>>(
            memory_resource,
            // due to lambda usage number of template instantiation increases
            // leading to slightly longer compilation time
            // but doesnt affect the performance and keeps the code simple
            // coverity[autosar_cpp14_a5_1_7_violation]
            typename PeriodicTask<Clock, decltype(wrapped_callable)>::ConstructionGuard{},
            std::forward<decltype(promise)>(promise),
            first_execution,
            interval,
            std::move(wrapped_callable));

        return task;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_PERIODIC_TASK_H
