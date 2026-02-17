/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Latch component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_LATCH_HPP
#define SCORE_LANGUAGE_FUTURECPP_LATCH_HPP

#include <atomic>
#include <condition_variable>
#include <limits>
#include <mutex>

#include <score/assert.hpp>

namespace score::cpp
{

/// \brief The latch class is a downward counter which can be used to synchronize threads.
///
/// The value of the counter is initialized on creation. Threads may block on the latch until the counter is decremented
/// to zero. There is no possibility to increase or reset the counter, which makes the latch a single-use barrier.
class latch
{
public:
    /// \brief Constructs a latch and initializes its internal counter.
    ///
    /// The behavior is undefined if expected is negative or greater than max().
    ///
    /// \param expected The initial value of the internal counter.
    /// \pre 0 <= expected <= max()
    explicit latch(const std::ptrdiff_t expected) : mutex_{}, cv_{}, count_{expected}, done_{expected == 0}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count_ >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(count_ <= max());
    }

    /// \brief Copy constructor is deleted.
    ///
    /// latch is neither copyable nor movable.
    latch(const latch&) = delete;
    /// \brief Copy assignment operator is deleted.
    ///
    /// latch is neither copyable nor movable.
    latch& operator=(const latch&) = delete;
    /// \brief Move constructor is deleted.
    ///
    /// latch is neither copyable nor movable.
    latch(latch&&) = delete;
    /// \brief Move assignment operator is deleted.
    ///
    /// latch is neither copyable nor movable.
    latch& operator=(latch&&) = delete;

    ~latch() = default;

    /// \brief Decrements the internal counter by n.
    ///
    /// \param n The value by which the internal counter is decreased.
    /// \pre If n is greater than the value of the internal counter or is negative, the behavior is undefined.
    void count_down(const std::ptrdiff_t n = 1)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n >= 0);

        const std::ptrdiff_t old{count_.fetch_sub(n, std::memory_order_acq_rel)};
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n <= old);
        if (old == n)
        {
            // See why we still need a lock and a second variable although `count_` is atomic:
            // https://embeddedartistry.com/blog/2022/01/10/remember-to-lock-around-all-stdcondition_variable-variables
            std::unique_lock<std::mutex> lock{mutex_};
            done_ = true;
            cv_.notify_all();
        }
    }

    /// \brief Blocks the calling thread until the internal counter reaches 0.
    ///
    /// If it is zero already, returns immediately.
    void wait() const
    {
        std::unique_lock<std::mutex> lock{mutex_};
        if (done_)
        {
            return;
        }
        cv_.wait(lock, [&done = done_]() { return done; });
    }

    /// \brief Tests if the internal counter equals zero.
    ///
    /// Returns true only if the internal counter has reached zero.
    bool try_wait() const noexcept
    {
        std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock};
        if (!lock.owns_lock())
        {
            return false;
        }
        return done_;
    }

    /// \brief Decrements the internal counter by n and (if necessary) blocks the calling thread until the
    /// counter reaches 0.
    ///
    /// \param n The value by which the internal counter is decreased.
    /// \pre If n is greater than the value of the internal counter or is negative, the behavior is undefined.
    void arrive_and_wait(const std::ptrdiff_t n = 1)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n >= 0);

        const std::ptrdiff_t old{count_.fetch_sub(n, std::memory_order_acq_rel)};
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n <= old);
        if (old == n)
        {
            // See why we still need a lock and a second variable although `count_` is atomic:
            // https://embeddedartistry.com/blog/2022/01/10/remember-to-lock-around-all-stdcondition_variable-variables
            std::unique_lock<std::mutex> lock{mutex_};
            done_ = true;
            cv_.notify_all();
        }
        else
        {
            std::unique_lock<std::mutex> lock{mutex_};
            cv_.wait(lock, [&done = done_]() { return done; });
        }
    }

    /// \brief Returns the maximum value of the internal counter supported by the implementation.
    static constexpr std::ptrdiff_t max() noexcept { return std::numeric_limits<std::ptrdiff_t>::max(); }

private:
    mutable std::mutex mutex_;
    mutable std::condition_variable cv_;
    std::atomic<std::ptrdiff_t> count_;
    bool done_;
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_LATCH_HPP
