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
#ifndef SCORE_LIB_CONCURRENCY_SIMPLE_TASK_H
#define SCORE_LIB_CONCURRENCY_SIMPLE_TASK_H

#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_promise.h"
#include "score/concurrency/task.h"
#include "score/concurrency/task_result.h"

#include <score/memory.hpp>
#include <score/memory_resource.hpp>
#include <score/stop_token.hpp>

#include <utility>

namespace score
{
namespace concurrency
{
namespace detail
{

class SimpleTaskBase : public Task
{
  protected:
    // Guard to secure the constructor against public to force people to use SimpleTaskFactory.
    // We need this to still enable the constructor for score::cpp::pmr::make_unique()
    struct ConstructionGuard
    {
    };

  public:
    /**
     * \brief Constructs a SimpleTask from any Callable.
     *
     * \param promise The promise which shall be set once the task is finished
     * \param callable The callable itself
     * \param arguments The arguments
     */
    explicit SimpleTaskBase(ConstructionGuard) noexcept : Task(), stop_source_{} {}

    ~SimpleTaskBase() override = default;

    SimpleTaskBase(const SimpleTaskBase&) = delete;
    SimpleTaskBase& operator=(const SimpleTaskBase&) = delete;

  protected:
    SimpleTaskBase(SimpleTaskBase&&) noexcept = default;
    SimpleTaskBase& operator=(SimpleTaskBase&&) noexcept = default;

  public:
    /**
     * \brief Calls underlying Callable
     * and sets promise if applicable
     *
     * \param token indicates whether the underlying task shall be stopped
     */
    void operator()(const score::cpp::stop_token token) override = 0;

    score::cpp::stop_source GetStopSource() const noexcept override
    {
        return stop_source_;
    }

  private:
    score::cpp::stop_source stop_source_;
};
}  // namespace detail

/**
 * \brief This class wraps any callable. This is necessary for two reasons, we need a clean - non templated - way to
 * pass down callables from the `Executor` interface towards the implementations and second we need some more logic to
 * set a future/promise in case a SimpleTask has been finished.
 */
template <class CallableType, class ResultType>
// false-positive: not a function and the other declaration has a partial template specialization
// coverity[autosar_cpp14_a13_3_1_violation]
class SimpleTask final : public detail::SimpleTaskBase
{
  public:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The "SimpleTask" instance must be created via "SimpleTaskFactory" factory instead of direct constructor call.
    // Thus, access to ConstructionGuard requires.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class SimpleTaskFactory;

    template <class LocalCallableType, class... ArgumentTypes>
    SimpleTask(typename detail::SimpleTaskBase::ConstructionGuard construction_guard,
               concurrency::InterruptiblePromise<ResultType>&& promise,
               LocalCallableType&& callable)
        : detail::SimpleTaskBase(construction_guard),
          callable_{std::forward<decltype(callable)>(callable)},
          promise_{std::forward<decltype(promise)>(promise)}
    {
    }

    void operator()(const score::cpp::stop_token token) override
    {
        score::cpp::ignore = this->promise_.SetValue(this->callable_(token));
    }

  private:
    // due to lambda usage number of template instantiation increases
    // leading to slightly longer compilation time
    // but doesnt affect the performance and keeps the code simple
    // coverity[autosar_cpp14_a5_1_7_violation]
    CallableType callable_;
    concurrency::InterruptiblePromise<ResultType> promise_;
};

/**
 * \brief This class wraps any callable. This is necessary for two reasons, we need a clean - non templated - way to
 * pass down callables from the `Executor` interface towards the implementations and second we need some more logic to
 * set a future/promise in case a SimpleTask has been finished.
 */
template <class CallableType>
class SimpleTask<CallableType, void> final : public detail::SimpleTaskBase
{
  public:
    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The "SimpleTask" instance must be created via "SimpleTaskFactory" factory instead of direct constructor call.
    // Thus, access to ConstructionGuard requires.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class SimpleTaskFactory;

    template <class LocalCallableType>
    SimpleTask(detail::SimpleTaskBase::ConstructionGuard construction_guard,
               concurrency::InterruptiblePromise<void>&& promise,
               LocalCallableType&& callable)
        : detail::SimpleTaskBase(construction_guard),
          callable_{std::forward<decltype(callable)>(callable)},
          promise_{std::move(promise)}
    {
    }

    void operator()(const score::cpp::stop_token token) override
    {
        this->callable_(token);
        score::cpp::ignore = this->promise_.SetValue();
    }

  private:
    // due to lambda usage number of template instantiation increases
    // leading to slightly longer compilation time
    // but doesnt affect the performance and keeps the code simple
    // coverity[autosar_cpp14_a5_1_7_violation]
    CallableType callable_;
    concurrency::InterruptiblePromise<void> promise_;
};

class SimpleTaskFactory final
{
  private:
    template <typename CallableType, typename... ArgumentTypes>
    using result_type = std::invoke_result_t<CallableType, score::cpp::stop_token, ArgumentTypes&&...>;

  public:
    /**
     * \brief Helper function to construct a SimpleTask.
     *
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A SimpleTask constructed from the provided Callable
     */
    template <class CallableType, class... ArgumentTypes>
    static auto Make(score::cpp::pmr::memory_resource* memory_resource, CallableType&& callable, ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<result_type<CallableType, ArgumentTypes...>> promise{};

        return InternalMake(memory_resource,
                            std::move(promise),
                            std::forward<decltype(callable)>(callable),
                            std::forward<decltype(arguments)>(arguments)...);
    }

    /**
     * \brief Helper function to construct a SimpleTask with TaskResult.
     *
     * \tparam CallableType The Callable type that shall be used
     * \tparam ArgumentTypes The argument types of the callable
     * \param callable The callable itself
     * \param arguments The arguments with what the callable shall be invoked
     * \return A tuple of TaskResult and SimpleTask constructed from the provided Callable
     */
    template <class CallableType, class... ArgumentTypes>
    static auto MakeWithTaskResult(score::cpp::pmr::memory_resource* memory_resource,
                                   CallableType&& callable,
                                   ArgumentTypes&&... arguments)
    {
        concurrency::InterruptiblePromise<result_type<CallableType, ArgumentTypes...>> promise{};
        auto future = promise.GetInterruptibleFuture().value();

        auto task = InternalMake(memory_resource,
                                 std::move(promise),
                                 std::forward<decltype(callable)>(callable),
                                 std::forward<decltype(arguments)>(arguments)...);

        // intentional usage; may generate more instantiations, but is harmless
        // coverity[autosar_cpp14_a5_1_7_violation]
        TaskResult<result_type<CallableType, ArgumentTypes...>> task_result{std::move(future), task->GetStopSource()};
        return std::make_pair(std::move(task_result), std::move(task));
    }

  private:
    template <typename CallableType, typename... ArgumentTypes>
    static auto Wrap(CallableType&& callable, ArgumentTypes&&... arguments)
    {
        // intentional usage; may generate more instantiations, but is harmless
        // coverity[autosar_cpp14_a5_1_7_violation]
        // coverity[autosar_cpp14_a18_9_2_violation] false-poisitve
        return [callable = std::forward<decltype(callable)>(callable),
                tuple = std::make_tuple(std::forward<decltype(arguments)>(arguments)...)](
                   const score::cpp::stop_token& token) mutable {
            return score::cpp::apply(std::forward<CallableType>(callable), std::tuple_cat(std::tie(token), tuple));
        };
    }

    template <class PromiseType, class CallableType, class... ArgumentTypes>
    // Suppress "AUTOSAR C++14 A0-1-3" rule finding. This rule states: "Every function defined in an anonymous
    // namespace, or static function with internal linkage, or private member function shall be used.".
    // coverity[autosar_cpp14_a0_1_3_violation] False-positive, method is used in static class methods above
    static auto InternalMake(score::cpp::pmr::memory_resource* memory_resource,
                             PromiseType&& promise,
                             CallableType&& callable,
                             ArgumentTypes&&... arguments)
    {
        auto wrapped_callable =
            Wrap(std::forward<decltype(callable)>(callable), std::forward<decltype(arguments)>(arguments)...);
        // due to lambda usage number of template instantiation increases
        // leading to slightly longer compilation time
        // but doesnt affect the performance and keeps the code simple
        // coverity[autosar_cpp14_a5_1_7_violation]
        using simple_task_type = SimpleTask<decltype(wrapped_callable), result_type<CallableType, ArgumentTypes...>>;
        auto task = score::cpp::pmr::make_unique<simple_task_type>(memory_resource,
                                                            typename simple_task_type::ConstructionGuard{},
                                                            std::forward<decltype(promise)>(promise),
                                                            std::forward<decltype(wrapped_callable)>(wrapped_callable));

        return task;
    }
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_SIMPLE_TASK_H
