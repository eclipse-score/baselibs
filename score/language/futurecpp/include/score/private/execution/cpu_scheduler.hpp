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
/// @file
/// @copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_HPP

#include <score/private/execution/cpu_scheduler_sender.hpp>
#include <score/private/execution/scheduler_t.hpp>
#include <score/private/execution/thread_pool.hpp>

namespace score::cpp
{
namespace execution
{

/// \brief A `cpu_scheduler` is a copyable handle to a `cpu_context`.
///
/// A `cpu_scheduler` has reference semantics with respect to its `cpu_context`. Calling any operation other than the
/// destructor on a system_scheduler after the `cpu_context` it was created from is destroyed is undefined behavior,
/// and that operation may access freed memory.
///
/// Similar to https://wg21.link/p2079r4 `system_scheduler` but does not represent a system wide scheduler
class cpu_scheduler
{
public:
    using scheduler_concept = scheduler_t;

    /// \brief `cpu_scheduler` is not independently constructable, and must be obtained from a `cpu_context`.
    cpu_scheduler() = delete;

    /// \brief `cpu_scheduler` is both move and copy constructable and assignable.
    ///
    /// \{
    cpu_scheduler(const cpu_scheduler&) = default;
    cpu_scheduler(cpu_scheduler&&) = default;
    cpu_scheduler& operator=(const cpu_scheduler&) = default;
    cpu_scheduler& operator=(cpu_scheduler&&) = default;
    ~cpu_scheduler() = default;
    /// \}

    /// \brief Customization point `score::cpp::schedule()`.
    auto schedule() { return detail::cpu_scheduler_sender{*pool_}; }

    /// \brief Two `cpu_scheduler` compare equal if they share the same underlying `cpu_context`.
    friend bool operator==(const cpu_scheduler& lhs, const cpu_scheduler& rhs) noexcept
    {
        return lhs.pool_ == rhs.pool_;
    }

private:
    friend class cpu_context;
    explicit cpu_scheduler(detail::thread_pool& pool) : pool_{&pool} {}

    detail::thread_pool* pool_;
};

/// \brief Two `cpu_scheduler` compare equal if they share the same underlying `cpu_context`.
inline bool operator!=(const cpu_scheduler& lhs, const cpu_scheduler& rhs) noexcept { return !(lhs == rhs); }

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_HPP
