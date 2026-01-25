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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BASIC_OPERATION_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BASIC_OPERATION_HPP

#include <score/private/execution/connect.hpp>
#include <score/private/execution/operation_state_t.hpp>
#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/sender_t.hpp>
#include <score/private/execution/start.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

template <typename Sender, typename Receiver>
class basic_operation
{
    static_assert(std::is_object<Sender>::value, "sender is not an object type");
    static_assert(is_sender<Sender>::value, "not a sender");
    static_assert(std::is_object<Receiver>::value, "receiver is not an object type");
    static_assert(is_receiver<Receiver>::value, "not a receiver");

    static_assert(std::is_object<connect_result_t<Sender, Receiver>>::value, "op-state is not an object type");
    static_assert(is_operation_state<connect_result_t<Sender, Receiver>>::value, "not an operation-state");

public:
    using operation_state_concept = operation_state_t;

    template <typename S, typename R>
    basic_operation(S&& s, R&& r) : op_state_{score::cpp::execution::connect(std::forward<S>(s), std::forward<R>(r))}
    {
    }

    void start() & noexcept { score::cpp::execution::start(op_state_); }
    void start() && = delete;

private:
    connect_result_t<Sender, Receiver> op_state_;
};

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_BASIC_OPERATION_HPP
