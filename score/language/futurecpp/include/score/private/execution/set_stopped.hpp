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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SET_STOPPED_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SET_STOPPED_HPP

#include <score/private/execution/receiver_t.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{

namespace detail
{
namespace set_stopped_t_disable_adl
{

struct set_stopped_t
{
    template <typename Receiver>
    void operator()(Receiver&& r) const
    {
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        constexpr bool is_const_rvalue{std::is_rvalue_reference<decltype(r)>::value &&
                                       std::is_const<std::remove_reference_t<decltype(r)>>::value};
        static_assert(!(std::is_lvalue_reference<decltype(r)>::value || is_const_rvalue), "ill-formed");
        using return_type = decltype(std::forward<Receiver>(r).set_stopped());
        static_assert(std::is_same<void, return_type>::value, "must return void");

        std::forward<Receiver>(r).set_stopped();
    }
};

} // namespace set_stopped_t_disable_adl
} // namespace detail

using detail::set_stopped_t_disable_adl::set_stopped_t;

/// \brief Calls the completion function `set_stopped` on the receiver.
///
/// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r7.html#spec-execution.receivers.set_stopped
///
/// `set_stopped_t` denotes a customization point. Define the customization point as a member function with
/// `set_stopped_t` as the first argument. Follows https://wg21.link/p2855r1.
inline constexpr set_stopped_t set_stopped{};

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SET_STOPPED_HPP
