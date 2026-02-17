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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_START_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_START_HPP

#include <score/private/execution/operation_state_t.hpp>

#include <type_traits>

namespace score::cpp
{
namespace execution
{

namespace detail
{
namespace start_t_disable_adl
{

struct start_t
{
    template <typename OperationState>
    void operator()(OperationState& op) const
    {
        static_assert(is_operation_state<OperationState>::value, "not an operation state");
        static_assert(!std::is_rvalue_reference<decltype(op)>::value, "ill-formed");
        using return_type = decltype(op.start());
        static_assert(std::is_same<void, return_type>::value, "must return void");

        op.start();
    }
};

} // namespace start_t_disable_adl
} // namespace detail

using detail::start_t_disable_adl::start_t;

/// \brief Starts the asynchronous operation associated with the operation state object.
///
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r7.html#spec-execution.opstate.start
///
/// `start_t` denotes a customization point. Define the customization point as a member function with `start_t` as
/// the first argument. Follows https://wg21.link/p2855r1.
inline constexpr start_t start{};

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_START_HPP
