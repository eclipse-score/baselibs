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
/// \brief Implementation of methods for generic value serializer type.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H

#include "score/json/internal/writer/vajson/writer/serializers/structures/generic_value_serializer.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/key_serializer.h"

namespace score
{
namespace json
{
namespace vajson
{
/// \brief Serializes another object into the output stream
/// \details
/// - Assert that the current state allows adding an object.
/// - Add an opening curly bracket.
/// - Serialize the object as a JSON object.
/// - Add a closing curly bracket.
template <typename Return>
template <typename Fn>
// coverity[autosar_cpp14_m9_3_3_violation]
auto GenericValueSerializer<Return>::operator<<(JObjectType<Fn> object) && noexcept
    -> GenericValueSerializer<Return>::Next
{
    return this->Serialize([this, &object]() noexcept {
        /// \brief Defines the return value type
        using ReturnType = decltype(object.fn(ObjectStart(this->os_.get())));
        static_assert(std::is_same<ReturnType, KeySerializer>::value, "Cannot close object in current state");

        this->os_.get().put('{');
        static_cast<void>(object.fn(ObjectStart(this->os_.get())));
        this->os_.get().put('}');
    });
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_IMPL_H
