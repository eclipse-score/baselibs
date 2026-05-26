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
 *        \brief  Serializer for generic JSON value types.
 *
 *      \details  Provides serializers for Null, Bool, Number, String, Array, and Object types.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <array>
#include <charconv>
#include <system_error>

#include "score/language/futurecpp/charconv.hpp"
#include "amsr/json/util/json_error_domain.h"
#include "amsr/json/util/types.h"
#include "amsr/json/writer/serializers/structures/serializer.h"
#include "amsr/json/writer/serializers/util/escaped_json_string.h"
#include "amsr/json/writer/serializers/util/length_serializer.h"
#include "amsr/json/writer/types/array_type.h"
#include "amsr/json/writer/types/basic_types.h"
#include "amsr/json/writer/types/bin_types.h"
#include "amsr/json/writer/types/object_type.h"

namespace amsr {
namespace json {
/*!
 * \brief           A serializer for JSON value types
 * \vpublic
 *
 * \tparam          Return
 *                  Type of the return value of a << operation. Must be one of the following types:
 *                  - Unit: Serializer has no follow-up state (outermost element).
 *                  - Self or GenericValueSerializer<T>: Next element is another value (e.g. inside arrays).
 *                  - KeySerializer: Next element is a key.
 *
 * \trace           DSGN-JSON-Writer-Serialization
 */
template <typename Return>
class GenericValueSerializer final {
 public:
  /*!
   * \brief           Type of the return value
   *
   * \details         Set the type of the return value to be either its own type GenericValueSerializer (for arrays or a
   *                  specified type) or the type specified by Return.
   * \vpublic
   */
  using Next = typename std::conditional_t<std::is_same<Return, Self>::value, GenericValueSerializer, Return>;

  /*!
   * \brief           Constructs a GenericValueSerializer from an output stream
   * \details         Do not create an instance of GenericValueSerializer directly, use the vpublic aliases in
   *                  amsr/json/writer/serializer.h
   * \vpublic
   *
   * \param[in]       os
   *                  Output stream to write into.
   * \param[in]       state
   *                  of the Serializer.
   * \param[in]       bom
   *                  The BOM type to write.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  explicit GenericValueSerializer(WriterType os, SerializerState state = SerializerState::kEmpty,
                                  EncodingType bom = EncodingType::kNone) noexcept
      : os_(os), serializer_state_{state} {
    this->WriteBom(bom);
  }

  /*!
   * \brief           Default move constructor
   *
   * \vpublic
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  GenericValueSerializer(GenericValueSerializer&&) noexcept = default;

  /*!
   * \brief           Default move assignment
   * \vpublic
   *
   * \return          A reference to the moved into object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  auto operator=(GenericValueSerializer&&) & noexcept -> GenericValueSerializer& = default;

  // Deleted copy constructor/assignment operator.
  GenericValueSerializer(GenericValueSerializer const&) = delete;
  auto operator=(GenericValueSerializer const&) -> GenericValueSerializer& = delete;

  /*!
   * \brief           Default DTOR
   * \vpublic
   */
  ~GenericValueSerializer() noexcept = default;

  /*!
   * \brief           Serializes a null value
   * \vpublic
   *
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JNullType) && noexcept -> Next {
    return this->Serialize([this]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      constexpr auto null_str = "null"sv;
      this->os_.get().write(null_str.data(), null_str.size());
    });
  }

  /*!
   * \brief           Serializes a boolean value
   * \vpublic
   *
   * \param[in]       b
   *                  Boolean value to serialize.
   * \return          The succeeding serializer.
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JBoolType b) && noexcept -> Next {
    score::safecpp::zstring_view const value{b.value ? "true" : "false"};
    // NOLINTNEXTLINE(whitespace/line_length)
    // VECTOR NL AutosarC++17_10-M8.5.1: MD_JSON_AutosarC++17_10-M8.5.1_use_of_possibly_unintialized_value_false_positive
    return this->Serialize([this, value]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().write(value.data(), value.size());
    });
  }

  /*!
   * \brief           Serializes a number value
   * \vpublic
   *
   * \tparam          T
   *                  Type of number.
   * \param[in]       number
   *                  value to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  template <typename T>
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JNumberType<T> number) && noexcept -> Next {
    return this->Serialize([this, number]() noexcept {
      // Buffer size: max 24 chars for double, ~20 for int64, extra space for safety
      std::array<char, 64> buffer{};
      T value = static_cast<T>(number.GetValue());
      
      // Convert number to string using std::to_chars
      score::cpp::to_chars_result conversion_result{score::cpp::to_chars(buffer.data(), buffer.data() + buffer.size(), value)};
      
      AssertCondition(ec == std::errc{},
                      "GenericValueSerializer: Could not convert number to textual representation.");

      // Create a span from the buffer to the end of written data
      score::cpp::span<char const> result_span{buffer.data(), static_cast<std::size_t>(ptr - buffer.data())};
      
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().write(result_span.data(), result_span.size());
    });
  }

  /*!
   * \brief           Serializes a string value
   * \vpublic
   *
   * \param[in]       string
   *                  value to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   *
   * \internal
   * - Add quotes to the begin and end of the string.
   * - Serialize the escaped string as a JSON string value.
   * \endinternal
   */
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JStringType string) && noexcept -> Next {
    return this->Serialize([this, string]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().Put('"');
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get() << internal::EscapedJsonString(string);
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().Put('"');
    });
  }

  /*!
   * \brief           Serializes a binary string value
   * \vpublic
   *
   * \param[in]       string
   *                  value to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   *
   * \internal
   * - Add a 's' to denote the following value as a string.
   * - Serialize the length of the string value as four bytes big endian.
   * - Write the string value.
   * \endinternal
   */
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JBinStringType string) && noexcept -> Next {
    return this->Serialize([this, string]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().put('s');
      // VCA_VAJSON_OUTPUTSTREAM
      internal::SerializeLength(this->os_.get(), string.GetLength());
      // VCA_VAJSON_OUTPUTSTREAM
      auto const& value = string.GetValue();
      this->os_.get().write(value.data(), value.size());
    });
  }

  /*!
   * \brief           Serializes a series of serializable values
   * \vpublic
   *
   * \tparam          Fn
   *                  Type of serializer function.
   * \param[in]       tuple
   *                  to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             The function contained in the argument does not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   *    requires true;
   * \endspec
   * \internal
   * - Add an opening square bracket.
   * - Serialize every value as a JSON value.
   * - Add a closing square bracket.
   * \endinternal
   */
  template <typename Fn>
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JArrayType<Fn> tuple) && noexcept -> Next {
    return this->Serialize([this, tuple]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().Put('[');
      static_cast<void>(tuple.fn(ArrayStart(this->os_.get())));
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().Put(']');
    });
  }

  /*!
   * \brief           Serializes a binary value
   * \vpublic
   *
   * \param[in]       bin
   *                  Value to serialize.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   *
   * \internal
   * - Add a 'b' to denote the following value as binary.
   * - Serialize the length of the binary value as four bytes big endian.
   * - Write the binary value.
   * \endinternal
   */
  // VECTOR NCL AutosarC++17_10-M9.3.3: MD_JSON_AutosarC++17_10-M9.3.3_logical_const
  auto operator<<(JBinType bin) && noexcept -> Next {
    return this->Serialize([this, bin]() noexcept {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().put('b');
      // VCA_VAJSON_OUTPUTSTREAM
      internal::SerializeLength(this->os_.get(), bin.GetLength());
      // VCA_VAJSON_OUTPUTSTREAM
      auto const& value = bin.GetValue();
      this->os_.get().write(value.data(), value.size());
    });
  }

  /*!
   * \brief           Serializes an object
   * \vpublic
   *
   * \tparam          Fn
   *                  Type of serializer function.
   * \param[in]       object
   *                  to serialize.
   * \return          The succeeding serializer.
   * \context         ANY
   * \pre             The function contained in the argument does not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   *    requires true;
   * \endspec
   */
  template <typename Fn>
  auto operator<<(JObjectType<Fn> object) && noexcept -> Next;

 private:
  /*!
   * \brief           Serializes a value
   * \tparam          Fn
   *                  Type of function.
   * \param[in]       fn
   *                  Serializer call function.
   * \return          The succeeding serializer.
   *
   * \context         ANY
   * \pre             The passed function does not throw any exceptions
   * \threadsafe      FALSE
   * \reentrant       FALSE
   *
   * \internal
   * - If another element was serialized before:
   *   - Add a comma.
   * - Execute the given serializer function.
   * \endinternal
   */
  template <typename Fn>
  auto Serialize(Fn&& fn) const noexcept -> Next {
    if (this->serializer_state_ == SerializerState::kNonEmpty) {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().Put(',');
    }
    std::forward<Fn>(fn)();
    return Next(this->os_.get(), SerializerState::kNonEmpty);
  }

  /*!
   * \brief           Writes the requested BOM type
   * \param[in]       type
   *                  The BOM type to write.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   *
   * \spec
   * requires true;
   * \endspec
   *
   * \internal
   * - Write the requested BOM to the stream.
   * \endinternal
   */
  void WriteBom(EncodingType type) const noexcept {
    if (type == EncodingType::kUtf8) {
      constexpr std::string_view kUtf8Bom{"\xEF\xBB\xBF"sv};
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().write(kUtf8Bom.data(), kUtf8Bom.size());
    }
  }

  /*!
   * \brief           Output stream to write into
   */
  WriterType os_;

  /*!
   * \brief           Serializer state
   */
  SerializerState serializer_state_;
};

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H_
