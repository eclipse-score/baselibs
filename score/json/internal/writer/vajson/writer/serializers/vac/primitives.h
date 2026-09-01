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
/// \brief A collection of serializers for libVac primitive data types.
/// \details Provides serializers for std::string, std::string_view, and std::uint8_t types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H

#include <cstdint>
#include <string>
#include <string_view>
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
template <typename Return>
class GenericValueSerializer;

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

/// \brief Serializes a string value directly
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \param[in] string to serialize.
/// \return The succeeding serializer.
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, ::std::string_view string) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << JString(string);
}

/// \brief Serializes a std::uint8_t value directly
/// \tparam Next type of serializer.
/// \param[in] serializer instance to write into.
/// \param[in] byte to serialize.
/// \return The succeeding serializer.
template <typename Next>
auto operator<<(GenericValueSerializer<Next>&& serializer, ::std::uint8_t byte) noexcept ->
    typename GenericValueSerializer<Next>::Next
{
    return std::move(serializer) << static_cast<std::uint16_t>(byte);
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_PRIMITIVES_H
