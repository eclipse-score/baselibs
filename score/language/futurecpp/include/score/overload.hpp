/********************************************************************************
 * Copyright (c) 2018 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2018 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Overload component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_OVERLOAD_HPP
#define SCORE_LANGUAGE_FUTURECPP_OVERLOAD_HPP

#include <utility>

namespace score::cpp
{

template <typename... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/// \brief Function for explicitly overloading a set of lambda functions and function objects.
///
/// The returned score::cpp::overloaded object can be called and is especially useful as a visitor, e.g., for variant.
///
/// Based on the proposal: http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0051r2.pdf
template <typename... T>
constexpr overloaded<typename std::decay<T>::type...> overload(T&&... t)
{
    return overloaded<typename std::decay<T>::type...>{std::forward<T>(t)...};
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_OVERLOAD_HPP
