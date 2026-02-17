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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_MAPLIKE_CONTAINER_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_MAPLIKE_CONTAINER_HPP

#include <type_traits>

namespace score::cpp
{
namespace detail
{

inline constexpr auto is_maplike_container_impl(...) -> std::false_type { return {}; }

template <typename C, typename = typename C::mapped_type>
constexpr auto is_maplike_container_impl(const C&) -> std::true_type
{
    return {};
}
template <typename T>
struct is_maplike_container : decltype(is_maplike_container_impl(std::declval<T>()))
{
};

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_IS_MAPLIKE_CONTAINER_HPP
