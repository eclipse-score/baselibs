/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
/// \file
/// \copyright Copyright (c) 2025 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Functional Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_BIND_BACK_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_BIND_BACK_HPP

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace detail
{

template <typename FD, typename... BoundArgs>
class bind_back_wrapper
{
public:
    template <typename F, typename... Args>
    constexpr explicit bind_back_wrapper(F&& f, Args&&... args)
        : f_(std::forward<F>(f)), args_(std::forward<Args>(args)...)
    {
    }

    template <typename... CallArgs>
    constexpr decltype(auto)
    operator()(CallArgs&&... call_args) & noexcept(std::is_nothrow_invocable_v<FD&, CallArgs..., BoundArgs&...>)
    {
        static_assert(std::is_invocable_v<FD&, CallArgs..., BoundArgs&...>);

        return std::apply(
            [&](BoundArgs&... bound_args) -> decltype(auto) {
                return std::invoke(f_, std::forward<CallArgs>(call_args)..., bound_args...);
            },
            args_);
    }

    template <typename... CallArgs>
    constexpr decltype(auto) operator()(CallArgs&&... call_args) const& noexcept(
        std::is_nothrow_invocable_v<const FD&, CallArgs..., const BoundArgs&...>)
    {
        static_assert(std::is_invocable_v<const FD&, CallArgs..., const BoundArgs&...>);

        return std::apply(
            [&](const BoundArgs&... bound_args) -> decltype(auto) {
                return std::invoke(f_, std::forward<CallArgs>(call_args)..., bound_args...);
            },
            args_);
    }

    template <typename... CallArgs>
    constexpr decltype(auto)
    operator()(CallArgs&&... call_args) && noexcept(std::is_nothrow_invocable_v<FD, CallArgs..., BoundArgs...>)
    {
        static_assert(std::is_invocable_v<FD, CallArgs..., BoundArgs...>);

        return std::apply(
            [&](BoundArgs&&... bound_args) -> decltype(auto) {
                return std::invoke(std::move(f_), std::forward<CallArgs>(call_args)..., std::move(bound_args)...);
            },
            std::move(args_));
    }

    template <typename... CallArgs>
    constexpr decltype(auto) operator()(CallArgs&&... call_args) const&& noexcept(
        std::is_nothrow_invocable_v<const FD, CallArgs..., const BoundArgs...>)
    {
        static_assert(std::is_invocable_v<const FD, CallArgs..., const BoundArgs...>);

        return std::apply(
            [&](const BoundArgs&&... bound_args) -> decltype(auto) {
                return std::invoke(std::move(f_), std::forward<CallArgs>(call_args)..., std::move(bound_args)...);
            },
            std::move(args_));
    }

private:
    FD f_;
    std::tuple<BoundArgs...> args_;
};

} // namespace detail

/// \brief Generates a perfect forwarding call wrapper which allows to invoke the callable target `f` with its last
/// `sizeof...(Args)` parameters bound to `args`
///
/// \tparam F the type of the callable object
/// \tparam Args the types of the arguments to bind to the last `sizeof...(Args)` parameters of the `Callable` object
/// \param f the `Callable` object that will be bound to some arguments
/// \param args the list of arguments to bind to the last `sizeof...(Args)` parameters of the `Callable` object
/// \return A function object of unspecified type, which can be called with the remaining parameters of the `Callable`
/// object
template <typename F, typename... Args>
constexpr auto bind_back(F&& f, Args&&... args)
{
    using FD = std::decay_t<F>;

    static_assert(std::is_constructible_v<FD, F>);
    static_assert(std::is_move_constructible_v<FD>);
    static_assert((std::is_constructible_v<std::decay_t<Args>, Args> && ...));
    static_assert((std::is_move_constructible_v<std::decay_t<Args>> && ...));

    return detail::bind_back_wrapper<FD, std::decay_t<Args>...>(std::forward<F>(f), std::forward<Args>(args)...);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_FUNCTIONAL_BIND_BACK_HPP
