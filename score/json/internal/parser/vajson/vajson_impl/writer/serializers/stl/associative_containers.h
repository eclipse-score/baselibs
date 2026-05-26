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
/*!        \file
 *        \brief  A collection of serializers for associative STL containers.
 *
 *      \details  Provides serializers for (unordered) set, (unordered) map, and (unordered) multimap types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "amsr/json/writer/types/array_type.h"
#include "amsr/json/writer/types/object_type.h"

namespace amsr {
namespace json {

/*!
 * \brief           Serializes a set of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Cmp
 *                  Type of comparison function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       set
 *                  Set to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::set<Value, Cmp, Alloc> const& set) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JArray(set);
}

/*!
 * \brief           Serializes an unordered set of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Hash
 *                  Type of hash function.
 * \tparam          Pred
 *                  Type of predicate function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       set
 *                  Unordered set to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                std::unordered_set<Value, Hash, Pred, Alloc> const& set) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JArray(set);
}

/*!
 * \brief           Serializes a map of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Key
 *                  Type of key. Must be convertible to a JKey.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Cmp
 *                  Type of comparison function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       map
 *                  Map to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::map<Key, Value, Cmp, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JObject(map);
}

/*!
 * \brief           Serializes a map of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Key
 *                  Type of key. Must be convertible to a JKey.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Cmp
 *                  Type of comparison function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       map
 *                  Map to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::map<Key, Value, Cmp, Alloc> const& map) noexcept
    -> typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JObject(map);
}

/*!
 * \brief           Serializes an unordered map of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Key
 *                  Type of key. Must be convertible to a JKey.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Hash
 *                  Type of hash function.
 * \tparam          Pred
 *                  Type of predicate function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       map
 *                  Unordered map to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                std::unordered_map<Key, Value, Hash, Pred, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JObject(map);
}

/*!
 * \brief           Serializes a multimap of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Key
 *                  Type of key. Must be convertible to a JKey.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Cmp
 *                  Type of comparison function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       map
 *                  Multimap to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Key, typename Value, typename Cmp, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::multimap<Key, Value, Cmp, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JObject(map);
}

/*!
 * \brief           Serializes an unordered multimap of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Key
 *                  Type of key. Must be convertible to a JKey.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Hash
 *                  Type of hash function.
 * \tparam          Pred
 *                  Type of predicate function.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       map
 *                  Unordered multimap to serialize.
 * \return          The succeeding serializer.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 *
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Next, typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer,
                std::unordered_multimap<Key, Value, Hash, Pred, Alloc> const& map) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JObject(map);
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_ASSOCIATIVE_CONTAINERS_H_
