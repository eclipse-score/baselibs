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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_OP_STATE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_OP_STATE_HPP

#include <score/private/execution/operation_state_t.hpp>
#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/set_stopped.hpp>
#include <score/private/execution/set_value.hpp>
#include <score/private/execution/start.hpp>
#include <score/private/execution/thread_pool.hpp>
#include <score/type_traits.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

template <typename Receiver>
class cpu_scheduler_op_state
{
    static_assert(std::is_object<Receiver>::value, "Receiver is not an object type");
    static_assert(is_receiver<Receiver>::value, "not a receiver");

public:
    using operation_state_concept = operation_state_t;

    template <typename U>
    cpu_scheduler_op_state(U&& receiver, thread_pool& pool) : task_{std::forward<U>(receiver)}, pool_{&pool}
    {
    }

    void start() & { pool_->push(task_); }
    void start() && = delete;

private:
    class task final : public base_task
    {
        template <typename T>
        using is_forwarding_ref_overload_for_task = std::is_same<task, score::cpp::remove_cvref_t<T>>;

    public:
        template <typename U, typename = std::enable_if_t<!is_forwarding_ref_overload_for_task<U>::value>>
        explicit task(U&& receiver) : base_task{}, receiver_{std::forward<U>(receiver)}
        {
        }

        void start() override { score::cpp::execution::set_value(std::move(receiver_)); }
        void disable() override { score::cpp::execution::set_stopped(std::move(receiver_)); }

    private:
        Receiver receiver_;
    };

    task task_;
    thread_pool* pool_;
};

template <typename Receiver>
auto make_cpu_scheduler_op_state(Receiver&& r, thread_pool& p)
{
    using type = detail::cpu_scheduler_op_state<score::cpp::remove_cvref_t<Receiver>>;
    return type{std::forward<Receiver>(r), p};
}

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_CPU_SCHEDULER_OP_STATE_HPP
