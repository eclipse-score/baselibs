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
 *        \brief  A collection of serializers for STD sequence containers.
 *
 *      \details  Provides serializers for std::array, std::vector, and std::deque types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <deque>
#include <type_traits>
#include <utility>
#include <vector>
#include "score/json/internal/writer/vajson/writer/types/array_type.h"

namespace amsr {
namespace json {

/*!
 * \brief           Serializes an array of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          N
 *                  Size of the array.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       array
 *                  to serialize.
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
template <typename Next, typename Value, std::size_t N>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::array<Value, N> const& array) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JArray(array);
}

/*!
 * \brief           Serializes a vector of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       vector
 *                  to serialize.
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
template <typename Next, typename Value, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::vector<Value, Alloc> const& vector) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JArray(vector);
}

/*!
 * \brief           Serializes a deque of serializable elements
 * \vpublic
 *
 * \tparam          Next
 *                  type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Alloc
 *                  Type of allocator.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       deque
 *                  to serialize.
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
template <typename Next, typename Value, typename Alloc>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::deque<Value, Alloc> const& deque) noexcept ->
    typename GenericValueSerializer<Next>::Next {
  return std::move(serializer) << JArray(deque);
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STL_SEQUENCE_CONTAINERS_H_
