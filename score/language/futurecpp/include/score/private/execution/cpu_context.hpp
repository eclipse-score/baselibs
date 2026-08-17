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
    template <typename T>
    using is_attribute = std::disjunction<std::is_same<score::cpp::thread::stack_size_hint, score::cpp::remove_cvref_t<T>>,
                                          std::is_same<score::cpp::thread::priority_hint, score::cpp::remove_cvref_t<T>>,
                                          std::is_same<score::cpp::thread::name_hint, score::cpp::remove_cvref_t<T>>>;

public:
    using worker_count = detail::thread_pool_worker_count;
    using stack_size_hint = score::cpp::detail::thread_stack_size_hint;
    using priority_hint = score::cpp::detail::thread_priority_hint;
    using name_hint = score::cpp::detail::thread_name_hint;

    /// \brief Constructs a `cpu_context`.
    ///
    /// \param allocator Allocator used for internal buffers. Defaults to `score::cpp::pmr::get_default_resource()`.
    /// \param count Number of workers to be created.
    /// \param optional_thread_attributes Supported attributes are stack_size_hint, priority_hint and name_hint.
    /// \{
    template <typename... Attrs, typename = std::enable_if_t<std::conjunction_v<is_attribute<Attrs>...>>>
    explicit cpu_context(const pmr::polymorphic_allocator<>& allocator,
                         const worker_count count,
                         Attrs&&... optional_thread_attributes)
        : pool_{allocator, count, std::forward<Attrs>(optional_thread_attributes)...}
    {
    }
    template <typename... Attrs, typename = std::enable_if_t<std::conjunction_v<is_attribute<Attrs>...>>>
    explicit cpu_context(const worker_count count, Attrs&&... optional_thread_attributes)
        : cpu_context{pmr::polymorphic_allocator<>{}, count, std::forward<Attrs>(optional_thread_attributes)...}
    {
    }
    /// \}

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
