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
/// \brief Serializer type for JSON booleans.

#ifndef SCORE_JSON_INTERNAL_WRITER_VAJSON_J_BOOL_TYPE_H
#define SCORE_JSON_INTERNAL_WRITER_VAJSON_J_BOOL_TYPE_H

namespace score::json::vajson
{
inline namespace types
{
/// \brief A Bool type
struct JBoolType final
{
    /// \brief Wrapped bool value
    bool value;
};

/// \brief Serializes a Bool value
/// \param[in] b Bool value to serialize.
/// \return The serializable bool type.
constexpr auto JBool(bool b) noexcept -> JBoolType
{
    return {b};
}

}  // namespace types
}  // namespace score::json::vajson

#endif  // SCORE_JSON_INTERNAL_WRITER_VAJSON_J_BOOL_TYPE_H
