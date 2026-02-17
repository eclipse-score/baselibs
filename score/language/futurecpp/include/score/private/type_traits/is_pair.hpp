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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_PAIR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_PAIR_HPP

#include <type_traits>
#include <utility>

namespace score::cpp
{

/// / \brief Check whether the type is a pair
/// the negative case
/// \tparam T: type to be checked
///
template <typename T>
struct is_pair : std::false_type
{
};

/// / \brief Check whether the type is a pair
/// the positive case
/// \tparam T: type to be checked
///
template <typename T1, typename T2>
struct is_pair<std::pair<T1, T2>> : std::true_type
{
};

/// / \brief Check whether the type is a pair
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr auto is_pair_v = is_pair<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_PAIR_HPP
