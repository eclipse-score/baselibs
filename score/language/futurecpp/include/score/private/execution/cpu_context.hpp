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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_CONTEXT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_CONTEXT_HPP

#include <score/private/execution/cpu_scheduler.hpp>
#include <score/private/execution/thread_pool.hpp>
#include <score/private/execution/thread_pool_worker_count.hpp>
#include <score/private/thread/thread_name_hint.hpp>
#include <score/private/thread/thread_stack_size_hint.hpp>
#include <score/memory_resource.hpp>

#include <cstdint>

namespace score::cpp
{
namespace execution
{

/// \brief The `cpu_context` creates a view on a CPU work-stealing thread pool.
///
/// A `cpu_context` must outlive any work launched on it.
///
/// Similar to https://wg21.link/p2079r4 `system_context` but does not represent a system wide context
class cpu_context
{
public:
    using worker_count = detail::thread_pool_worker_count;
    using stack_size_hint = score::cpp::detail::thread_stack_size_hint;
    using name_hint = score::cpp::detail::thread_name_hint;

    /// \brief Constructs a `cpu_context`.
    ///
    /// \param count Number of workers to be created.
    /// \param stack_size Configures the stack size for the worker threads. Defaults to system default stack size.
    /// \param name Configures the name of the worker threads. Defaults to empty name.
    /// \param allocator Allocator used for internal buffers. Defaults to `score::cpp::pmr::get_default_resource()`.
    explicit cpu_context(const worker_count count,
                         const stack_size_hint stack_size = stack_size_hint{0U},
                         const name_hint& name = name_hint{""},
                         const pmr::polymorphic_allocator<>& allocator = {})
        : pool_{count, stack_size, name, allocator}
    {
    }

    /// \brief The `cpu_context` is non-copyable and non-moveable.
    ///
    /// \{
    cpu_context(const cpu_context&) = delete;
    cpu_context(cpu_context&&) = delete;
    cpu_context& operator=(const cpu_context&) = delete;
    cpu_context& operator=(cpu_context&&) = delete;
    /// \}

    /// \brief The `cpu_context` must outlive schedulers obtained from it.
    ///
    /// If there are outstanding schedulers at destruction time, this is undefined behavior.
    ~cpu_context() = default;

    /// \brief Returns a `cpu_scheduler` instance that holds a reference to the `cpu_context`.
    auto get_scheduler() { return cpu_scheduler{pool_}; }

    /// \brief Returns a value representing the maximum number of threads the context may support.
    std::int32_t max_concurrency() const noexcept { return pool_.max_concurrency(); }

private:
    detail::thread_pool pool_;
};

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_CONTEXT_HPP
