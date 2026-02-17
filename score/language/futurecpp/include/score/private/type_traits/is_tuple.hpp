/********************************************************************************
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2017 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_TUPLE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_TUPLE_HPP

#include <tuple>
#include <type_traits>

namespace score::cpp
{

///
/// \brief Check whether the type is a tuple
/// the negative trait
/// \tparam T: type to be checked
///
template <typename T>
struct is_tuple : std::false_type
{
};

/// / \brief Check whether the type is a tuple
/// the positive case
/// \tparam T: type to be checked
///
template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type
{
};

/// / \brief Check whether the type is a tuple
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr auto is_tuple_v = is_tuple<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_TUPLE_HPP
