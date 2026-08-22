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
#ifndef SCORE_LIB_CONCURRENCY_TYPE_TRAITS_H
#define SCORE_LIB_CONCURRENCY_TYPE_TRAITS_H

#include <score/optional.hpp>

#include <type_traits>

namespace score
{

template <typename T, typename = void>
struct is_basic_lockable : std::false_type
{
};

template <typename T>
struct is_basic_lockable<T, std::void_t<decltype(std::declval<T&>().lock()), decltype(std::declval<T&>().unlock())>>
    : std::true_type
{
};

template <typename T>
inline constexpr bool is_basic_lockable_v = is_basic_lockable<T>::value;
template <typename T>
using is_basic_lockable_t = typename is_basic_lockable<T>::type;

}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TYPE_TRAITS_H
