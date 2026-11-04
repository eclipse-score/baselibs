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
/// \brief Serializer type for JSON strings.

#ifndef SCORE_JSON_INTERNAL_WRITER_VAJSON_J_STRING_TYPE_H
#define SCORE_JSON_INTERNAL_WRITER_VAJSON_J_STRING_TYPE_H

#include <string>
#include <string_view>

namespace score::json::vajson
{
inline namespace types
{
/// \brief A String type
class JStringType final
{
  public:
    /// \brief Constructs a String type
    /// \param[in] s String to serialize.
    constexpr explicit JStringType(std::string_view s) noexcept : value_{s} {}

    /// \brief Returns the contained value
    /// \return The value.
    [[nodiscard]] auto GetValue() const noexcept -> std::string_view
    {
        return this->value_;
    }

  private:
    /// \brief Wrapped string value
    std::string_view value_;
};

/// \brief Serializes a String value
/// \param[in] s String to serialize.
/// \return The serializable string type.
constexpr auto JString(std::string_view s) noexcept -> JStringType
{
    return JStringType(s);
}

/// \brief Serializes a String value
/// \param[in] s String to serialize.
/// \return The serializable string type.
inline auto JString(const std::string& s) noexcept -> JStringType
{
    return JString(std::string_view{s});
}

}  // namespace types
}  // namespace score::json::vajson

#endif  // SCORE_JSON_INTERNAL_WRITER_VAJSON_J_STRING_TYPE_H
