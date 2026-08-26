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
/// \file
/// \brief A collection of serializers for associative STL containers.
/// \details Provides serializers for (unordered) set, (unordered) map, and (unordered) multimap types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H

#include "score/json/internal/writer/vajson/writer/types/array_type.h"
#include "score/json/internal/writer/vajson/writer/types/object_type.h"
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace score
{
namespace json
{
namespace vajson
{

/// \brief Serializes a set of serializable elements
/// \tparam Next type of serializer.
/// \tparam Value Type of value.
/// \tparam Cmp Type of comparison function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] set Set to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, const std::set<Value, Cmp, Alloc>& set) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JArray(set);
}

/// \brief Serializes an unordered set of serializable elements
/// \tparam Next type of serializer.
/// \tparam Value Type of value.
/// \tparam Hash Type of hash function.
/// \tparam Pred Type of predicate function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] set Unordered set to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                const std::unordered_set<Value, Hash, Pred, Alloc>& set) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JArray(set);
}

/// \brief Serializes a map of serializable elements
/// \tparam Next type of serializer.
/// \tparam Key Type of key. Must be convertible to a JKey.
/// \tparam Value Type of value.
/// \tparam Cmp Type of comparison function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] map Map to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, const std::map<Key, Value, Cmp, Alloc>& map) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JObject(map);
}

/// \brief Serializes a map of serializable elements
/// \tparam Next type of serializer.
/// \tparam Key Type of key. Must be convertible to a JKey.
/// \tparam Value Type of value.
/// \tparam Cmp Type of comparison function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] map Map to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, const std::map<Key, Value, Cmp, Alloc>& map) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JObject(map);
}

/// \brief Serializes an unordered map of serializable elements
/// \tparam Next type of serializer.
/// \tparam Key Type of key. Must be convertible to a JKey.
/// \tparam Value Type of value.
/// \tparam Hash Type of hash function.
/// \tparam Pred Type of predicate function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] map Unordered map to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                const std::unordered_map<Key, Value, Hash, Pred, Alloc>& map) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JObject(map);
}

/// \brief Serializes a multimap of serializable elements
/// \tparam Next type of serializer.
/// \tparam Key Type of key. Must be convertible to a JKey.
/// \tparam Value Type of value.
/// \tparam Cmp Type of comparison function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] map Multimap to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, const std::multimap<Key, Value, Cmp, Alloc>& map) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JObject(map);
}

/// \brief Serializes an unordered multimap of serializable elements
/// \tparam Next type of serializer.
/// \tparam Key Type of key. Must be convertible to a JKey.
/// \tparam Value Type of value.
/// \tparam Hash Type of hash function.
/// \tparam Pred Type of predicate function.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] map Unordered multimap to serialize.
/// \return The succeeding serializer.
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                const std::unordered_multimap<Key, Value, Hash, Pred, Alloc>& map) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JObject(map);
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H
