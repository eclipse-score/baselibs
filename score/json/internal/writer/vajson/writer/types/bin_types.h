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
/// \brief Implementation of methods for Bin type.
/// \details Provides serializers for arrays and tuples.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BIN_TYPES_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BIN_TYPES_H

#include <limits>
#include <string>

#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief Safely convert a length
/// \param[in] length to convert.
/// \return The converted length.
inline auto ConvertLength(const std::size_t length) noexcept -> std::uint32_t
{
    AssertCondition(length <= std::numeric_limits<std::uint32_t>::max(), "Length exceeds std::uint32_t range.");

    // coverity[autosar_cpp14_a4_7_1_violation]
    // coverity[autosar_cpp14_m0_3_1_violation]
    return static_cast<std::uint32_t>(length);
}

}  // namespace internal

inline namespace types
{
/// \brief A binary type
class JBinType final
{
  public:
    /// \brief Constructs a binary type
    /// \param[in] b Bytes to serialize.
    explicit JBinType(score::cpp::span<const char> b) noexcept : value_(b) {}

    /// \brief Returns the contained value
    /// \return The value.
    auto GetValue() const noexcept -> score::cpp::span<const char>
    {
        return this->value_;
    }

    /// \brief Returns the length
    /// \return The length.
    auto GetLength() const noexcept -> std::uint32_t
    {
        return internal::ConvertLength(this->value_.size());
    }

  private:
    /// \brief Wrapped binary value
    score::cpp::span<const char> value_;
};

/// \brief Serializes a binary value
/// \param[in] b Bytes to serialize.
/// \return The serializable binary value.
inline auto JBin(score::cpp::span<const char> b) noexcept -> JBinType
{
    return JBinType{b};
}

/// \brief A binary string type
class JBinStringType final
{
  public:
    /// \brief Constructs a binary string
    /// \param[in] s String to serialize.
    constexpr explicit JBinStringType(StringView s) noexcept : value_(s) {}

    /// \brief Returns the contained value
    /// \return The value.
    auto GetValue() const noexcept -> StringView
    {
        return this->value_;
    }

    /// \brief Returns the length
    /// \return The length.
    auto GetLength() const noexcept -> std::uint32_t
    {
        return internal::ConvertLength(this->value_.size());
    }

  private:
    /// \brief Wrapped string value
    StringView value_;
};

/// \brief Serializes a string as binary
/// \param[in] s The string to serialize.
/// \return The serializable string value.
constexpr auto JBinString(StringView s) noexcept -> JBinStringType
{
    return JBinStringType{s};
}

/// \brief Serializes a string as binary
/// \param[in] s The string to serialize.
/// \return The serializable string value.
inline auto JBinString(const std::string& s) noexcept -> JBinStringType
{
    return JBinString(std::string_view{s});
}

/// \brief A binary key type
class JBinKeyType final
{
  public:
    /// \brief Constructs a binary key
    /// \param[in] s Key to serialize.
    constexpr explicit JBinKeyType(StringView s) noexcept : value_(s) {}

    /// \brief Returns the contained value
    /// \return The value.
    auto GetValue() const noexcept -> StringView
    {
        return this->value_;
    }

    /// \brief Returns the length
    /// \return The length.
    auto GetLength() const noexcept -> std::uint32_t
    {
        return internal::ConvertLength(this->value_.size());
    }

  private:
    /// \brief Wrapped key value
    StringView value_;
};

/// \brief Serializes a key as binary
/// \param[in] s The key to serialize.
/// \return The serializable key value.
constexpr auto JBinKey(StringView s) noexcept -> JBinKeyType
{
    return JBinKeyType{s};
}

/// \brief Serializes a key as binary
/// \param[in] s The key to serialize.
/// \return The serializable key value.
inline auto JBinKey(const std::string& s) noexcept -> JBinKeyType
{
    return JBinKey(std::string_view{s});
}
// clang-format off
}  // inline namespace types
// clang-format off
}  // namespace vajson
}  // namespace json
}  // namespace score
#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_TYPES_BIN_TYPES_H
