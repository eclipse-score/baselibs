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
#ifndef SCORE_LIB_CONCURRENCY_PERIODIC_TASK_H
#define SCORE_LIB_CONCURRENCY_PERIODIC_TASK_H

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
namespace detail
{

/**
 * Base of all periodic tasks. Splitting PeriodicTask over two classes allows us to expose the call-operator taking
 * a condition variable without forcing users to know the type of the callable. This is important for testing when the
 * tester wants to make sure that the periodic task was called with the correct timing.
 * @tparam Clock (must fulfill https://en.cppreference.com/w/cpp/named_req/Clock)
 */
template <class Clock>
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class PeriodicTaskBase : public Task
{
  public:
    /**
     * \brief Constructs a PeriodicTask. Not for public use, instead use the PeriodicTaskFactory.
     *
     * \param promise The promise which shall be set once the task is finished
     * \param first_execution The point of time the first execution shall occur
     * \param interval The period of the executions following the first one
     * \param wrapped_callable The callable of the task
     */
    PeriodicTaskBase(concurrency::InterruptiblePromise<void>&& promise,
                     typename Clock::time_point first_execution,
                     typename Clock::duration interval)
        : Task(),
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
        (*this)(token, cv_);
    }

    /**
     * \brief Calls underlying Callable
     * and sets promise on the last run
     *
     * \note Only exposed for testing purposes
     *
     * \tparam CV The type of the condition variable to be used
     * \param executor The executor calling this function
     * \param token indicates whether the underlying task shall be stopped
     * \param cv The condition variable to use for waiting
     */
    template <typename CV>
    void operator()(const score::cpp::stop_token token, CV& cv)
    {
        while (true)
        {
            if (execution_time_point_ >= Clock::now())
            {
                std::unique_lock<std::mutex> lock{mutex_};

                // Since there is no overload without stop predicate, just return false in the stop predicate to signal
                // that we always want to wait until the time point was reached or the stop token was set.
                score::cpp::ignore = cv.wait_until(lock, token, execution_time_point_, []() noexcept {
                    return false;
                });
            }

            const auto continue_execution = CallCallable(token);

            // Conceptually, we always want at least one iteration to run. Hence, we do not check in the condition of
            // while but after the call to the callable.
            if ((!continue_execution) || token.stop_requested())
            {
                break;
            }

            if (Clock::time_point::max() - this->interval_ < this->execution_time_point_)
            {
                // Overflow of time counter impending. We can not log this because logging depends on lib/concurrency.
                // Because there is nothing else we could do to save this situation, simply terminate. Once we switch to
                // InterruptiblePromise we can set the promise with an error.
                std::terminate();
            }
            this->execution_time_point_ += this->interval_;
        }

        // Only set the promise with the last job
        score::cpp::ignore = promise_.SetValue();
    }

    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }

  protected:
    virtual bool CallCallable(const score::cpp::stop_token& token) = 0;

    typename Clock::time_point GetExecutionTimePoint() const noexcept
    {
        return execution_time_point_;
    }

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
// Suppress "AUTOSAR C++14 M3-2-3" rule finding.
// This rule states: "A type, object or function that is used in multiple translation units shall
// be declared in one and only one file".
// Justification: This is false positive because file header include guards ensures ODR.
// coverity[autosar_cpp14_m3_2_3_violation]
class PeriodicTask final : public detail::PeriodicTaskBase<Clock>
{
  private:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Guard to secure the constructor against public to force people to use PeriodicTaskFactory.
    // We need this to still enable the constructor for score::cpp::pmr::make_unique()
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class PeriodicTaskFactory;
    struct ConstructionGuard
    {
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
        return callable_(token, this->GetExecutionTimePoint());
    }
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
    [[deprecated(
        "SPP_DEPRECATION: Performance Problem! Please use \'concurrency/timed_executor/periodic_task.h\' Note: "
        "requires the usage of a ConcurrentTimedExecutor")]] static auto
    Make(score::cpp::pmr::memory_resource* memory_resource,
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
    [[deprecated(
        "SPP_DEPRECATION: Performance Problem! Please use \'concurrency/timed_executor/periodic_task.h\' Note: "
        "requires the usage of a ConcurrentTimedExecutor")]] static auto
    MakeWithTaskResult(score::cpp::pmr::memory_resource* memory_resource,
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
    template <typename Clock, typename CallableType, typename... ArgumentTypes>
    static auto WrapParameters(CallableType&& callable, ArgumentTypes&&... arguments)
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

    template <typename Clock,
              typename CallableType,
              typename std::enable_if_t<
                  std::is_same<std::invoke_result_t<CallableType(const score::cpp::stop_token&, const typename Clock::time_point)>,
                               bool>::value,
                  bool> = true>
    static auto WrapReturnValue(CallableType&& callable)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-poisitve
        return [callable = std::forward<decltype(callable)>(callable)](
                   const score::cpp::stop_token& token, const typename Clock::time_point intended_execution) mutable {
            return score::cpp::apply(std::forward<CallableType>(callable),
                              std::tuple_cat(std::tie(token), std::tie(intended_execution)));
        };
    }

    template <
        typename Clock,
        typename CallableType,
        typename std::enable_if_t<
            !std::is_same<std::invoke_result_t<CallableType(const score::cpp::stop_token&, const typename Clock::time_point)>,
                          bool>::value,
            bool> = true>
    // overload resolution is unambiguous due to sfinae
    // coverity[autosar_cpp14_a13_3_1_violation]
    static auto WrapReturnValue(CallableType&& callable)
    {
        // coverity[autosar_cpp14_a18_9_2_violation] false-poisitve
        return [callable = std::forward<decltype(callable)>(callable)](
                   const score::cpp::stop_token& token, const typename Clock::time_point intended_execution) mutable {
            // coverity[autosar_cpp14_a0_1_2_violation] apply returns void
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
            std::forward<decltype(wrapped_callable)>(wrapped_callable));

        return task;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_PERIODIC_TASK_H
