/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_HPP

#include <score/private/container/intrusive_forward_list.hpp>
#include <score/private/execution/thread_pool_queue.hpp>
#include <score/private/execution/thread_pool_worker_count.hpp>
#include <score/private/thread/thread_name_hint.hpp>
#include <score/private/thread/thread_stack_size_hint.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/assert.hpp>
#include <score/jthread.hpp>
#include <score/latch.hpp>
#include <score/memory_resource.hpp>
#include <score/stop_token.hpp>
#include <score/vector.hpp>

#include <atomic>
#include <cstdint>

namespace score::cpp
{
namespace execution
{
namespace detail
{

/// \brief A task for running on the thread pool.
class base_task : public score::cpp::detail::intrusive_forward_list_node
{
public:
    /// \brief Virtual base class dtor.
    virtual ~base_task() = default;

    /// \brief Starts the task.
    ///
    /// The thread pool calls the function to run the task. Once a task has been submitted to the thread pool exactly
    /// one (but not both) of `start` or `disable` is invoked.
    virtual void start() = 0;

    /// \brief Disables the task so it is not started.
    ///
    /// The thread pool calls the function to indicate that the task is not started, because the thread pool is stopped.
    /// Once a task has been submitted to the thread pool exactly one (but not both) of `start` or `disable` is
    /// invoked.
    virtual void disable() = 0;

protected:
    base_task() = default;
    base_task(const base_task&) = default;
    base_task& operator=(const base_task&) = default;
    base_task(base_task&&) = default;
    base_task& operator=(base_task&&) = default;
};

/// \brief Work-stealing style thread pool.
///
/// Each worker is assigned a queue. A Worker will try to steal work from other queues in case its own queue is blocked.
/// If it is not successful at stealing, it then waits on its own queue. Work is distributed between the queues by
/// keeping track of the last queue to receive new work.
class thread_pool
{
public:
    using worker_count = detail::thread_pool_worker_count;
    using stack_size_hint = score::cpp::detail::thread_stack_size_hint;
    using name_hint = score::cpp::detail::thread_name_hint;

    /// \brief Create a new thread pool object using the `allocator` and tuned to the specified options.
    ///
    /// \pre count > 0
    ///
    /// \param count Number of workers to be created.
    /// \param stack_size Stack size for the worker threads. Defaults to system default stack size.
    /// \param name Configures the name of the worker threads. Defaults to empty name.
    /// \param allocator Allocator used for internal buffers. Defaults to `score::cpp::pmr::get_default_resource()`.
    explicit thread_pool(const worker_count count,
                         const stack_size_hint stack_size = stack_size_hint{0U},
                         const name_hint& name = name_hint{""},
                         const score::cpp::pmr::polymorphic_allocator<>& allocator = {})
        : sync_point_{count.value()}
        , worker_count_{[worker_count = count.value()]() {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(worker_count > 0);
            return static_cast<std::uint32_t>(worker_count);
        }()}
        , push_index_{0U}
        , queues_{worker_count_, allocator}
        , threads_{allocator}
    {
        threads_.reserve(worker_count_);

        for (std::uint32_t i{0U}; i < worker_count_; ++i)
        {
            // NOLINTNEXTLINE(performance-unnecessary-value-param)
            static_cast<void>(threads_.emplace_back(
                stack_size, name, [this, index = i](const score::cpp::stop_token token) { work(token, index); }));
        }

        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(worker_count_ == queues_.size());
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(worker_count_ == threads_.size());
    }

    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    thread_pool& operator=(thread_pool&&) = delete;

    /// \brief Destroys the object.
    ///
    /// Requests blocked threads to be released and then joins them.
    ~thread_pool()
    {
        for (auto& t : threads_)
        {
            score::cpp::ignore = t.request_stop();
        }
        for (auto& q : queues_)
        {
            q.abort();
        }
    }

    /// \brief Enqueues a task into one of the available queues.
    void push(base_task& task)
    {
        const auto current = push_index_.fetch_add(1U, std::memory_order_relaxed);
        queues_[current % worker_count_].push(task);
    }

    /// \brief Returns the number of worker threads.
    std::int32_t max_concurrency() const noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(worker_count_ == queues_.size());
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(worker_count_ == threads_.size());
        return static_cast<std::int32_t>(worker_count_);
    }

private:
    void work(const score::cpp::stop_token& token, const std::uint32_t queue_index)
    {
        while (!token.stop_requested())
        {
            base_task* task{nullptr};

            for (std::uint32_t i{0U}; i < worker_count_; ++i)
            {
                task = queues_[(queue_index + i) % worker_count_].try_to_pop();
                if (task != nullptr)
                {
                    break;
                }
            }

            if (task == nullptr) // no work found -> block
            {
                task = queues_[queue_index].pop();
            }

            if (task != nullptr)
            {
                task->start();
            }
        }

        sync_point_.arrive_and_wait(); // wait for all worker threads stop stealing from `queues_[queue_index]`

        base_task* task{queues_[queue_index].try_to_pop()};
        while (task != nullptr)
        {
            task->disable();
            task = queues_[queue_index].try_to_pop();
        }
    }

    score::cpp::latch sync_point_;
    std::uint32_t worker_count_;
    std::atomic<std::uint32_t> push_index_;
    score::cpp::pmr::vector<score::cpp::execution::detail::thread_pool_queue<base_task>> queues_;
    score::cpp::pmr::vector<score::cpp::jthread> threads_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_HPP
