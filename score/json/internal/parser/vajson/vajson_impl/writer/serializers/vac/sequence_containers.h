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
 *        \brief  A collection of serializers for libVac sequence containers.
 *
 *      \details  Provides serializers for score::cpp::span, score::cpp::static_vector
 *                types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <utility>

#include "amsr/json/writer/types/array_type.h"

#include "score/span.hpp"
#include "score/vector.hpp"

#include "score/static_vector.h"

namespace amsr {
namespace json {

/*!
 * \brief           Serializes an array view of serializable elements
 * \vpublic
 *
 * \tparam          Serializer
 *                  Type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \param[in]       serializer
 *                  instance to write into.
 * \param[in]       view
 *                  to serialize.
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
template <typename Serializer, typename Value>
auto operator<<(Serializer&& serializer, ::score::cpp::span<Value> const& view) noexcept -> typename Serializer::Next {
  return std::forward<Serializer>(serializer) << JArray(view);
}

/*!
 * \brief           Serializes a vector of serializable elements
 * \vpublic
 *
 * \tparam          Serializer
 *                  Type of serializer.
 * \tparam          Value
 *                  Type of value.
 * \tparam          Alloc
 *                  Vector allocator.
 * \param[in,out]   serializer
 *                  instance to write into.
 * \param[in]       vector
 *                  Data to serialize.
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
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer&& serializer, ::score::cpp::pmr::vector<Value, Alloc> const& vector) noexcept ->
    typename Serializer::Next {
  return std::forward<Serializer>(serializer) << JArray(vector);
}

/*!
 * \brief           Serializes a static vector of serializable elements
 * \vpublic
 *
 * \tparam          Serializer
 *                  Type of serializer.
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
 * \synchronous     -
 * \trace           DSGN-JSON-Writer-Serializable-Data-Structures
 * \spec
 * requires true;
 * \endspec
 */
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer&& serializer, ::score::cpp::static_vector<Value, Alloc> const& vector) noexcept ->
    typename Serializer::Next {
  return std::forward<Serializer>(serializer) << JArray(vector);
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H_
