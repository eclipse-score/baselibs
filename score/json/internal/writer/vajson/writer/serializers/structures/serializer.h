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
 *        \brief  Contains common types and forward declarations for JSON serializers.
 *
 *********************************************************************************************************************/
#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <functional>
#include <ostream>

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace amsr {
namespace json {
/*!
 * \brief           State of the object to be serialized
 *
 * \details         Indicates if the object is empty or not. Commas should only appended if the object is not empty.
 * \vpublic
 */
enum class SerializerState : bool { kEmpty, kNonEmpty };

/*!
 * \brief           An empty type that signifies that the serializer has no follow-up state
 *
 * \vprivate        component private
 *
 * \trace           DSGN-JSON-Writer-Error-Prevention
 */
class Unit {
 public:
  /*!
   * \brief           Constructs a Unit type from an output stream
   *
   * \details         This satisfies the 'Next' state interface for serializers.
   * \vprivate        Vector component internal API
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  explicit Unit(std::reference_wrapper<std::ostream>,
                SerializerState = SerializerState::kEmpty) noexcept {}
};

/*!
 * \brief           A marker struct that only tells the GenericValueSerializer to return itself after using operator<<()
 *
 * \vprivate        component private
 *
 * \trace           DSGN-JSON-Writer-Error-Prevention
 */
class Self {};

/*!
 * \brief           Forward declaration for the GenericValueSerializer
 *
 * \vprivate        component private
 */
template <typename Return = Self>
class GenericValueSerializer;

/*!
 * \brief           A serializer type for single values
 *
 * \vprivate        component private
 */
using ValueSerializer = GenericValueSerializer<Unit>;

/*!
 * \brief           A serializer type for JSON documents
 *
 * \details         Intentionally a using to make it obvious that a JSON document must start with a single value.
 * \vpublic
 */
using DocumentSerializer = ValueSerializer;

/**********************************************************************************************************************
 *  Object Declarations *
 *********************************************************************************************************************/

/*!
 * \brief           Forward declaration for the KeySerializer
 */
class KeySerializer;

/*!
 * \brief           A serializer type for the start of JSON objects
 *
 * \details         Typedef for the initial Object serializer state where only a key is allowed.
 * \vpublic
 */
using ObjectStart = KeySerializer;

/*!
 * \brief           A serializer type for JSON objects
 *
 * \details         This class only allows adding a value into the object and the next concatenation will only allow a
 *                  key.
 * \vprivate        component private
 */
using ObjectSerializerValue = GenericValueSerializer<KeySerializer>;

/**********************************************************************************************************************
 *  Tuple Declarations *
 *********************************************************************************************************************/

/*!
 * \brief           A serializer type for JSON arrays
 *
 * \details         Serializes multiple, potentially inhomogeneous values.
 * \vprivate        component private
 */
using ArraySerializer = GenericValueSerializer<>;

/*!
 * \brief           A serializer type for the start of JSON arrays
 *
 * \details         Typedef for the initial Array serializer state.
 * \vpublic
 */
using ArrayStart = ArraySerializer;

/*!
 * \brief           Type of the output writer
 *
 * \vpublic
 */
using WriterType = std::reference_wrapper<std::ostream>;
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_SERIALIZER_H_
