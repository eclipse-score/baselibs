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
#ifndef SCORE_LIB_SCOPE_EXIT_SCOPE_EXIT_H
#define SCORE_LIB_SCOPE_EXIT_SCOPE_EXIT_H

#include "score/scope_exit/flag_owner.h"

#include <score/callback.hpp>

#include <functional>
#include <type_traits>

namespace score::utils
{

/// \brief Scope guard which calls the provided function on exit.
///
/// Ensures that the callback is only called on destruction of a ScopeExit once by transferring ownership of the
/// callable when moving a ScopeExit object.
///
/// Implements std::experimental::scope_exit (https://en.cppreference.com/w/cpp/experimental/scope_exit.html). The only
/// difference is that we also make ScopeExit move assignable.
template <typename CallbackType = score::cpp::callback<void()>>
class ScopeExit final
{
    static_assert(std::is_invocable_r_v<void, CallbackType>);

  public:
    explicit ScopeExit(CallbackType exit_function)
        : execute_on_destruction_{true}, exit_function_{std::move(exit_function)}
    {
    }

    ~ScopeExit()
    {
        ExecuteExitFunction();
    }

    /// \brief Makes the ScopeExit inactive such that it will not longer call the exit function on destruction.
    void Release() noexcept
    {
        execute_on_destruction_.Clear();
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;

    ScopeExit(ScopeExit&&) noexcept = default;
    ScopeExit& operator=(ScopeExit&& other) noexcept
    {
        if (this != &other)
        {
            ExecuteExitFunction();

            execute_on_destruction_ = std::move(other.execute_on_destruction_);
            exit_function_ = std::move(other.exit_function_);
        }
        return *this;
    }

  private:
    void ExecuteExitFunction()
    {
        if (execute_on_destruction_.IsSet())
        {
            std::invoke(exit_function_);
        }
    }

    FlagOwner execute_on_destruction_{true};
    CallbackType exit_function_;
};

}  // namespace score::utils

#endif  // SCORE_LIB_SCOPE_EXIT_SCOPE_EXIT_H
