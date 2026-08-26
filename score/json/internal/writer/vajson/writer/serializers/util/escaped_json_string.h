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
/// \brief Serializer for JSON string literals.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H

#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"

namespace score
{
namespace json
{
namespace vajson
{
namespace internal
{
/// \brief An escaped JSON string type
class EscapedJsonString
{
  public:
    /// \brief Constructs an EscapedJsonString from a JSON key
    /// \param[in] key to serialize.
    explicit EscapedJsonString(JKeyType key) noexcept : EscapedJsonString(key.GetValue()) {}

    /// \brief Constructs an EscapedJsonString from a JSON string
    /// \param[in] string to serialize.
    explicit EscapedJsonString(JStringType string) noexcept : EscapedJsonString(string.GetValue()) {}

    /// \brief Returns the contained string
    /// \return The contained string.
    auto GetValue() const noexcept -> std::string_view
    {
        return this->value_;
    }

  private:
    /// \brief Constructs an EscapedJsonString from a StringView
    /// \param[in] value The value to serialize.
    explicit EscapedJsonString(std::string_view value) noexcept : value_{value} {}

    /// \brief Value to write as a JSON string literal
    std::string_view value_;
};

// NOLINTNEXTLINE(whitespace/line_length)
// coverity[autosar_cpp14_m5_0_16_violation]
/// \brief Serializes an escaped string literal type
/// \details
/// - If the string contains a character that needs to be escaped in JSON:
/// - Serialize the escaped character.
/// - Otherwise:
/// - Serialize the character directly.
/// \param[in] os Output stream to write into.
/// \param[in] string to escape and serialize.
/// \return A reference to the output stream.
auto inline operator<<(std::ostream& os, EscapedJsonString string) noexcept -> std::ostream&
{
    for (const char ch : string.GetValue())
    {
        switch (std::char_traits<char>::to_int_type(ch))
        {
            case std::char_traits<char>::to_int_type('"'):
            {
                os.write("\\\"", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\\'):
            {
                os.write("\\\\", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\b'):
            {
                os.write("\\b", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\f'):
            {
                os.write("\\f", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\n'):
            {
                os.write("\\n", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\r'):
            {
                os.write("\\r", 2);
                break;
            }
            case std::char_traits<char>::to_int_type('\t'):
            {
                os.write("\\t", 2);
                break;
            }
            default:
                os.put(ch);
                break;
        }
    }

    return os;
}

}  // namespace internal
}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H
