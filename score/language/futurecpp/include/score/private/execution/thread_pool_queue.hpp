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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_QUEUE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_QUEUE_HPP

#include <score/private/container/intrusive_forward_list.hpp>
#include <score/private/execution/spin_mutex.hpp>
#include <condition_variable>
#include <mutex>

namespace score::cpp
{
namespace execution
{
namespace detail
{

/// \brief The queue is designed for the `score::cpp::detail::thread_pool`.
///
/// \note The queue in general is a multiple-producer-multiple-consumer queue but it must be ensured that only a single
/// consumer blocks in `pop`. Given an empty queue and two threads blocking in `pop`: When two elements are pushed then
/// only the first thread is unblocked because the queue **was** empty at this time. The second thread still blocks
/// although the queue is not empty. The `thread_pool` solves this by associating one queue to one worker thread
/// and only this thread is doing a blocking `pop`.
///
/// \note The queue ensures that all threads are unblocked in destructor. But the `thread_pool` takes care that no
/// thread is actually using the queue anymore.
template <typename T>
class thread_pool_queue
{
public:
    /// \brief Default construction.
    thread_pool_queue() : mutex_{}, ready_{}, queue_{}, abort_{false} {}

    /// \brief Non-movable and non-copyable.
    /// \{
    thread_pool_queue(const thread_pool_queue&) = delete;
    thread_pool_queue& operator=(const thread_pool_queue&) = delete;
    thread_pool_queue(thread_pool_queue&&) = delete;
    thread_pool_queue& operator=(thread_pool_queue&&) = delete;
    /// \}

    /// \brief Calls `abort()` and unblocks all waiting threads.
    ~thread_pool_queue() { abort(); }

    /// \brief Enqueues an object and unblocks a potentially waiting thread in `pop`.
    ///
    /// \note `task` must be alive until it is popped from the queue or this class is destroyed.
    void push(T& task)
    {
        std::unique_lock<spin_mutex> lock{mutex_};
        const bool was_empty{queue_.empty()};
        queue_.push_back(task);
        lock.unlock(); // avoid blocking the notified thread when notifying with lock held
        if (was_empty)
        {
            ready_.notify_one();
        }
    }

    /// \brief Pops an object or blocks if the queue is empty.
    ///
    /// \return `nullptr` if `abort()` was called. A pointer to the popped object otherwise.
    T* pop()
    {
        std::unique_lock<spin_mutex> lock{mutex_};

        ready_.wait(lock, [this]() { return (!queue_.empty()) || abort_; });

        if (abort_)
        {
            return nullptr;
        }
        T* const task{&queue_.front()};
        queue_.pop_front();
        return task;
    }

    /// \brief Enqueues an object if the queue is not blocked.
    ///
    /// \note `task` must be alive until it is popped from the queue or this class is destroyed.
    ///
    /// \return True if the object was enqueued. False otherwise.
    bool try_to_push(T& task)
    {
        std::unique_lock<spin_mutex> lock{mutex_, std::try_to_lock};
        if (!lock.owns_lock())
        {
            return false;
        }
        const bool was_empty{queue_.empty()};
        queue_.push_back(task);
        lock.unlock(); // avoid blocking the notified thread when notifying with lock held
        if (was_empty)
        {
            ready_.notify_one();
        }
        return true;
    }

    /// \brief Pops an object if the queue is neither blocked nor empty.
    ///
    /// \return A pointer to the popped object, if any. `nulltpr` otherwise.
    T* try_to_pop()
    {
        std::unique_lock<spin_mutex> lock{mutex_, std::try_to_lock};
        if ((!lock.owns_lock()) || queue_.empty())
        {
            return nullptr;
        }
        T* const task{&queue_.front()};
        queue_.pop_front();
        return task;
    }

    /// \brief Unblocks all threads that are waiting on the queue.
    void abort()
    {
        {
            std::unique_lock<spin_mutex> lock{mutex_};
            abort_ = true;
        }
        ready_.notify_all();
    }

private:
    spin_mutex mutex_;
    std::condition_variable_any ready_;
    score::cpp::detail::intrusive_forward_list<T> queue_;
    bool abort_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THREAD_POOL_QUEUE_HPP
