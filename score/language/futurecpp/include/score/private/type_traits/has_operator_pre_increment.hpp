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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_PRE_INCREMENT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_PRE_INCREMENT_HPP

#include <type_traits>

#include <score/private/type_traits/is_detected.hpp>

namespace score::cpp
{

namespace detail
{

///
/// \brief Check whether T is incrementable
///
/// \tparam T: type to be checked
///
template <class T>
using has_operator_pre_increment_check = decltype(std::declval<T&>() = ++std::declval<T&>());

} // namespace detail

///
/// \brief Check whether T is increment op'tor can be applied to T
///
/// \tparam T: type to be checked
///
template <typename T>
using has_operator_pre_increment = score::cpp::is_detected<detail::has_operator_pre_increment_check, T>;

///
/// \brief Check whether T is increment op'tor can be applied to T
///
/// \tparam T: type to be checked
///
template <typename T>
constexpr bool has_operator_pre_increment_v = has_operator_pre_increment<T>::value;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_HAS_OPERATOR_PRE_INCREMENT_HPP
