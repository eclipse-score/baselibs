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
/// @brief Serializer type for JSON numbers.

#ifndef SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NUMBER_TYPE_H
#define SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NUMBER_TYPE_H

#include <type_traits>

namespace score::json::vajson
{
inline namespace types
{
/// @brief A Number type
/// @details bool is excluded: it is a JSON boolean, not a JSON number, and has no std::to_chars overload.
///     Use JBool instead.
/// @tparam N Type of number.
template <typename N, typename = std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>>>
class JNumberType final
{
  public:
    /// @brief Constructs a Number type
    /// @param[in] num Number to serialize.
    constexpr explicit JNumberType(N num) noexcept : value_{num} {}

    /// @brief Returns the contained value
    /// @return The value.
    [[nodiscard]] auto GetValue() const noexcept -> N
    {
        return this->value_;
    }

  private:
    /// @brief Wrapped number value
    N value_;
};

/// @brief Serializes a Number value
/// @details bool is excluded: it is a JSON boolean, not a JSON number. Use JBool instead.
/// @tparam N Type of number.
/// @param[in] n The number to serialize.
/// @return The serializable number type.
template <typename N, typename = std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>>>
constexpr auto JNumber(N n) noexcept -> JNumberType<N>
{
    return JNumberType<N>{n};
}

}  // namespace types
}  // namespace score::json::vajson

#endif  // SCORE_JSON_INTERNAL_WRITER_VAJSON_J_NUMBER_TYPE_H
