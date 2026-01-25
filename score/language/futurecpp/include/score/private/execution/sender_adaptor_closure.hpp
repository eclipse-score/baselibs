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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP

#include <score/private/execution/sender_t.hpp>
#include <score/private/type_traits/is_derived_from.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{

template <typename Sender>
struct sender_adaptor_closure
{
};

template <typename Sender>
using is_sender_adaptor_closure =
    score::cpp::detail::is_derived_from<std::decay_t<Sender>, sender_adaptor_closure<std::decay_t<Sender>>>;

template <typename Sender,
          typename Closure,
          typename = std::enable_if_t<is_sender<Sender>::value && is_sender_adaptor_closure<Closure>::value>>
auto operator|(Sender&& sender, Closure&& closure)
{
    return std::forward<Closure>(closure)(std::forward<Sender>(sender));
}

} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_SENDER_ADAPTOR_CLOSURE_HPP
