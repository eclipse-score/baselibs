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
/// \brief A collection of serializers for basic JSON types.
/// \details Provides serializers for Null, Bool, Key, Number, and String types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BASIC_TYPES_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BASIC_TYPES_H

#include <string>
#include <string_view>
#include <type_traits>

namespace score::json::vajson
{
inline namespace types
{
/// \brief A Null type
struct JNullType final
{
};

/// \brief Serializes a Null value
/// \return The serializable null type.
constexpr auto JNull() noexcept -> JNullType
{
    return JNullType{};
}

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

/// \brief A Key type
class JKeyType final
{
  public:
    /// \brief Constructs a Key type
    /// \param[in] s Key to serialize.
    explicit constexpr JKeyType(std::string_view s) noexcept : value_(s) {}

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

/// \brief Serializes a Key value
/// \param[in] s Key to serialize.
/// \return The serializable key type.
constexpr auto JKey(std::string_view s) noexcept -> JKeyType
{
    return JKeyType{s};
}

/// \brief A Number type
/// \details bool is excluded: it is a JSON boolean, not a JSON number, and has no std::to_chars overload.
///     Use JBool instead.
/// \tparam N Type of number.
template <typename N, typename = std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>>>
class JNumberType final
{
  public:
    /// \brief Constructs a Number type
    /// \param[in] num Number to serialize.
    constexpr explicit JNumberType(N num) noexcept : value_(num) {}

    /// \brief Returns the contained value
    /// \return The value.
    [[nodiscard]] auto GetValue() const noexcept -> N
    {
        return this->value_;
    }

  private:
    /// \brief Wrapped number value
    N value_;
};

/// \brief Serializes a Number value
/// \details bool is excluded: it is a JSON boolean, not a JSON number. Use JBool instead.
/// \tparam N Type of number.
/// \param[in] n The number to serialize.
/// \return The serializable number type.
template <typename N, typename = std::enable_if_t<std::is_arithmetic_v<N> && !std::is_same_v<N, bool>>>
constexpr auto JNumber(N n) noexcept -> JNumberType<N>
{
    return JNumberType<N>{n};
}

/// \brief A String type
class JStringType final
{
  public:
    /// \brief Constructs a String type
    /// \param[in] s String to serialize.
    constexpr explicit JStringType(std::string_view s) noexcept : value_(s) {}

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

// clang-format off
}  // namespace types
// // clang-format on
} // namespace score::json::vajson


#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BASIC_TYPES_H
