/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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

#ifndef SCORE_SERIALIZATION_DETAILS_TYPE_TRAITS_H
#define SCORE_SERIALIZATION_DETAILS_TYPE_TRAITS_H

#include <score/span.hpp>
#include <score/static_vector.hpp>

#include <array>
#include <deque>
#include <list>
#include <type_traits>
#include <vector>

namespace score
{
namespace serialization
{

namespace details
{

/// @brief Base case, we do not support any type besides the ones specialized (listed) below
template <class T>
struct is_supported_sequence_container
{
    static const bool value = false;
};

template <class T, class Alloc>
struct is_supported_sequence_container<std::vector<T, Alloc>>
{
    static const bool value = true;
};

template <class T, class Alloc>
struct is_supported_sequence_container<std::list<T, Alloc>>
{
    static const bool value = true;
};

template <class T, class Alloc>
struct is_supported_sequence_container<std::deque<T, Alloc>>
{
    static const bool value = true;
};

template <class T, std::size_t Size>
struct is_supported_sequence_container<score::cpp::static_vector<T, Size>>
{
    static const bool value = true;
};

template <class T, std::size_t Size>
struct is_supported_sequence_container<std::array<T, Size>>
{
    static const bool value = true;
};

template <class T>
struct is_supported_sequence_container<score::cpp::span<T>>
{
    static const bool value = true;
};

/// @brief Check whether the value_type of a Container is supported
template <bool B>
using bool_constant = std::integral_constant<bool, B>;

template <typename Container, typename SupportedType>
using is_supported = std::is_same<std::remove_const_t<typename Container::value_type>, SupportedType>;

template <typename Container>
using is_supported_value_type =
    bool_constant<is_supported<Container, std::uint8_t>::value || is_supported<Container, char>::value>;

/// @brief Base case, nothing supports emplace back
template <class T>
struct supports_emplace_back
{
    static const bool value = false;
};

template <class T, class Alloc>
struct supports_emplace_back<std::vector<T, Alloc>>
{
    static const bool value = true;
};

template <class T, class Alloc>
struct supports_emplace_back<std::list<T, Alloc>>
{
    static const bool value = true;
};

template <class T, class Alloc>
struct supports_emplace_back<std::deque<T, Alloc>>
{
    static const bool value = true;
};

template <class T, std::size_t Size>
struct supports_emplace_back<score::cpp::static_vector<T, Size>>
{
    static const bool value = true;
};

}  // namespace details

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_DETAILS_TYPE_TRAITS_H
