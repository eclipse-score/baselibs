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
/// \brief A serializer for arrays.
/// \details Provides the serializer for ad-hoc defined tuples.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H

#include <type_traits>
#include <utility>

namespace score::json::vajson
{
inline namespace types
{
/// \brief Serialize an ad-hoc defined Tuple as heterogeneous array
/// \tparam Fn The function type that defines the serialization.
template <typename Fn>
struct JArrayType final
{
    /// \brief Wrapped function value
    Fn fn;
};

/// \brief Serializes an ad-hoc defined Tuple as a heterogeneous array
/// \details The function can be used to define a tuple by adding values.
/// \tparam Fn Type of serializer function. Must take an ArrayStart&& and return the follow-up serializer.
/// \param[in] fn Function used to serialize the tuple.
/// \return A serializable Tuple type.
/// \pre The passed function does not throw any exceptions
template <typename Fn, typename = std::enable_if_t<std::is_rvalue_reference_v<Fn&&>>>
auto JArray(Fn&& fn) noexcept -> JArrayType<Fn>
{  // coverity[autosar_cpp14_a13_3_1_violation]
    return {std::forward<Fn>(fn)};
}

// clang-format off
}  // inline namespace types
// clang-format off
} // namespace score::json::vajson



#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_ARRAY_TYPE_H
