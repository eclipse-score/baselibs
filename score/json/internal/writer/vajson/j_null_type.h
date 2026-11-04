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
/// @file
/// @brief Serializer type for JSON null.

#ifndef SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NULL_TYPE_H
#define SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NULL_TYPE_H

namespace score::json::vajson
{
inline namespace types
{
/// @brief A Null type
struct JNullType final
{
};

/// @brief Serializes a Null value
/// @return The serializable null type.
constexpr auto JNull() noexcept -> JNullType
{
    return JNullType{};
}

}  // namespace types
}  // namespace score::json::vajson

#endif  // SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NULL_TYPE_H
