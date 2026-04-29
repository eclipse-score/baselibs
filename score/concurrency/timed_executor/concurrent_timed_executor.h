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
#ifndef SCORE_LIB_CONCURRENCY_CONCURRENT_TIMED_EXECUTOR_H
#define SCORE_LIB_CONCURRENCY_CONCURRENT_TIMED_EXECUTOR_H

#include "score/concurrency/condition_variable.h"
#include "score/concurrency/executor.h"
#include "score/concurrency/timed_executor/timed_executor.h"
#include "score/concurrency/timed_executor/timed_task.h"

#include <score/assert.hpp>
#include <score/deque.hpp>
#include <score/jthread.hpp>
#include <score/memory.hpp>
#include <score/memory_resource.hpp>
#include <score/set.hpp>
#include <score/stop_token.hpp>
#include <score/type_traits.hpp>
#include <score/vector.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <type_traits>

namespace score::concurrency
{

/// @brief This executor tries to implement an earliest start-time first algorithm.
/// @noted Running tasks will _not_ get interrupted. If the concurrency level is to low, this means that tasks will
/// not get executed according to their schedule. After all, this is only a best effort approach, since the OS is at
/// the end the deciding factor, what gets scheduled when.
template <class Clock>
class ConcurrentTimedExecutor final : public TimedExecutor<Clock>
{
  public:
    using TimePoint = std::chrono::time_point<Clock>;

    explicit ConcurrentTimedExecutor(score::cpp::pmr::memory_resource* memory_resource,
                                     score::cpp::pmr::unique_ptr<Executor> executor)
        : TimedExecutor<Clock>{memory_resource}, executor_{std::move(executor)}, queue_{}, free_{}, waiting_{}, mutex_{}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(executor_ != nullptr);
        for (std::size_t counter{0U}; counter < executor_->MaxConcurrencyLevel(); counter++)
        {
            // it's alright since the conditional variable is moved to the functor
            // coverity[autosar_cpp14_a18_5_8_violation]
            auto conditional_variable =
                score::cpp::pmr::make_shared<concurrency::InterruptibleConditionalVariable>(memory_resource);

            // capturing `this` is fine, since executor_ will be destroyed upon destruction of `this`
            executor_->Post(
                [this, conditional_variable = std::move(conditional_variable)](const score::cpp::stop_token token) {
                    // GCOV_EXCL_START: false-positive, covered by ConcurrentTimedExecutorFixture
                    while (not token.stop_requested())
                    // GCOV_EXCL_STOP
                    {
                        Work(token, conditional_variable);
                    }
                });
        }
    }

    ConcurrentTimedExecutor(const ConcurrentTimedExecutor&) = delete;
    ConcurrentTimedExecutor(ConcurrentTimedExecutor&&) noexcept = delete;
    ConcurrentTimedExecutor& operator=(const ConcurrentTimedExecutor&) = delete;
    ConcurrentTimedExecutor& operator=(ConcurrentTimedExecutor&&) noexcept = delete;

    ~ConcurrentTimedExecutor() noexcept override
    {
        executor_.reset();
    }

    [[nodiscard]] std::size_t MaxConcurrencyLevel() const noexcept override
    {
        return executor_->MaxConcurrencyLevel();
    }

    [[nodiscard]] bool ShutdownRequested() const noexcept override
    {
        return executor_->ShutdownRequested();
    }

    void Shutdown() noexcept override
    {
        executor_->Shutdown();
    }

  protected:
    void Enqueue(score::cpp::pmr::unique_ptr<TimedTask<Clock>> task) override
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(task != nullptr, "Contract violation, nullptr as task provided");

        const auto next_execution_point = task->GetNextExecutionPoint();
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(next_execution_point.has_value(),
                                     "Contract violation, no next execution point given.");

        // coverity[autosar_cpp14_a8_5_3_violation] false-positive: not auto
        std::unique_lock lock{mutex_};
        ScheduleAtInternal(std::move(lock), next_execution_point.value(), std::move(task));
    }

  private:
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding. This rule states:"Friend declarations shall not be used.".
    // Justification:
    // We break encapsulation for unit tests, in order to ensure reliable / non-flaky unit tests.
    // It is impossible to implement the tests in a meaningful way having a thread inbetween.
    // Because then it is unclear when the `tick` has been finished. It would be even worse to make the `Work()`
    // function public, thus we decided to make the fixture a friend.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class ConcurrentTimedExecutorFixture;

    // false-positive: used as executors task main body
    // coverity[autosar_cpp14_a0_1_3_violation]
    void Work(const score::cpp::stop_token token,
              std::shared_ptr<concurrency::InterruptibleConditionalVariable> conditional_variable)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(conditional_variable != nullptr);
        score::cpp::pmr::unique_ptr<TimedTask<Clock>> task{};  // LCOV_EXCL_LINE tested via EnsureTaskIsRetrievedAndExecuted
        {
            // coverity[autosar_cpp14_a8_5_3_violation] false-positive: not auto
            std::unique_lock lock{mutex_};
            score::cpp::ignore = free_.emplace(conditional_variable);
            if (not conditional_variable->wait(mutex_, token, [this]() {
                    return not queue_.empty();
                }))
            {
                return;  // got interrupted, shutdown the worker thread
            }
            score::cpp::ignore = free_.erase(conditional_variable);

            auto time_task_pair = std::move(queue_.front());
            const auto next_execution_point = time_task_pair.first;
            task = std::move(time_task_pair.second);
            queue_.pop_front();

            if (Clock::now() < next_execution_point)
            {
                // since we have no good way to figure out if a new task was added, we do not care about spurious
                // wake-ups
                score::cpp::ignore = waiting_.emplace(next_execution_point, conditional_variable);
                score::cpp::ignore = conditional_variable->wait_until(mutex_, token, next_execution_point);
                score::cpp::ignore = waiting_.erase(std::make_pair(next_execution_point, conditional_variable));

                // we always add back to the queue, this handles spurious wake-ups and ensures that if a task
                // with a shorter next_execution_point has been added while waiting, that this will be executed first
                ScheduleAtInternal(std::move(lock), next_execution_point, std::move(task));
                return;
            }
        }

        {
            // coverity[autosar_cpp14_m0_1_9_violation] false-positive: callback is stored into the token
            [[maybe_unused]] score::cpp::stop_callback cb{token, [&task]() noexcept {
                                                       score::cpp::ignore = task->GetStopSource().request_stop();
                                                   }};
            std::invoke(*task, task->GetStopSource().get_token());
        }

        auto next_possible_execution_point = task->GetNextExecutionPoint();
        if (next_possible_execution_point.has_value())
        {
            // here we ensure the contract of ScheduleAtInternal, that task has a next execution point
            // coverity[autosar_cpp14_a8_5_3_violation] false-positive: not auto
            std::unique_lock lock{mutex_};
            ScheduleAtInternal(std::move(lock), next_possible_execution_point.value(), std::move(task));
        }
    }

    void ScheduleAtInternal(std::unique_lock<std::mutex> lock,
                            const TimePoint time_point,
                            score::cpp::pmr::unique_ptr<TimedTask<Clock>> task)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(lock.owns_lock());

        const auto iterator = std::lower_bound(queue_.begin(),
                                               queue_.end(),
                                               std::make_pair(time_point, nullptr),
                                               [](const auto& lhs, const auto& rhs) noexcept -> bool {
                                                   return lhs.first < rhs.first;
                                               });
        score::cpp::ignore = queue_.insert(iterator, std::make_pair(time_point, std::move(task)));
        score::cpp::ignore = WakeUp(std::move(lock), time_point);
    }

    /// \details When adding new tasks to the executor (also if they are rescheduled), we have to consider that there
    /// are multiple threads sleeping and thus waiting already. We do not want to wakeup all threads (`notify_all`) on
    /// one condition variable, since this would lead to a lot of mutex contention. But we cannot wake up only one
    /// thread, since this could lead to cases where threads are waiting for tasks that are executed way later, then
    /// tasks that have shortly been added. That's why this method identifies, which threads wait until which point, and
    /// wakes up the first (most close thread). After that, it's task get's readded (rescheduled) and in this another
    /// thread might be woken up. This way wen ensure that all needed threads are woken up, but only as much as
    /// necessary.
    std::unique_lock<std::mutex> WakeUp(std::unique_lock<std::mutex> lock, TimePoint time_point)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(lock.owns_lock());

        if (not free_.empty())
        {
            free_.begin()->get()->notify_one();
        }
        else
        {
            auto it = std::lower_bound(waiting_.begin(),
                                       waiting_.end(),
                                       std::make_pair(time_point, nullptr),
                                       [](const auto& lhs, const auto& rhs) noexcept -> bool {
                                           return lhs.first < rhs.first;
                                       });
            if (it != waiting_.cend())
            {
                it->second->notify_one();
            }
        }

        return lock;
    }

    score::cpp::pmr::unique_ptr<Executor> executor_;
    score::cpp::pmr::deque<std::pair<TimePoint, score::cpp::pmr::unique_ptr<TimedTask<Clock>>>> queue_;
    score::cpp::pmr::set<std::shared_ptr<concurrency::InterruptibleConditionalVariable>> free_;
    score::cpp::pmr::set<std::pair<TimePoint, std::shared_ptr<concurrency::InterruptibleConditionalVariable>>> waiting_;
    std::mutex mutex_;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_NON_CONCURRENT_TIMED_EXECUTOR_H
