/********************************************************************************
 * Copyright (c) 2022 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2022 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_INVOKE_TRAITS_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_INVOKE_TRAITS_HPP

#include <functional>
#include <type_traits>
#include <utility>

namespace score::cpp
{

namespace detail
{

template <typename F, typename... Args, std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>{}, int> = 0>
constexpr auto try_invoke(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));

template <typename F, typename... Args, std::enable_if_t<std::is_member_pointer<std::decay_t<F>>{}, int> = 0>
constexpr auto try_invoke(F&& f, Args&&... args) -> decltype(std::mem_fn(f)(std::forward<Args>(args)...));

template <typename, typename, typename...>
struct is_invocable_impl : std::false_type
{
};
template <typename F, typename... Args>
struct is_invocable_impl<decltype(
                             static_cast<void>(score::cpp::detail::try_invoke(std::declval<F>(), std::declval<Args>()...))),
                         F,
                         Args...> : std::true_type
{
};

template <typename R>
void expect(R);

template <typename R,
          typename F,
          typename... Args,
          std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>{}, int> = 0>
constexpr auto try_invoke_r(F&& f, Args&&... args)
    -> decltype(expect<R>(std::forward<F>(f)(std::forward<Args>(args)...)));

template <typename R,
          typename F,
          typename... Args,
          std::enable_if_t<std::is_member_pointer<std::decay_t<F>>{}, int> = 0>
constexpr auto try_invoke_r(F&& f, Args&&... args) -> decltype(expect<R>(std::mem_fn(f)(std::forward<Args>(args)...)));

template <typename, typename, typename, typename...>
struct is_invocable_r_impl : std::false_type
{
};

template <typename F, typename... Args>
struct is_invocable_r_impl<decltype(
                               static_cast<void>(score::cpp::detail::try_invoke(std::declval<F>(), std::declval<Args>()...))),
                           void,
                           F,
                           Args...> : std::true_type
{
};

template <typename R, typename F, typename... Args>
struct is_invocable_r_impl<decltype(static_cast<void>(
                               score::cpp::detail::try_invoke_r<R>(std::declval<F>(), std::declval<Args>()...))),
                           R,
                           F,
                           Args...> : std::true_type
{
};

} // namespace detail

/// \brief Determines whether `F` can be invoked with the arguments `Args....`
///
/// https://en.cppreference.com/w/cpp/types/is_invocable
template <typename F, typename... Args>
struct is_invocable : score::cpp::detail::is_invocable_impl<void, F, Args...>
{
};

/// \brief Determines whether `F` can be invoked with the arguments `Args...` to yield a result that is convertible to
/// `R`.
///
/// https://en.cppreference.com/w/cpp/types/is_invocable
///
/// If R is cv void, the result can be any type.
template <typename R, typename F, typename... Args>
struct is_invocable_r : score::cpp::detail::is_invocable_r_impl<void, R, F, Args...>
{
};

namespace detail
{

template <bool, typename, typename...>
struct invoke_result_impl : std::false_type
{
};
template <typename F, typename... Args>
struct invoke_result_impl<true, F, Args...>
{
    using type = decltype(score::cpp::detail::try_invoke(std::declval<F>(), std::declval<Args>()...));
};

} // namespace detail

/// \brief Deduces the return type of an INVOKE expression at compile time.
///
/// https://en.cppreference.com/w/cpp/types/result_of
///
/// \{
template <typename F, typename... Args>
struct invoke_result
{
    using type = typename score::cpp::detail::invoke_result_impl<score::cpp::is_invocable<F, Args...>::value, F, Args...>::type;
};
template <typename F, typename... Args>
using invoke_result_t = typename score::cpp::invoke_result<F, Args...>::type;
/// \}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_INVOKE_TRAITS_HPP
