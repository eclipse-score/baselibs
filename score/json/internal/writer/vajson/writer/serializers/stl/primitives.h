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
/// \brief A collection of serializers for primitive data types.
/// \details Provides serializers for pointer, bool, number, and string types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"

namespace score
{
namespace json
{
namespace vajson
{
/// \brief Forward declaration for the GenericValueSerializer
/// \tparam Return type after using operator<<().
template <typename Return>
class GenericValueSerializer;

/// \brief Serializes a null value directly from a nullptr literal
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \return The succeeding serializer.
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, std::nullptr_t) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JNull();
}

/// \brief Serializes a value directly from a pointer
/// \details
/// - If the given pointer is a nullptr:
/// - Serialize it as a null type.
/// - Otherwise:
/// - Serialize the pointer's value.
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \param[in] ptr The pointer whose value to serialize.
/// \return The succeeding serializer.
template <typename Next, typename T>
auto operator<<(GenericValueSerializer<Next>&& serializer, const T* ptr) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return (ptr == nullptr) ? (std::move(serializer) << JNull()) : (std::move(serializer) << *ptr);
}

/// \brief Serializes a bool value directly
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \param[in] b Bool value to serialize.
/// \return The succeeding serializer.
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, bool b) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JBool(b);
}

/// \brief Serializes a number value directly
/// \tparam Next type of serializer.
/// \tparam N Type of number.
/// \param[in] serializer instance to write into.
/// \param[in] number to serialize.
/// \return The succeeding serializer.
template <typename Next, typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value>>
auto operator<<(GenericValueSerializer<Next>&& serializer, N number) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JNumber(number);
}

/// \brief Serializes a string value directly
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \param[in] string to serialize.
/// \return The succeeding serializer.
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, const std::string& string) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JString(string);
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STL_PRIMITIVES_H
