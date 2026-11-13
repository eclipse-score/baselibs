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

#include <chrono>
#include <type_traits>
#include <utility>

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

template <typename T, typename Duration = std::chrono::milliseconds, typename = void>
struct is_timed_lockable_for : std::false_type
{
};

template <typename T, typename Duration>
struct is_timed_lockable_for<T,
                             Duration,
                             std::void_t<decltype(static_cast<bool>(std::declval<T&>().try_lock_for(Duration{})))>>
    : is_basic_lockable<T>
{
};

template <typename T, typename Duration = std::chrono::milliseconds>
inline constexpr bool is_timed_lockable_for_v = is_timed_lockable_for<T, Duration>::value;

template <typename T, typename TimePoint = std::chrono::steady_clock::time_point, typename = void>
struct is_timed_lockable_until : std::false_type
{
};

template <typename T, typename TimePoint>
struct is_timed_lockable_until<T,
                               TimePoint,
                               std::void_t<decltype(static_cast<bool>(std::declval<T&>().try_lock_until(TimePoint{})))>>
    : is_basic_lockable<T>
{
};

template <typename T, typename TimePoint = std::chrono::steady_clock::time_point>
inline constexpr bool is_timed_lockable_until_v = is_timed_lockable_until<T, TimePoint>::value;

template <typename T, typename = void>
struct LockWithOwnsLock : std::false_type
{
};

template <typename T>
struct LockWithOwnsLock<T, std::enable_if_t<std::is_same_v<decltype(std::declval<const T&>().owns_lock()), bool>>>
    : is_basic_lockable<T>
{
};

}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_TYPE_TRAITS_H
