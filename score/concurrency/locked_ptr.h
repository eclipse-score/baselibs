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
#ifndef SCORE_LIB_CONCURRENCY_LOCKED_PTR_H
#define SCORE_LIB_CONCURRENCY_LOCKED_PTR_H

#include "score/concurrency/type_traits.h"
#include "score/concurrency/unlock_guard.h"

#include <utility>

namespace score
{
namespace concurrency
{

/**
 * @brief RAII smart pointer that allows for thread-safe access to an object guarded by a lock.
 *        The Lock is held during the lifetime of LockedPtr. On destruction, lock is destroyed which would release the
 * lock. LockedPtr provides pointer-like semantics (operator*, operator->) to access the underlying object.
 *        Additionally, LockedPtr provides an unlock_guard() method that returns an UnlockGuard which can be used to
 * temporarily unlock the Lock while ensuring it gets locked again when the UnlockGuard goes out of scope.
 *
 * @tparam T Type of the object being pointed to.
 * @tparam Lock Type of the lock that manages access to the object. Must satisfy the BasicLockable concept.
 *
 * @details
 * LockedPtr is a smart pointer that combines a raw pointer to an object of type T and a lock of type Lock.
 * The LockedPtr ensures thread-safe access to the pointed-to object.
 *
 * Usage:
 * @code
 * std::mutex m;
 * SomeType obj;
 *
 * {
 *     LockedPtr locked_ptr(&obj, std::unique_lock(m));
 *     // Access obj through locked_ptr while the lock is held
 *     locked_ptr->some_method();
 * } // lock is released when locked_ptr goes out of scope
 * @endcode
 */
template <typename T,
          typename Lock,
          typename =
              std::enable_if_t<std::conjunction_v<is_basic_lockable<Lock>, std::negation<std::is_reference<Lock>>>>>
class LockedPtr
{
  public:
    /**
     * @brief Constructs a LockedPtr that manages the given pointer and lock.
     */
    LockedPtr(T* ptr, Lock lock) : ptr_(ptr), lock_(std::move(lock)) {}

    /**
     * @brief Destructor that destroys the lock thereby releasing the lock.
     */
    ~LockedPtr() = default;

    /**
     * @brief Copy construction and copy assignment are not supported.
     *        The LockedPtr ensures unique ownership of the lock and thus the object.
     */
    LockedPtr(const LockedPtr&) = delete;
    LockedPtr& operator=(const LockedPtr&) = delete;

    /**
     * @brief Move constructor
     * @param other The LockedPtr to move from.
     * @post The moved-from LockedPtr, other, does not point to any object (other == nullptr is true)
     *       and the lock is in a moved-out state.
     */
    LockedPtr(LockedPtr&& other) noexcept(std::is_nothrow_move_constructible_v<Lock>)
        : ptr_(other.ptr_), lock_(std::move(other.lock_))
    {
        other.ptr_ = nullptr;
    }

    /**
     * @brief Move assignment operator
     * @param other The LockedPtr to move from.
     * @post The moved-from LockedPtr, other, does not point to any object (other == nullptr is true)
     *       and the lock is in a moved-out state.
     */
    LockedPtr& operator=(LockedPtr&& other) noexcept(std::is_nothrow_move_assignable_v<Lock>)
    {
        if (this != &other)
        {
            ptr_ = std::exchange(other.ptr_, nullptr);
            lock_ = std::move(other.lock_);
        }

        return *this;
    }

    /**
     * @brief Swaps the contents of this LockedPtr with another.
     * @param other The LockedPtr to swap with.
     */
    void swap(LockedPtr& other) noexcept(std::is_nothrow_swappable_v<Lock>)
    {
        std::swap(ptr_, other.ptr_);
        std::swap(lock_, other.lock_);
    }

    /**
     * @brief Dereference operator to access the underlying object.
     * @return Reference to the underlying object.
     */
    [[nodiscard]] T& operator*()
    {
        return *ptr_;
    }

    /**
     * @brief Dereference operator to access the underlying object.
     * @return Reference to the underlying object.
     */
    [[nodiscard]] const T& operator*() const
    {
        return *ptr_;
    }

    /**
     * @brief Pointer access to the underlying object.
     * @return Pointer to the underlying object.
     */
    [[nodiscard]] T* operator->()
    {
        return ptr_;
    }

    /**
     * @brief Pointer access to the underlying object.
     * @return Pointer to the underlying object.
     */
    [[nodiscard]] const T* operator->() const
    {
        return ptr_;
    }

    /**
     * @brief Gets the raw pointer to the underlying object.
     * @return Pointer to the underlying object.
     * @remarks This does not transfer ownership; the LockedPtr still manages the lock.
     */
    [[nodiscard]] T* get() noexcept
    {
        return ptr_;
    }

    /**
     * @brief Gets the raw pointer to the underlying object.
     * @return Pointer to the underlying object.
     * @remarks This does not transfer ownership; the LockedPtr still manages the lock.
     */
    [[nodiscard]] const T* get() const noexcept
    {
        return ptr_;
    }

    /**
     * @brief Creates an UnlockGuard that temporarily unlocks the Lock.
     * @return An UnlockGuard that unlocks the Lock for its lifetime.
     * @remarks It is Undefined Behaviour if the returned UnlockGuard outlives the LockedPtr.
     *          The UnlockGuard should be destroyed before the LockedPtr is destroyed.
     *          Moving the LockedPtr while an UnlockGuard exists is also Undefined Behaviour.
     */
    [[nodiscard]] UnlockGuard<Lock> unlock_guard()
    {
        return UnlockGuard<Lock>{lock_};
    }

    /**
     * @brief Creates an UnlockGuard that temporarily unlocks the Lock.
     * @return An UnlockGuard that unlocks the Lock for its lifetime.
     * @remarks It is Undefined Behaviour if the returned UnlockGuard outlives the LockedPtr.
     *          The UnlockGuard should be destroyed before the LockedPtr is destroyed.
     *          Moving the LockedPtr while an UnlockGuard exists is also Undefined Behaviour.
     */
    [[nodiscard]] UnlockGuard<Lock> unlock_guard() const
    {
        return UnlockGuard<Lock>{lock_};
    }

    /**
     * @brief Bool conversion operator checks if the LockedPtr is managing a valid pointer.
     * @return true if the LockedPtr is managing a non-null pointer, false otherwise.
     * @note This does not check the state of the Lock as they are held for the lifetime of the LockedPtr.
     */
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return ptr_ != nullptr;
    }

    /**
     * @brief Equality comparison with another LockedPtr.
     * @param other The LockedPtr to compare with.
     * @return true if both LockedPtrs point to the same object, false otherwise.
     * @note The locks of the LockedPtrs are not compared - they are held for the lifetime of the LockedPtr.
     */
    [[nodiscard]] bool operator==(const LockedPtr& other) const noexcept
    {
        return ptr_ == other.ptr_;
    }

    /**
     * @brief Inequality comparison with another LockedPtr.
     * @param other The LockedPtr to compare with.
     * @return true if both LockedPtrs point to different objects, false otherwise.
     * @note The locks of the LockedPtrs are not compared - they are held for the lifetime of the LockedPtr.
     */
    [[nodiscard]] bool operator!=(const LockedPtr& other) const noexcept
    {
        return !(*this == other);
    }

    /**
     * @brief Equality comparison with a raw pointer.
     * @param p The raw pointer to compare with.
     * @return true if the LockedPtr points to the same object as the raw pointer, false otherwise.
     * @note This does not check the state of the Lock as they are held for the lifetime of the LockedPtr.
     */
    [[nodiscard]] bool operator==(const T* p) const noexcept
    {
        return ptr_ == p;
    }

    /**
     * @brief Inequality comparison with a raw pointer.
     * @param p The raw pointer to compare with.
     * @return true if the LockedPtr points to a different object than the raw pointer, false otherwise.
     * @note This does not check the state of the Lock as they are held for the lifetime of the LockedPtr.
     */
    [[nodiscard]] bool operator!=(const T* p) const noexcept
    {
        return !(*this == p);
    }

    /**
     * @brief Equality comparison with nullptr.
     * @param nullptr_t The nullptr to compare with.
     * @return true if the LockedPtr points to nullptr, false otherwise.
     * @note This does not check the state of the Lock.
     */
    [[nodiscard]] bool operator==(const std::nullptr_t&) const noexcept
    {
        return ptr_ == nullptr;
    }

    /**
     * @brief Inequality comparison with nullptr.
     * @param nullptr_t The nullptr to compare with.
     * @return true if the LockedPtr points to a non-nullptr, false otherwise.
     * @note This does not check the state of the Lock.
     */
    [[nodiscard]] bool operator!=(const std::nullptr_t&) const noexcept
    {
        return !(*this == nullptr);
    }

  private:
    T* ptr_;
    mutable Lock lock_;
};

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_LOCKED_PTR_H
