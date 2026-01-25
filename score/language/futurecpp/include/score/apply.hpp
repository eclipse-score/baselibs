/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Apply component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_APPLY_HPP
#define SCORE_LANGUAGE_FUTURECPP_APPLY_HPP

#include <score/private/functional/invoke.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

#include <score/for_each_tuple_element.hpp>

namespace score::cpp
{
namespace detail
{

/// \brief Helper function that implements the unpacking of the tuple arguments into
/// the arguments of the Callable object f
template <typename Function, typename Tuple, std::size_t... Is>
constexpr decltype(auto) apply_impl(Function&& f, Tuple&& t, std::index_sequence<Is...>)
{
    return score::cpp::detail::invoke(std::forward<Function>(f), std::get<Is>(std::forward<Tuple>(t))...);
}

} // namespace detail

/// \brief Calls the function `f` with the arguments stored in the tuple like object `t`
///
/// \tparam Function the `Callable` object type
/// \tparam Tuple the tuple-like type which stores the arguments for the `Function`.
/// \param f instance of the `Callable` object
/// \param t instance of the function parameters `Tuple`.
/// \return the result of applying the function object.
template <typename Function, typename Tuple>
constexpr decltype(auto) apply(Function&& f, Tuple&& t)
{
    return score::cpp::detail::apply_impl(std::forward<Function>(f),
                                   std::forward<Tuple>(t),
                                   std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_APPLY_HPP
