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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_SENDER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_SENDER_HPP

#include <score/private/execution/basic_operation.hpp>
#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/sender_t.hpp>
#include <score/private/execution/then_receiver.hpp>
#include <score/type_traits.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

template <typename Sender, typename Invocable>
class then_sender
{
    static_assert(std::is_object<Sender>::value, "Sender is not an object type");
    static_assert(is_sender<Sender>::value, "not a sender");
    static_assert(std::is_object<Invocable>::value, "Invocable is not an object type");

public:
    using sender_concept = sender_t;

    template <typename S, typename I>
    then_sender(S&& s, I&& i) : s_{std::forward<S>(s)}, i_{std::forward<I>(i)}
    {
    }

    template <typename Receiver>
    auto connect(Receiver&& r) & -> basic_operation<Sender, then_receiver<score::cpp::remove_cvref_t<Receiver>, Invocable>>
    {
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        return {s_, score::cpp::execution::detail::make_then_receiver(std::forward<Receiver>(r), i_)};
    }

    template <typename Receiver>
    auto connect(Receiver&& r) && -> basic_operation<Sender, then_receiver<score::cpp::remove_cvref_t<Receiver>, Invocable>>
    {
        static_assert(is_receiver<Receiver>::value, "not a receiver");
        return {std::move(s_), score::cpp::execution::detail::make_then_receiver(std::forward<Receiver>(r), std::move(i_))};
    }

private:
    Sender s_;
    Invocable i_;
};

template <typename Sender, typename Invocable>
auto make_then_sender(Sender&& s, Invocable&& i)
{
    using type = then_sender<score::cpp::remove_cvref_t<Sender>, score::cpp::remove_cvref_t<Invocable>>;
    return type{std::forward<Sender>(s), std::forward<Invocable>(i)};
}

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_SENDER_HPP
