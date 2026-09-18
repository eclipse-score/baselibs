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
/// \brief A collection of serializers for objects.
/// \details Provides serializers for Object types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_H

#include <type_traits>
#include <utility>

namespace score::json::vajson
{
inline namespace types
{
/// \brief An Object type
/// \tparam Fn Type of serializer function.
template <typename Fn>
struct JObjectType final
{
    /// \brief Function used to serialize the object
    Fn fn;
};

/// \brief Serializes an object value
/// \tparam Fn Type of serializer function. Must take an ObjectStart&& and return the follow-up serializer.
/// \param[in] fn Function used to serialize the object.
/// \return The serializable object type.
/// \pre The passed function does not throw any exceptions
template <typename Fn, typename = std::enable_if_t<std::is_rvalue_reference<Fn&&>::value>>
auto JObject(Fn&& fn) noexcept -> JObjectType<Fn>
{  // coverity[autosar_cpp14_a13_3_1_violation]
    return {std::forward<Fn>(fn)};
}

// clang-format off
}  // inline namespace types
// clang-format on
}  // namespace score::json::vajson

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_OBJECT_TYPE_H
