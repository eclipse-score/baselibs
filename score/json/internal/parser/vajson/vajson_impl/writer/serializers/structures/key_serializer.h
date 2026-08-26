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
 *        \brief  Serializer for JSON keys.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/structures/serializer.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/util/escaped_json_string.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/serializers/util/length_serializer.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/types/array_type.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/types/basic_types.h"
#include "score/json/internal/parser/vajson/vajson_impl/writer/types/object_type.h"

namespace amsr {
namespace json {
/*!
 * \brief           A serializer for JSON keys
 *
 * \details         This class only allows adding a key into the object and always returns a value serializer to only
 *                  allow a value for the next concatenation operation.
 * \vpublic
 */
class KeySerializer final {
 public:
  /*!
   * \brief           Serializer state after adding a key
   * \vpublic
   */
  using Next = ObjectSerializerValue;

  /*!
   * \brief           Constructs a KeySerializer from an output stream
   * \details         Do not create an instance of KeySerializer directly, use the vpublic aliases in
   *                  amsr/json/writer/serializer.h
   * \vpublic
   *
   * \param[in]       os
   *                  Output stream to write into.
   * \param[in]       state
   *                  of the Serializer.
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
  explicit KeySerializer(WriterType os, SerializerState state = SerializerState::kEmpty) noexcept
      : os_(os), serializer_state_{state} {}

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
  KeySerializer(KeySerializer&&) noexcept = default;

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
  auto operator=(KeySerializer&&) & noexcept -> KeySerializer& = default;

  // Deleted copy constructor copy assignment operator.
  KeySerializer(KeySerializer const&) = delete;
  auto operator=(KeySerializer const&) -> KeySerializer& = delete;

  /*!
   * \brief           Default DTOR
   * \vpublic
   */
  ~KeySerializer() noexcept = default;

  /*!
   * \brief           Serializes a key
   * \vpublic
   *
   * \param[in]       key
   *                  to serialize.
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
   * - Add a comma, if necessary.
   * - Serialize the key.
   * \endinternal
   */
  auto operator<<(JKeyType key) const&& noexcept -> Next {
    this->WriteComma();

    // VCA_VAJSON_OUTPUTSTREAM
    this->os_.get().put('"');
    // VCA_VAJSON_OUTPUTSTREAM
    this->os_.get() << internal::EscapedJsonString(key);
    // VCA_VAJSON_OUTPUTSTREAM
    constexpr auto colon_str = R"(":)"sv;
    this->os_.get().write(colon_str.data(), colon_str.size());

    return Next(this->os_.get());
  }

  /*!
   * \brief           Serializes a binary key
   * \vpublic
   *
   * \param[in]       key
   *                  to serialize.
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
   * - Add a comma, if necessary.
   * - Serialize the length of the key as four bytes big endian.
   * - Write the key.
   * \endinternal
   */
  auto operator<<(JBinKeyType key) const&& noexcept -> Next {
    this->WriteComma();

    // VCA_VAJSON_OUTPUTSTREAM
    this->os_.get().put('k');
    // VCA_VAJSON_OUTPUTSTREAM
    internal::SerializeLength(this->os_.get(), key.GetLength());
    // VCA_VAJSON_OUTPUTSTREAM
    const auto value = key.GetValue();
    this->os_.get().write(value.data(), value.size());

    return Next(this->os_.get());
  }

 private:
  /*!
   * \brief           Adds a comma to the stream, if necessary
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
   * - If another element was serialized before, add a comma.
   * \endinternal
   */
  void WriteComma() const noexcept {
    if (this->serializer_state_ == SerializerState::kNonEmpty) {
      // VCA_VAJSON_OUTPUTSTREAM
      this->os_.get().put(',');
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

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H_
