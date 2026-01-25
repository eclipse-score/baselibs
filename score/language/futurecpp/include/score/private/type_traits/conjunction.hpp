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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP

#include <type_traits>

namespace score::cpp
{

/// @brief Forms the "logical AND" conjunction on the sequence of traits Ts....
///
/// \{
template <typename...>
struct conjunction : std::true_type
{
};
template <typename T>
struct conjunction<T> : T
{
};
template <typename T, typename... Ts>
struct conjunction<T, Ts...> : std::conditional_t<static_cast<bool>(T::value), conjunction<Ts...>, T>
{
};
template <typename... Ts>
constexpr bool conjunction_v = conjunction<Ts...>::value;
/// \}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_CONJUNCTION_HPP
