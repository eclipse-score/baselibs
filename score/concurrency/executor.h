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
#ifndef SCORE_LIB_CONCURRENCY_EXECUTOR_H
#define SCORE_LIB_CONCURRENCY_EXECUTOR_H

#include "score/concurrency/simple_task.h"
#include "score/concurrency/task.h"

#include <score/memory.hpp>
#include <score/memory_resource.hpp>
#include <score/type_traits.hpp>

#include <memory>
#include <type_traits>
#include <utility>

namespace score
{
namespace concurrency
{

/**
 * \brief The Executor represents the interface for any execution policy.
 * It enables to post or submit any Callable or Task to be asynchronously
 * scheduled by an explicit execution policy (e.g. a ThreadPool).
 *
 * The Executor is not copieable, but moveable.
 */
class Executor
{
  protected:
    template <typename CallableType, typename... ArgumentTypes>
    using EnableIfIsCallable = typename std::enable_if<
        score::cpp::is_invocable<CallableType, const score::cpp::stop_token&, ArgumentTypes...>::value>::type;

  public:
    explicit Executor(score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource())
        : memory_resource_{memory_resource} {};
    virtual ~Executor() noexcept = default;
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

  protected:
    Executor(Executor&&) noexcept = default;
    Executor& operator=(Executor&&) noexcept = default;

  public:
    /**
     * \return The maximumConcurrencyLevel of this Executor instantiation
     */
    virtual std::size_t MaxConcurrencyLevel() const noexcept = 0;

    /**
     * \brief Indicates whether a shutdown was previously requested
     * \return true if shutdown() was called, false otherwise
     */
    virtual bool ShutdownRequested() const noexcept = 0;

    /**
     * \brief Requests all contained threads to stop,
     * using their respective score::cpp::stop_token.
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
    score::cpp::pmr::memory_resource* GetMemoryResource() noexcept
    {
        return memory_resource_;
    }

    /**
     * \brief Enqueues a Task for execution, without
     * relying on if the task is finished.
     *
     * Note: If the result of the task is important, the
     * future / promise schema has to be implemented on
     * your own. No submit() method is provided.
     *
     * \param task The Task to execute
     */
    void Post(score::cpp::pmr::unique_ptr<Task> task)
    {
        Enqueue(std::move(task));
    }

    /**
     * \brief Enqueues a Callable for execution, without
     * relying on its result (fire-and-forget).
     *
     * \tparam CallableType The type of the callable
     * \tparam ArgumentTypes The types of the callables arguments
     * \param callable The callable to execute
     * \param arguments The types to use when executing the callable
     */
    template <class CallableType, class... ArgumentTypes, typename = EnableIfIsCallable<CallableType, ArgumentTypes...>>
    // overload resolution is unambiguous due to sfinae
    // coverity[autosar_cpp14_a13_3_1_violation]
    void Post(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        DoPost(*this, std::forward<CallableType>(callable), std::forward<ArgumentTypes>(arguments)...);
    }

    /**
     * \brief Enqueues a Callable for execution while expecting
     * it to return a result.
     *
     * \tparam CallableType The type of the callable
     * \tparam ArgumentTypes The types of the callables arguments
     * \param callable The callable to execute
     * \param arguments The types to use when executing the callable
     * \return A std::future of the Callables return value, which will be set
     * once the callable finished its execution.
     */
    template <class CallableType, class... ArgumentTypes, typename = EnableIfIsCallable<CallableType, ArgumentTypes...>>
    auto Submit(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        return DoSubmit(*this, std::forward<CallableType>(callable), std::forward<ArgumentTypes>(arguments)...);
    }

  protected:
    virtual void Enqueue(score::cpp::pmr::unique_ptr<Task> task) = 0;

    template <class ExecutorType,
              class CallableType,
              class... ArgumentTypes,
              typename = EnableIfIsCallable<CallableType, ArgumentTypes...>>
    // coverity[autosar_cpp14_a7_1_8_violation] false-positive
    static void DoPost(ExecutorType& executor, CallableType&& callable, ArgumentTypes&&... arguments)
    {
        auto task = SimpleTaskFactory::Make(
            executor.memory_resource_, std::forward<CallableType>(callable), std::forward<ArgumentTypes>(arguments)...);
        executor.Enqueue(std::move(task));
    }

    template <class ExecutorType,
              class CallableType,
              class... ArgumentTypes,
              typename = EnableIfIsCallable<CallableType, ArgumentTypes...>>
    // coverity[autosar_cpp14_a7_1_8_violation] false-positive
    static auto DoSubmit(ExecutorType& executor, CallableType&& callable, ArgumentTypes&&... arguments)
    {
        auto task = SimpleTaskFactory::MakeWithTaskResult(
            executor.memory_resource_, std::forward<CallableType>(callable), std::forward<ArgumentTypes>(arguments)...);
        executor.Enqueue(std::move(task.second));
        return std::move(task.first);
    }

  private:
    score::cpp::pmr::memory_resource* memory_resource_;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_EXECUTOR_H
