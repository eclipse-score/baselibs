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
#ifndef SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H
#define SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H

#include "score/concurrency/type_traits.h"

namespace score::concurrency
{

/**
 * @brief RAII helper to temporarily unlock a lockable object.
 *
 * Usage:
 * @code
 * std::mutex m;
 * std::vector<message> messages; //guarded by m
 *
 * std::unique_lock<std::mutex> lock(m); // lock m
 * auto messages_copy{messages}; // copy messages while m is locked
 * bool send_success{false};
 *
 * {
 *     UnlockGuard<std::mutex> ug(lock); // unlocks m
 *     // send without lock
 *     send_success = send(messages_copy);
 * } // lock is locked again when ug goes out of scope
 *
 * if (send_success)
 * {
 *    messages.clear(); // safe to modify messages again because m is locked
 * }
 * @endcode
 *
 * @tparam Lockable Type of the lockable object (e.g., std::mutex, std::shared_mutex, std::unique_lock,
 * std::shared_lock). Must satisfy the BasicLockable concept (is_basic_lockable).
 */
template <typename Lockable, typename = std::enable_if_t<is_basic_lockable_v<Lockable>>>
class UnlockGuard
{
  public:
    /**
     * @brief Holds on to the Lockable and unlocks it if it owns a lock.
     * @pre A lockable without owns_lock() must be locked before constructing the UnlockGuard.
     * @param lockable The Lockable object to be temporarily unlocked.
     */
    explicit UnlockGuard(Lockable& lockable) : lockable_{lockable}, relock_{OwnsLock(lockable)}
    {
        if (relock_)
        {
            lockable_.unlock();
        }
    }
    /**
     * @brief Locks the Lockable again if this guard unlocked it.
     */
    ~UnlockGuard()
    {
        if (relock_)
        {
            lockable_.lock();
        }
    }

    /**
     * @brief Both, copy and move, construction and assignment are not supported.
     *        The UnlockGuard ensures unique ownership of the lockable.
     */
    UnlockGuard(const UnlockGuard&) = delete;
    UnlockGuard(UnlockGuard&&) = delete;
    UnlockGuard& operator=(const UnlockGuard&) = delete;
    UnlockGuard& operator=(UnlockGuard&&) = delete;

  private:
    static bool OwnsLock(const Lockable& lockable)
    {
        if constexpr (LockWithOwnsLock<Lockable>::value)
        {
            return lockable.owns_lock();
        }
        return true;
    }

    Lockable& lockable_;
    bool relock_;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_UNLOCK_GUARD_H
