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
/// \brief A collection of serializers for libVac sequence containers.
/// \details Provides serializers for score::cpp::span, score::cpp::static_vector types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H

#include <utility>

#include "score/json/internal/writer/vajson/writer/types/array_type.h"

#include "score/span.hpp"
#include "score/vector.hpp"

#include "score/static_vector.h"

namespace score
{
namespace json
{
namespace vajson
{

/// \brief Serializes an array view of serializable elements
/// \tparam Serializer Type of serializer.
/// \tparam Value Type of value.
/// \param[in] serializer instance to write into.
/// \param[in] view to serialize.
/// \return The succeeding serializer.
template <typename Serializer, typename Value>
auto operator<<(Serializer&& serializer, const ::score::cpp::span<Value>& view) noexcept -> typename Serializer::Next
{
    return std::forward<Serializer>(serializer) << JArray(view);
}

/// \brief Serializes a vector of serializable elements
/// \tparam Serializer Type of serializer.
/// \tparam Value Type of value.
/// \tparam Alloc Vector allocator.
/// \param[in,out] serializer instance to write into.
/// \param[in] vector Data to serialize.
/// \return The succeeding serializer.
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer&& serializer, const ::score::cpp::pmr::vector<Value, Alloc>& vector) noexcept ->
    typename Serializer::Next
{
    return std::forward<Serializer>(serializer) << JArray(vector);
}

/// \brief Serializes a static vector of serializable elements
/// \tparam Serializer Type of serializer.
/// \tparam Value Type of value.
/// \tparam Alloc Type of allocator.
/// \param[in] serializer instance to write into.
/// \param[in] vector to serialize.
/// \return The succeeding serializer.
template <typename Serializer, typename Value, typename Alloc>
auto operator<<(Serializer&& serializer, const ::score::cpp::static_vector<Value, Alloc>& vector) noexcept ->
    typename Serializer::Next
{
    return std::forward<Serializer>(serializer) << JArray(vector);
}

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_VAC_SEQUENCE_CONTAINERS_H
