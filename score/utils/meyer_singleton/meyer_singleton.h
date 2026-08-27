/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_UTILS_MEYER_SINGLETON_MEYER_SINGLETON_H
#define SCORE_LIB_UTILS_MEYER_SINGLETON_MEYER_SINGLETON_H

#include "score/concurrency/atomic_indirector.h"

#include <score/assert.hpp>

#include <atomic>
#include <functional>
#include <type_traits>

namespace score::singleton
{

/// \brief Enum which is used to represent whether the singleton static object has been initialized and whether it
/// was initialized using GetInstance() or GetInstanceInitializedWithCallable().
///
/// It's made public to allow mocking of the atomic storing the InitializationState.
enum class InitializationState
{
    NOT_INITIALIZED,
    INITIALIZED_WITHOUT_CALLABLE,
    INITIALIZED_WITH_CALLABLE
};

/// \brief Static class which allows creating a singleton in a thread safe manner.
///
/// The potential issues associated with creating singletons in a multithreaded context is described in Ticket-247205.
/// The root cause appears to be a compiler bug in gcc (which is not present in clang):
///   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99613/
///
/// The issue is summarised here:
///
/// Constructing static objects:
///   The standard states that, in the context of initializing a static variable, "If control enters the declaration
///   concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the
///   initialization." (Ref: https://eel.is/c++draft/stmt.dcl#3).
///
/// Destruction sequence of static objects:
///   The standard guarantees (paraphrased to omit the lawyer language) that if a static object is created before
///   another static object, then the second object will be destroyed before the first. (Ref:
///   https://eel.is/c++draft/basic.start.term#4)
///
/// Based on these two points, we consider the following case:
///   Thread A tries to initialize static object X and then static object Y.
///   Thread B tries to initialize static object X.
///
/// There are 3 possible sequences of interest:
///  1. Thread A initializes X, Thread A initializes Y, Thread B reads X.
///  2. Thread A initializes X, Thread B tries to initialize X but it's already being initialized by thread A so it
///  waits for thread A to finish, Thread A initializes Y.
///  3. Thread B initializes X, Thread A tries to initialize X but it's already being initialized by thread B so it
///  waits for thread B to finish, Thread A initializes Y.
///
/// In all these cases, from the perspective of both threads, X was initialized before Y and therefore Y should be
/// destroyed before X. However, in practice, we see that sometimes although both threads see that X was initialized
/// before Y, X is still destroyed before Y. This can lead to a crash if Y relies on X in its destruction sequence.
///
/// Workaround:
///
/// Atomic operations tagged memory_order_seq_cst (default) order memory such that everything that happened-before a
/// store in one thread becomes a visible side effect in the thread that did a load. (Ref:
/// https://en.cppreference.com/w/c/atomic/memory_order.html) This means that memory / operations cannot be reordered
/// around the atomic load. I.e. if X is initialized before the atomic load and Y is constructed after the atomic
/// load, then the creation order of these two cannot be reordered.
///
/// We therefore store an atomic pointer to the static object which is loaded by any thread which wants to access the
/// static object, creating a compiler barrier / fence which prevents reordering.
template <typename Object, template <class> class AtomicIndirectorType = concurrency::AtomicIndirectorReal>
class MeyerSingleton
{
    // The point of a singleton is to have a single instance per process! Therefore, the object type should not be
    // copyable / movable.
    static_assert(!std::is_copy_constructible_v<Object>);
    static_assert(!std::is_copy_assignable_v<Object>);
    static_assert(!std::is_move_constructible_v<Object>);
    static_assert(!std::is_move_assignable_v<Object>);

  public:
    /// \brief Initializes static singleton object in thread safe manner on first call and returns reference to
    /// singleton object
    ///
    /// Ordering of static object creation is guaranteed around a call to this function with respect to destruction
    /// order. I.e. any static object initialized before calling this function will be destroyed after the static object
    /// returned by this function and vice versa (even if this function is called from different threads).
    ///
    /// \pre For a given Object singleton, GetInstance() cannot be called after calling
    /// GetInstanceInitializedWithCallable()
    template <typename... Args>
    static Object& GetInstance(Args&&... args)
    {
        // Try setting the intialization state to INITIALIZED_WITHOUT_CALLABLE while ensuring that the state is not
        // already set to INITIALIZED_WITH_CALLABLE (which would happen if another thread has called of is calling
        // GetInstanceInitializedWithCallable())
        constexpr auto desired_new_state = InitializationState::INITIALIZED_WITHOUT_CALLABLE;
        constexpr auto disallowed_current_state = InitializationState::INITIALIZED_WITH_CALLABLE;
        TrySettingSingletonInitializationState(desired_new_state, disallowed_current_state);

        return *(GetSingletonWithFence(std::forward<Args>(args)...).load());
    }

    /// \brief Same as GetInstance above except can initialize underlying static object with callable
    ///
    /// Callable will only be called once when initializing the underlying static object
    /// \pre For a given Object singleton, GetInstanceInitializedWithCallable() cannot be called after calling
    /// GetInstance()
    template <typename InitializationCallable>
    static Object& GetInstanceInitializedWithCallable(InitializationCallable&& callable)
    {
        // Due to what appears to be a compiler bug (https://github.com/llvm/llvm-project/issues/55346), using
        // std::is_invocable_r with a non-moveable type fails (since the implementation uses is_convertible to check the
        // return type which always returns false for non-movable types). Therefore, we check that the callable is
        // invocable and the return type is of type Object explicitly.
        static_assert(std::is_invocable_v<InitializationCallable>);
        static_assert(std::is_same_v<Object, std::invoke_result_t<InitializationCallable>>);

        constexpr auto desired_new_state = InitializationState::INITIALIZED_WITH_CALLABLE;
        constexpr auto disallowed_current_state = InitializationState::INITIALIZED_WITHOUT_CALLABLE;
        TrySettingSingletonInitializationState(desired_new_state, disallowed_current_state);

        return *(GetSingletonWithFenceCallable(std::forward<InitializationCallable>(callable)).load());
    }

  private:
    /// \brief Tries to set the value of the InitializationState to the provided value while asserting in a thread safe
    /// manner that the current value is not the disallowed value.
    ///
    /// This function is used to ensure that GetInstance() and GetInstanceInitializedWithCallable() are not called for
    /// the same singleton object.
    static void TrySettingSingletonInitializationState(const InitializationState desired_new_state,
                                                       const InitializationState disallowed_state)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(
            (desired_new_state == InitializationState::INITIALIZED_WITHOUT_CALLABLE) ||
            (desired_new_state == InitializationState::INITIALIZED_WITH_CALLABLE));

        auto current_initialization_state =
            AtomicIndirectorType<InitializationState>::load(singleton_initialization_state_);
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            current_initialization_state != disallowed_state,
            "GetInstance() can not be called after calling GetInstanceInitializedWithCallable (or vice versa)!");

        const auto exchange_result = AtomicIndirectorType<InitializationState>::compare_exchange_strong(
            singleton_initialization_state_, current_initialization_state, desired_new_state);

        // If another thread changed the initialization state in the meantime, then it should only have been set to
        // desired_new_state by another thread calling GetInstance or GetInstanceInitializedWithCallable (the same
        // function that is calling this function). There are no code code paths which can set it to NOT_INITIALIZED
        // (this is just the initial value) and GetInstance() is not allowed to be called after calling
        // GetInstanceInitializedWithCallable, or vice versa (which would set the value to disallowed_state).
        if (!exchange_result)
        {
            SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
                current_initialization_state != disallowed_state,
                "GetInstance() can not be called after calling GetInstanceInitializedWithCallable (or vice versa)!");
            SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
                current_initialization_state != InitializationState::NOT_INITIALIZED,
                "Defensive programming: No code path will set the value to NOT_INITIALIZED after construction. This is "
                "either a memory corruption or a programming bug!");
        }
        // At this point, the value of singleton_initialization_state_ can only be desired_new_state (either set
        // by this thread or another thread in parallel).
    }

    template <typename... Args>
    static std::atomic<Object*>& GetSingletonWithFence(Args&&... args)
    {
        // score-local-static-variables: non-const static storage is required to fulfill the singleton pattern. The
        // static object has internal linkage and is accessed via an atomic pointer to ensure that accessing it is
        // thread safe with its creation. coverity[autosar_cpp14_a3_3_2_violation]
        static std::atomic<Object*> resource{InitializeSingleton(std::forward<Args>(args)...)};
        return resource;
    }

    template <typename... Args>
    static Object* InitializeSingleton(Args&&... args)
    {
        // score-local-static-variables: non-const static storage is required to fulfill the singleton pattern. The
        // static object has internal linkage and is accessed via an atomic pointer to ensure that accessing it is
        // thread safe with its creation. coverity[autosar_cpp14_a3_3_2_violation]
        static Object resource{std::forward<Args>(args)...};
        return &resource;
    }

    template <typename InitializationCallable>
    static std::atomic<Object*>& GetSingletonWithFenceCallable(InitializationCallable&& callable)
    {
        // score-local-static-variables: non-const static storage is required to fulfill the singleton pattern. The
        // static object has internal linkage and is accessed via an atomic pointer to ensure that accessing it is
        // thread safe with its creation. coverity[autosar_cpp14_a3_3_2_violation]
        static std::atomic<Object*> resource{
            InitializeSingletonCallable(std::forward<InitializationCallable>(callable))};
        return resource;
    }

    template <typename InitializationCallable>
    static Object* InitializeSingletonCallable(InitializationCallable&& callable)
    {
        // score-local-static-variables: non-const static storage is required to fulfill the singleton pattern. The
        // static object has internal linkage and is accessed via an atomic pointer to ensure that accessing it is
        // thread safe with its creation. coverity[autosar_cpp14_a3_3_2_violation]
        static Object resource{std::invoke(callable)};
        return &resource;
    }

    static std::atomic<InitializationState> singleton_initialization_state_;
};
template <typename Object, template <class> class AtomicIndirectorType>
std::atomic<InitializationState> MeyerSingleton<Object, AtomicIndirectorType>::singleton_initialization_state_{};

}  // namespace score::singleton

#endif  // SCORE_LIB_UTILS_MEYER_SINGLETON_MEYER_SINGLETON_H
