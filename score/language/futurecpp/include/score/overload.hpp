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

#include <type_traits>
#include <utility>
#include <score/type_traits.hpp>

namespace score::cpp
{

// using pack expansion is only available with C++17. Thus it is reimplemented as a recursive solution.
// Disable doxygen for this code part because it complains about a potential infinite recursion.
//! \cond Doxygen_Suppress

template <typename T, typename... Ts>
struct overloaded : T, overloaded<Ts...>
{
    using T::operator();
    using overloaded<Ts...>::operator();

    template <typename U, typename... Us>
    constexpr explicit overloaded(U&& u, Us&&... us) : T{std::forward<U>(u)}, overloaded<Ts...>{std::forward<Us>(us)...}
    {
    }
};

template <typename T>
struct overloaded<T> : T
{
private:
    template <typename U>
    using is_forwarding_ref_overload_for_overloaded = std::is_same<overloaded, score::cpp::remove_cvref_t<U>>;

public:
    using T::operator();

    template <typename U, typename = std::enable_if_t<!is_forwarding_ref_overload_for_overloaded<U>::value>>
    constexpr explicit overloaded(U&& u) : T{std::forward<U>(u)}
    {
    }
};

//! \endcond

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
