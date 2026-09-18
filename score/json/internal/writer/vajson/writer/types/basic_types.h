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
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

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
constexpr inline auto JNull() noexcept -> JNullType
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
constexpr inline auto JBool(bool b) noexcept -> JBoolType
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

/// \brief Serializes a Key value
/// \param[in] s Key to serialize.
/// \return The serializable key type.
inline auto JKey(const std::string& s) noexcept -> JKeyType
{
    return JKey(std::string_view(s));
}

inline namespace literals
{
/// \brief Serializes a Key value
/// \param[in] s String literal to serialize.
/// \param[in] size Size of the pointer.
/// \return The serializable key type.
// coverity[autosar_cpp14_a13_1_3_violation]
constexpr auto operator""_key(const char* s, std::size_t size) noexcept -> JKeyType
{
    return JKey(score::safecpp::zstring_view{s, size});
}

// clang-format off
}  // namespace literals
// clang-format on

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

/// \brief A function object used to serialize predefined serializers
/// \tparam Container Type of container to serialize.
template <typename Container>
class IdSerializer
{
  public:
    /// \brief Value Type of container
    using value_type = typename Container::value_type;

    /// \brief Returns the unchanged value
    /// \param[in] v Value to return.
    /// \return The unchanged value.
    template <typename Value = value_type>
    auto operator()(Value&& v) const noexcept -> Value
    {
        return std::forward<Value>(v);
    }
};

// clang-format off
}  // namespace types
// // clang-format on
} // namespace score::json::vajson


#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BASIC_TYPES_H
