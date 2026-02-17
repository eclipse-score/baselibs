/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.TypeMap component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_TYPE_MAP_HPP
#define SCORE_LANGUAGE_FUTURECPP_TYPE_MAP_HPP

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace score::cpp
{

template <typename K, typename V>
struct type_map_entry;

///
/// \tparam Ts pack of type_map_entries
/// \note It is not required to be instantiated, thus no definition is needed - an incomplete type suffices.
///
template <typename... Ts>
struct mapping;

namespace detail
{

template <typename, typename...>
struct extract_value;

template <typename T, typename K, typename V, typename... Tail>
struct extract_value<T, type_map_entry<K, V>, Tail...>
{
    // compare the given type with available entries
    using type =
        std::conditional_t<std::is_same<T, K>::value, type_map_entry<K, V>, typename extract_value<T, Tail...>::type>;
};

template <typename T>
struct extract_value<T>
{
    using type = void;
};

template <typename...>
struct consistency;

template <typename... K, typename... V>
struct consistency<type_map_entry<K, V>...>
{
    template <typename T, size_t N>
    static constexpr T sum(const std::array<T, N>& v)
    {
        T result{};
        for (size_t i{}; i < N; ++i)
        {
            result += v[i];
        }
        return result;
    }
    template <typename T, typename... Ts>
    static constexpr auto make_array(Ts&&... values) -> std::array<T, sizeof...(Ts)>
    {
        return {static_cast<T>(values)...};
    }
    template <typename T, typename... Ts>
    static constexpr auto create_binary_mask()
    {
        return make_array<size_t>(std::is_same<T, Ts>::value...);
    }

    template <typename T, typename... Ts>
    static constexpr size_t is_single_key_unique()
    {
        return static_cast<size_t>(1UL == sum(create_binary_mask<T, Ts...>()));
    }

    static constexpr bool are_all_keys_unique()
    {
        return sizeof...(K) == sum(make_array<size_t>(is_single_key_unique<K, K...>()...));
    }
};

template <typename, typename>
struct type_map;

///
///\brief The following functions will deduct the value for a given type key according to a defined mapping of
/// key-value.
///\tparam KeyType: a type which was defined as a key_type in Entries
///\tparam Mapper: expect to have a packed tuple type containing Entries
///\tparam Entries: a pack of type_map_entry where each key should be unique, otherwise a clash will occur
///
template <typename KeyType, typename... Entries>
struct type_map<KeyType, mapping<Entries...>>
{
    static_assert(consistency<Entries...>::are_all_keys_unique(), "The key-types of the given mapping are not unique");
    using type = typename detail::extract_value<KeyType, Entries...>::type;
    static_assert(!std::is_void<type>::value, "The requested Key was not defined");
};

} // namespace detail

///
/// \tparam K type used as key
/// \tparam V type used as value
///
template <typename K, typename V>
struct type_map_entry
{
    using key_type = K;
    using value_type = V;
};

///
///\tparam KeyType: a type which was defined as a key_type in Entries
///\tparam Entries: a pack of type_map_entry where each key should be unique, otherwise it will not compile
///\note is the associated type_map_entry where KeyType is specified as type_map_entry::key_type. Thus the types of
/// type_map_entry are exposed
template <typename KeyType, typename Mapping>
using type_map_t = typename detail::type_map<KeyType, Mapping>::type;

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_TYPE_MAP_HPP
