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
#ifndef SCORE_LIB_CONCURRENCY_SYNCHRONIZED_QUEUE_H
#define SCORE_LIB_CONCURRENCY_SYNCHRONIZED_QUEUE_H

#include "score/concurrency/notification.h"

#include <score/deque.hpp>

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>

namespace score::concurrency
{
namespace details
{
/// \brief The SharedState class contains shared data between SynchronizedQueue and QueueSender
template <typename T, typename Notification>
class SharedState
{
  public:
    explicit SharedState(std::size_t max_length,
                         score::cpp::pmr::memory_resource* const memory_resource = score::cpp::pmr::new_delete_resource()) noexcept
        : max_queue_length_{max_length}, mutex_{}, queue_{memory_resource}, notification_{}
    {
    }

    ~SharedState() = default;

    // Autosar c++14 M11-0-1 states that "Classes, structs and unions shall not be defined inside functions."
    // this class acts as container to gather a set of internals that are related.
    // coverity[autosar_cpp14_m11_0_1_violation]
    const std::size_t max_queue_length_;
    // coverity[autosar_cpp14_m11_0_1_violation]
    std::mutex mutex_;
    // coverity[autosar_cpp14_m11_0_1_violation]
    score::cpp::pmr::deque<T> queue_;
    // coverity[autosar_cpp14_m11_0_1_violation]
    Notification notification_;
};
}  // namespace details

/// \brief The SynchronizedQueue provides a mechanism of synchronized queue
/// to be abble to write and read queue from one or from different threads
///
/// \details This class provides possibility to create one reader and multiple senders.
/// Reader could be only one. And senders can be pocied or assigned.
/// Queue can be any type specified by parameter <T>
///
/// Class contains SharedState that is shared between reader and senders
/// that helps to synchronize read-write operations with the queue.
/// \tparam T The type which the class is supposed to use as queue type
template <typename T, typename Notification = score::concurrency::Notification>
class SynchronizedQueue final
{
  public:
    explicit SynchronizedQueue(
        std::size_t max_length,
        score::cpp::pmr::memory_resource* const memory_resource = score::cpp::pmr::new_delete_resource()) noexcept
        : queue_shared_state_(std::make_shared<details::SharedState<T, Notification>>(max_length, memory_resource))
    {
    }

    explicit SynchronizedQueue(std::shared_ptr<details::SharedState<T, Notification>> shared_state) noexcept
        : queue_shared_state_(std::move(shared_state))
    {
    }

    SynchronizedQueue(const SynchronizedQueue&) = delete;
    SynchronizedQueue(SynchronizedQueue&&) noexcept = default;
    SynchronizedQueue& operator=(const SynchronizedQueue&) & = delete;
    SynchronizedQueue& operator=(SynchronizedQueue&&) & noexcept = default;

    ~SynchronizedQueue() noexcept = default;

    /// \brief Tries to get an element from the queue until specified timeout_duration has elapsed or the
    /// or stop_token.request_stop() is called.
    ///
    /// \param timeout The maximum time that shall be waited
    /// \param token A stop_token that can abort any wait
    /// \returns value on success read of queue or nullopt if:
    /// - timeout was reached and queue was still empty
    /// - stop_token.request_stop() was called and queue was still empry
    template <class Rep, class Period>
    std::optional<T> Pop(const std::chrono::duration<Rep, Period>& timeout, score::cpp::stop_token token)
    {
        std::optional<T> result{};
        std::unique_lock<std::mutex> lock{queue_shared_state_->mutex_};

        if (queue_shared_state_->queue_.empty())
        {
            lock.unlock();
            if (!queue_shared_state_->notification_.waitForWithAbort(timeout, token))
            {
                return result;
            }
            lock.lock();
        }
        queue_shared_state_->notification_.reset();

        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(!queue_shared_state_->queue_.empty(), "Signal received with empty queue");

        result = std::move(queue_shared_state_->queue_.front());
        queue_shared_state_->queue_.pop_front();

        return result;
    }

    class QueueSender
    {
      protected:
        explicit QueueSender(std::shared_ptr<details::SharedState<T, Notification>> queue) noexcept
            : sync_queue_{std::move(queue)} {};

      public:
        ~QueueSender() noexcept = default;

        QueueSender(QueueSender&& other) noexcept = default;
        QueueSender& operator=(QueueSender&& other) noexcept = default;

        QueueSender(const QueueSender& other) noexcept = default;
        QueueSender& operator=(const QueueSender& other) noexcept = default;

        // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
        // Force "QueueSender" to be created only with "SynchronizedQueue"
        // coverity[autosar_cpp14_a11_3_1_violation]
        friend class SynchronizedQueue;

        /// \brief Tries to push new element into the queue
        ///
        /// \param item Element that should be placed into the queue
        /// \returns true on success write or false if the queue is already full
        bool push(const T& item)
        {
            // false positive: lock returns shared_ptr with automatic storage duration
            // coverity[autosar_cpp14_a18_5_8_violation]
            auto sync_queue_instance = sync_queue_.lock();
            if (sync_queue_instance != nullptr)
            {
                // coverity[autosar_cpp14_m0_1_3_violation] false-positive
                // coverity[autosar_cpp14_m0_1_9_violation] false-positive
                std::lock_guard<std::mutex> lock(sync_queue_instance->mutex_);
                if (sync_queue_instance->queue_.size() < sync_queue_instance->max_queue_length_)
                {
                    sync_queue_instance->queue_.push_back(item);
                    sync_queue_instance->notification_.notify();
                    return true;
                }
            }
            return false;
        }

        /// \brief Tries to push new element into the queue
        ///
        /// \param item Element that should be placed into the queue
        /// \returns true on success write or false if the queue is already full
        bool push(T&& item)
        {
            // false positive: lock returns shared_ptr with automatic storage duration
            // coverity[autosar_cpp14_a18_5_8_violation]
            auto sync_queue_instance = sync_queue_.lock();
            if (sync_queue_instance != nullptr)
            {
                // coverity[autosar_cpp14_m0_1_3_violation] false-positive
                // coverity[autosar_cpp14_m0_1_9_violation] false-positive
                std::lock_guard<std::mutex> lock(sync_queue_instance->mutex_);
                if (sync_queue_instance->queue_.size() < sync_queue_instance->max_queue_length_)
                {
                    sync_queue_instance->queue_.push_back(std::move(item));
                    sync_queue_instance->notification_.notify();
                    return true;
                }
            }
            return false;
        }

      private:
        std::weak_ptr<details::SharedState<T, Notification>> sync_queue_;
    };

    QueueSender CreateSender() const noexcept
    {
        return QueueSender(queue_shared_state_);
    }

  private:
    std::shared_ptr<details::SharedState<T, Notification>> queue_shared_state_;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_SYNCHRONIZED_QUEUE_H
