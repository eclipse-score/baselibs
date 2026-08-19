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
#ifndef SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_H
#define SCORE_LIB_CONCURRENCY_TIMED_EXECUTOR_H

#include "score/concurrency/timed_executor/delayed_task.h"
#include "score/concurrency/timed_executor/periodic_task.h"
#include "score/concurrency/timed_executor/timed_task.h"

#include <score/memory.hpp>
#include <score/memory_resource.hpp>
#include <score/type_traits.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace score::concurrency
{

template <typename Clock>
class TimedExecutor
{
  protected:
    template <typename CallableType, typename... ArgumentTypes>
    // coverity[autosar_cpp14_a0_1_3_violation] false-positive: is used
    constexpr static bool IsInvocable() noexcept
    {
        return score::cpp::is_invocable<CallableType,
                                        const score::cpp::stop_token&,
                                        const typename Clock::time_point,
                                        ArgumentTypes...>::value;
    }

  public:
    explicit TimedExecutor(score::cpp::pmr::memory_resource* memory_resource) : memory_resource_{memory_resource} {};

    virtual ~TimedExecutor() noexcept = default;
    TimedExecutor(const TimedExecutor&) = delete;
    TimedExecutor& operator=(const TimedExecutor&) = delete;

  protected:
    TimedExecutor(TimedExecutor&&) noexcept = default;
    TimedExecutor& operator=(TimedExecutor&&) noexcept = default;

  public:
    /**
     * \return The maximum level of concurrency which can be achieved by this Executor instantiation
     */
    virtual std::size_t MaxConcurrencyLevel() const noexcept = 0;

    /**
     * \brief Indicates whether a shutdown was previously requested
     * \return true if shutdown() was called, false otherwise
     */
    virtual bool ShutdownRequested() const noexcept = 0;

    /**
     * \brief Requests all contained threads to stop, using their respective score::cpp::stop_token.
     *
     * Important: There is _no_ guarantee that the underlying
     * Task implementations listen to the stop_token!
     */
    virtual void Shutdown() noexcept = 0;

    /**
     * \brief Returns the memory resource used by this executor.
     * Allows to use the memory resource of this executor when manually
     * creating a task that is later assigned to this executor.
     * @return The memory resource
     */
    score::cpp::pmr::memory_resource* GetMemoryResource() const noexcept
    {
        return memory_resource_;
    }

    /**
     * \brief Enqueues a Task for execution, without relying on if the task is finished.
     *
     * \note: If the result of the task is important, the
     * future / promise schema has to be implemented on
     * your own. No submit() method is provided.
     *
     * \param task The Task to execute
     */
    void Post(score::cpp::pmr::unique_ptr<TimedTask<Clock>> task)
    {
        Enqueue(std::move(task));
    }

    /**
     * \brief Enqueues a Callable for execution, without relying on its result (fire-and-forget).
     *
     * \tparam CallableType The type of the callable
     * \tparam ArgumentTypes The types of `callable`'s arguments
     * \param callable The callable to execute
     * \param arguments The types to use when executing the callable
     */
    template <class CallableType, class... ArgumentTypes>
    // overload resolution is unambiguous due to SFINAE
    // coverity[autosar_cpp14_a13_3_1_violation]
    std::enable_if_t<IsInvocable<CallableType, ArgumentTypes...>()> Post(const typename Clock::duration interval,
                                                                         CallableType&& callable,
                                                                         ArgumentTypes&&... arguments)
    {
        DoPost(*this,
               Clock::now(),
               interval,
               std::forward<CallableType>(callable),
               std::forward<ArgumentTypes>(arguments)...);
    }

    template <class CallableType, class... ArgumentTypes>
    // overload resolution is unambiguous due to SFINAE
    // coverity[autosar_cpp14_a13_3_1_violation]
    std::enable_if_t<IsInvocable<CallableType, ArgumentTypes...>()> Post(
        const typename Clock::time_point first_execution,
        const typename Clock::duration interval,
        CallableType&& callable,
        ArgumentTypes&&... arguments)
    {
        DoPost(*this,
               first_execution,
               interval,
               std::forward<CallableType>(callable),
               std::forward<ArgumentTypes>(arguments)...);
    }

    template <class CallableType, class... ArgumentTypes>
    std::enable_if_t<IsInvocable<CallableType, ArgumentTypes...>()>
    // overload resolution is unambiguous due to SFINAE
    // coverity[autosar_cpp14_a13_3_1_violation]
    Post(const typename Clock::time_point execution_point, CallableType&& callable, ArgumentTypes&&... arguments)
    {
        DoPost(*this, execution_point, std::forward<CallableType>(callable), std::forward<ArgumentTypes>(arguments)...);
    }

  private:
    virtual void Enqueue(score::cpp::pmr::unique_ptr<TimedTask<Clock>> task) = 0;

    template <class ExecutorType, class CallableType, class... ArgumentTypes>
    static auto DoPost(ExecutorType& executor,
                       const typename Clock::time_point first_execution,
                       const typename Clock::duration interval,
                       CallableType&& callable,
                       ArgumentTypes&&... arguments)
    {
        auto task =
            PeriodicTaskFactory::Make<Clock, CallableType, ArgumentTypes...>(executor.memory_resource_,
                                                                             first_execution,
                                                                             interval,
                                                                             std::forward<CallableType>(callable),
                                                                             std::forward<ArgumentTypes>(arguments)...);
        executor.Enqueue(std::move(task));
    }

    template <class ExecutorType, class CallableType, class... ArgumentTypes>
    // overload resolution is unambiguous due to SFINAE
    // coverity[autosar_cpp14_a13_3_1_violation]
    static auto DoPost(ExecutorType& executor,
                       const typename Clock::time_point execution_point,
                       CallableType&& callable,
                       ArgumentTypes&&... arguments)
    {
        auto task =
            DelayedTaskFactory::Make<Clock, CallableType, ArgumentTypes...>(executor.memory_resource_,
                                                                            execution_point,
                                                                            std::forward<CallableType>(callable),
                                                                            std::forward<ArgumentTypes>(arguments)...);
        executor.Enqueue(std::move(task));
    }

    score::cpp::pmr::memory_resource* memory_resource_;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_EXECUTOR_H
