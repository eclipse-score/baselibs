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
#ifndef SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H
#define SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H

#include "score/concurrency/locked_ptr.h"
#include "score/concurrency/type_traits.h"

#include <functional>
#include <memory>
#include <mutex>
#include <tuple>
#include <type_traits>

namespace score::concurrency
{

/**
 * @file synchronized.h
 * @brief Small helper providing serialized access to an object.
 *
 * @section what What is Synchronized?
 * Synchronized<T, Lockable> wraps an object of type T together with a Lockable
 * (std::mutex by default) and provides RAII-based, mutex-protected access
 * to that object. All reads and writes are expected to go through lock()
 * or WithLock(), making the relationship between the object and its lock
 * explicit and harder to misuse.
 *
 * @section member_types Member Types
 * - value_type: The type T of the synchronized object.
 * - pointer: LockedPtr<T, std::unique_lock<Lockable>>
 * - const_pointer: LockedPtr<const T, std::unique_lock<Lockable>>
 *
 * @section how How does it work?
 * - Internally stores:
 *     - mutable Lockable mut;
 *     - T obj;
 * - lock():
 *     - Acquires mut via std::unique_lock.
 *     - Returns a pointer (or a const_pointer for const Synchronized)
 *       with the lock held which is released when the returned pointer goes out of scope.
 * - WithLock(Func&& f):
 *     - Acquires the lock.
 *     - Invokes the callable f(pointer) or f(const_pointer) with the lock held.
 *     - The lock is released when f returns (or exits due to an exception).
 *
 * @section usage Usage Example
 * Basic example:
 * @code
 * using score::concurrency::Synchronized;
 *
 * Synchronized<Sender> sender(local_endpoint, remote_endpoint);
 *
 * // Modify
 * bool success = sender.WithLock([message](pointer sp) {
 *     return sp->send(message);
 * });
 *
 * // Read
 * auto total_messages = sender.WithLock([](const_pointer sp) {
 *     return sp->stats.num_successful_messages + sp->stats.num_failed_messages;
 * });
 * @endcode
 *
 * For a longer critical section:
 * @code
 * Synchronized<std::vector<dtc_status>> dtc_statuses;
 *
 * if (auto new_dtcs = receive_dtcs(); !new_dtcs.empty()) {
 *     auto dtcs = dtc_statuses.lock();    // mutex locked
 *     dtcs->reserve(dtcs->size() + new_dtcs.size());
 *     dtcs->insert(dtcs->end(), new_dtcs.begin(), new_dtcs.end());
 * } // dtcs goes out of scope, mutex unlocked
 * @endcode
 *
 * @note
 * - The default Lockable is std::mutex, which is non-recursive.
 *   Locking the same mutex multiple times from the same thread has
 *   undefined behaviour
 *   (see https://en.cppreference.com/w/cpp/thread/mutex/lock).
 * - Therefore, calling lock() / WithLock() re-entrantly on the same
 *   Synchronized instance is not supported and must be avoided.
 *
 * The intended usage is simple, non-reentrant critical sections:
 * @code
 * Synchronized<Foo> f;
 * f.WithLock([](Foo& x) { x.do_something(); });
 * @endcode
 */
template <typename T,
          typename Lockable = std::mutex,
          // std::is_default_constructible_v<Lockable> applies to Mutex Named Requirement
          // but not for non-standard non-default constructible Lockable
          typename = std::enable_if_t<std::conjunction_v<std::is_destructible<Lockable>, is_basic_lockable<Lockable>>>>
class Synchronized
{
  public:
    using value_type = T;
    using pointer = LockedPtr<T, std::unique_lock<Lockable>>;
    using const_pointer = LockedPtr<const T, std::unique_lock<Lockable>>;

    /**
     * @brief Constructs a Synchronized object with a T obj constructed by forwarding t_args to T's constructor.
     * @param t_args Arguments that construct a T.
     */
    template <typename... ArgsToConstructT>
    explicit Synchronized(ArgsToConstructT&&... t_args) noexcept(
        std::is_nothrow_constructible_v<T, ArgsToConstructT...> && std::is_nothrow_constructible_v<Lockable>)
        // std::forward on a parameter pack does not decay arrays
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) false positive
        : mut{}, obj(std::forward<ArgsToConstructT>(t_args)...)
    {
    }

    /**
     * @brief Constructs a Synchronized object with a T obj constructed from an initializer_list.
     * @param ilistToConstructT An initializer_list to construct T.
     */
    template <typename U>
    // Forwarding reference overload intentional for template instantiation
    // coverity[autosar_cpp14_a13_3_1_violation]
    Synchronized(std::initializer_list<U> ilistToConstructT) noexcept(
        std::is_nothrow_constructible_v<T, std::initializer_list<U>> && std::is_nothrow_constructible_v<Lockable>)
        : mut{}, obj(ilistToConstructT)
    {
    }

    /**
     * @brief Piecewise constructor for Synchronized allowing for in-place construction of
     *        both T and Lockable with separate argument lists.
     *
     * @param t_args Arguments for constructing T
     * @param lockable_args Arguments for constructing Lockable
     */
    template <typename... ArgsToConstructT,
              typename... ArgsToConstructLockable,
              typename = std::enable_if_t<std::is_constructible_v<T, ArgsToConstructT...>>,
              typename = std::enable_if_t<std::is_constructible_v<Lockable, ArgsToConstructLockable...>>>
    explicit Synchronized(
        std::piecewise_construct_t,
        std::tuple<ArgsToConstructT...> t_args,
        std::tuple<ArgsToConstructLockable...>
            lockable_args) noexcept(std::is_nothrow_constructible_v<T, ArgsToConstructT...> &&
                                    std::is_nothrow_constructible_v<Lockable, ArgsToConstructLockable...>)
        : mut(std::make_from_tuple<Lockable>(lockable_args)), obj(std::make_from_tuple<T>(t_args))
    {
    }

    ~Synchronized() = default;

    // Explicitly non-copyable and non-movable (by design)
    Synchronized(const Synchronized&) = delete;
    Synchronized(Synchronized&&) = delete;
    Synchronized& operator=(const Synchronized&) = delete;
    Synchronized& operator=(Synchronized&&) = delete;

    /**
     * @brief Locks the internal Lockable to provide a pointer (LockedPtr) with the lock held and a pointer to the
     *        synchronized/guarded T object.
     * @return An RAII pointer that holds the lock for its lifetime and provides a pointer-like interface to the
     *         synchronized/guarded T object.
     * @remarks The returned LockedPtr must not outlive the Synchronized object as it refers on to the internals of
     *          Synchronized.
     */
    [[nodiscard]] pointer lock()
    {
        return pointer{&obj, std::unique_lock{mut}};
    }

    /**
     * @brief Locks the internal Lockable to provide a const_pointer (LockedPtr) with the lock held and a pointer to the
     *         synchronized/guarded const T object.
     * @return An RAII const_pointer that holds the lock for its lifetime and provides a pointer-like interface to the
     *         synchronized/guarded const T object.
     * @remarks The returned LockedPtr must not outlive the Synchronized object as it refers on to the internals of
     *          Synchronized.
     */
    [[nodiscard]] const_pointer lock() const
    {
        return const_pointer{&obj, std::unique_lock{mut}};
    }

    /**
     * @brief Invokes the passed callable f with a pointer (LockedPtr) to the synchronized/guarded T object while
     *        holding the lock.
     * @tparam Func Type of the callable to invoke.
     * @param f The callable to invoke with the locked pointer.
     * @return The result of invoking f.
     * @remarks f has exclusive thread-safe mutable access to the synchronized object.
     * @deprecated Use lock() and the returned pointer's and_then(), transform(), or or_else() instead.
     */
    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func, pointer>>>
    [[deprecated("Use lock() with and_then(), transform(), or or_else() instead.")]]
    auto WithLock(Func&& f)
    {
        return std::invoke(std::forward<Func>(f), lock());
    }

    /**
     * @brief Invokes the passed callable f with a const_pointer (LockedPtr) to the synchronized/guarded const T object
     *        while holding the lock.
     * @tparam Func Type of the callable to invoke.
     * @param f The callable to invoke with the locked const pointer.
     * @return The result of invoking f.
     * @remarks f has exclusive thread-safe const access to the synchronized object.
     * @deprecated Use lock() and the returned const_pointer's and_then(), transform(), or or_else() instead.
     */
    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func, const_pointer>>>
    // Forwarding reference overload intentional: const/non-const dispatch
    // coverity[autosar_cpp14_a13_3_1_violation]
    [[deprecated("Use lock() with and_then(), transform(), or or_else() instead.")]]
    auto WithLock(Func&& f) const
    {
        return std::invoke(std::forward<Func>(f), lock());
    }

  private:
    mutable Lockable mut;
    T obj;
};

}  // namespace score::concurrency

#endif  // SCORE_LIB_CONCURRENCY_SYNCHRONIZED_H
