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
#ifndef SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_TYPE_TRAITS_H
#define SCORE_COMMON_SERIALIZATION_INCLUDE_SERIALIZATION_VISIT_TYPE_TRAITS_H

#include <string>
#include <type_traits>
#include <utility>

namespace score
{
namespace common
{
namespace visitor
{

namespace detail
{
template <class...>
using void_t = void;
}

template <typename T, typename = void>
struct has_resize : std::false_type
{
};

template <typename T>
struct has_resize<T, detail::void_t<decltype(std::declval<T>().resize(0))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_clear : std::false_type
{
};

template <typename T>
struct has_clear<T, detail::void_t<decltype(std::declval<T>().clear())>> : std::true_type
{
};

template <typename T>
struct is_resizeable
{
    // It is used by is_vector_serializable to enable it or not.
    // coverity[autosar_cpp14_a0_1_1_violation : FALSE]
    static constexpr bool value = ((has_resize<T>::value || has_clear<T>::value) ||
                                   (std::is_default_constructible<T>::value && std::is_copy_assignable<T>::value));
};

template <typename T, typename = void, typename = void>
struct is_vector_serializable : std::false_type
{
};

template <typename T>
struct is_vector_serializable<
    T,
    detail::void_t<typename T::value_type,
                   decltype(std::declval<T>().begin()),
                   decltype(std::declval<T>().cbegin()),
                   decltype(std::declval<T>().size()),
                   decltype(std::declval<T>().push_back(std::declval<typename T::value_type>())),
                   typename std::enable_if<is_resizeable<T>::value>::type>> : std::true_type
{
};

}  // namespace visitor
}  // namespace common
}  // namespace score

#endif
