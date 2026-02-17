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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_RECEIVER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_RECEIVER_HPP

#include <score/private/execution/receiver_t.hpp>
#include <score/private/execution/set_stopped.hpp>
#include <score/private/execution/set_value.hpp>
#include <score/private/functional/invoke.hpp>
#include <score/type_traits.hpp>

#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace execution
{
namespace detail
{

template <typename Receiver, typename Invocable>
class then_receiver
{
    static_assert(std::is_object<Receiver>::value, "Receiver is not an object type");
    static_assert(is_receiver<Receiver>::value, "not a receiver");
    static_assert(std::is_object<Invocable>::value, "Invocable is not an object type");

public:
    using receiver_concept = receiver_t;

    template <typename R, typename I>
    then_receiver(R&& r, I&& i) : r_{std::forward<R>(r)}, i_{std::forward<I>(i)}
    {
    }

    template <typename... Ts,
              typename std::enable_if_t<std::is_void<score::cpp::invoke_result_t<Invocable, Ts...>>::value, bool> = true>
    void set_value(Ts&&... v) &&
    {
        score::cpp::detail::invoke(std::move(i_), std::forward<Ts>(v)...);
        score::cpp::execution::set_value(std::move(r_));
    }

    template <typename... Ts,
              typename std::enable_if_t<!std::is_void<score::cpp::invoke_result_t<Invocable, Ts...>>::value, bool> = true>
    void set_value(Ts&&... v) &&
    {
        score::cpp::execution::set_value(std::move(r_), score::cpp::detail::invoke(std::move(i_), std::forward<Ts>(v)...));
    }

    void set_stopped() && { score::cpp::execution::set_stopped(std::move(r_)); }

private:
    Receiver r_;
    Invocable i_;
};

template <typename Receiver, typename Invocable>
auto make_then_receiver(Receiver&& r, Invocable&& i)
{
    using type = then_receiver<score::cpp::remove_cvref_t<Receiver>, score::cpp::remove_cvref_t<Invocable>>;
    return type{std::forward<Receiver>(r), std::forward<Invocable>(i)};
}

} // namespace detail
} // namespace execution
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_EXECUTION_THEN_RECEIVER_HPP
