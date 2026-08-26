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
/*!        \file
 *        \brief  Serializer for JSON string literals.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/structures/serializer.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/types/basic_types.h"

namespace amsr {
namespace json {
namespace internal {
/*!
 * \brief           An escaped JSON string type
 * \vprivate        component private
 */
class EscapedJsonString {
 public:
  /*!
   * \brief           Constructs an EscapedJsonString from a JSON key
   * \vprivate        Vector component internal API
   *
   * \param[in]       key
   *                  to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  explicit EscapedJsonString(JKeyType key) noexcept : EscapedJsonString(key.GetValue()) {}

  /*!
   * \brief           Constructs an EscapedJsonString from a JSON string
   * \vprivate        Vector component internal API
   *
   * \param[in]       string
   *                  to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  explicit EscapedJsonString(JStringType string) noexcept : EscapedJsonString(string.GetValue()) {}

  /*!
   * \brief           Returns the contained string
   * \vprivate        Vector component internal API
   *
   * \return          The contained string.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetValue() const noexcept -> std::string_view { return this->value_; }

 private:
  /*!
   * \brief           Constructs an EscapedJsonString from a StringView
   * \param[in]       value
   *                  The value to serialize.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   *
   * \spec
   * requires true;
   * \endspec
   */
  explicit EscapedJsonString(std::string_view value) noexcept : value_{value} {}

  /*!
   * \brief           Value to write as a JSON string literal
   */
  std::string_view value_;
};

// NOLINTNEXTLINE(whitespace/line_length)
// VECTOR NCL AutosarC++17_10-M5.0.16, Metric-HIS.VG: MD_JSON_AutosarC++17_10-M5.0.16_pointer_arithmetic, MD_JSON_Metric-HIS.VG_json_value
/*!
 * \brief           Serializes an escaped string literal type
 * \vprivate        component private
 *
 * \param[in]       os
 *                  Output stream to write into.
 * \param[in]       string
 *                  to escape and serialize.
 * \return          A reference to the output stream.
 *
 * \context         ANY
 * \pre             -
 * \threadsafe      FALSE
 * \reentrant       FALSE
 * \spec
 * requires true;
 * \endspec
 *
 * \internal
 * - If the string contains a character that needs to be escaped in JSON:
 *   - Serialize the escaped character.
 * - Otherwise:
 *   - Serialize the character directly.
 * \endinternal
 */
auto inline operator<<(std::ostream& os, EscapedJsonString string) noexcept -> std::ostream& {
  for (char const ch : string.GetValue()) {
    switch (std::char_traits<char>::to_int_type(ch)) {
      case std::char_traits<char>::to_int_type('"'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\\"", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\\'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\\\", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\b'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\b", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\f'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\f", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\n'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\n", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\r'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\r", 2);
        break;
      }
      case std::char_traits<char>::to_int_type('\t'): {
        // VCA_VAJSON_OUTPUTSTREAM
        os.write("\\t", 2);
        break;
      }
      default:
        // VCA_VAJSON_OUTPUTSTREAM
        os.put(ch);
        break;
    }
  }

  return os;
}

}  // namespace internal
}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_UTIL_ESCAPED_JSON_STRING_H_
