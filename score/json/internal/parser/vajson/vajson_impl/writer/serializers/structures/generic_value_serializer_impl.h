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
 *        \brief  Implementation of methods for generic value serializer type.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/structures/generic_value_serializer.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/structures/key_serializer.h"

namespace amsr {
namespace json {
/*!
 * \brief           Serializes another object into the output stream
 * \vprivate        component private
 *
 * \spec
 * requires true;
 * \endspec
 * \internal
 * - Assert that the current state allows adding an object.
 * - Add an opening curly bracket.
 * - Serialize the object as a JSON object.
 * - Add a closing curly bracket.
 * \endinternal
 */
template <typename Return>
template <typename Fn>
// VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
auto GenericValueSerializer<Return>::operator<<(JObjectType<Fn> object) && noexcept
    -> GenericValueSerializer<Return>::Next {
  return this->Serialize([this, &object]() noexcept {
    /*!
     * \brief           Defines the return value type
     */
    using ReturnType = decltype(object.fn(ObjectStart(this->os_.get())));
    static_assert(std::is_same<ReturnType, KeySerializer>::value, "Cannot close object in current state");

    // VCA_VAJSON_OUTPUTSTREAM
    this->os_.get().put('{');
    // VCA_VAJSON_OUTPUTSTREAM
    static_cast<void>(object.fn(ObjectStart(this->os_.get())));
    // VCA_VAJSON_OUTPUTSTREAM
    this->os_.get().put('}');
  });
}

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H_
