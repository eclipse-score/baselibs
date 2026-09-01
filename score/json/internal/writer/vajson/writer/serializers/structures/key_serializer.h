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
/// \brief Serializer for JSON keys.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H

#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/serializers/util/escaped_json_string.h"
#include "score/json/internal/writer/vajson/writer/serializers/util/length_serializer.h"
#include "score/json/internal/writer/vajson/writer/types/array_type.h"
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"
#include "score/json/internal/writer/vajson/writer/types/object_type.h"

namespace score
{
namespace json
{
namespace vajson
{
/// \brief A serializer for JSON keys
/// \details This class only allows adding a key into the object and always returns a value serializer to only allow a
///     value for the next concatenation operation.
class KeySerializer final
{
  public:
    /// \brief Serializer state after adding a key
    using Next = ObjectSerializerValue;

    /// \brief Constructs a KeySerializer from an output stream
    /// \details Do not create an instance of KeySerializer directly, use the aliases in
    ///     score/json/internal/writer/vajson/writer/serializers/structures/serializer.h
    /// \param[in] os Output stream to write into.
    /// \param[in] state of the Serializer.
    explicit KeySerializer(WriterType os, SerializerState state = SerializerState::kEmpty) noexcept
        : os_(os), serializer_state_{state}
    {
    }

    /// \brief Default move constructor
    KeySerializer(KeySerializer&&) noexcept = default;

    /// \brief Default move assignment
    /// \return A reference to the moved into object.
    auto operator=(KeySerializer&&) & noexcept -> KeySerializer& = default;

    // Deleted copy constructor copy assignment operator.
    KeySerializer(const KeySerializer&) = delete;
    auto operator=(const KeySerializer&) -> KeySerializer& = delete;

    /// \brief Default DTOR
    ~KeySerializer() noexcept = default;

    /// \brief Serializes a key
    /// \details
    /// - Add a comma, if necessary.
    /// - Serialize the key.
    /// \param[in] key to serialize.
    /// \return The succeeding serializer.
    auto operator<<(JKeyType key) const&& noexcept -> Next
    {
        this->WriteComma();

        this->os_.get().put('"');
        this->os_.get() << internal::EscapedJsonString(key);
        constexpr auto colon_str = R"(":)"sv;
        this->os_.get().write(colon_str.data(), colon_str.size());

        return Next(this->os_.get());
    }

    /// \brief Serializes a binary key
    /// \details
    /// - Add a comma, if necessary.
    /// - Serialize the length of the key as four bytes big endian.
    /// - Write the key.
    /// \param[in] key to serialize.
    /// \return The succeeding serializer.
    auto operator<<(JBinKeyType key) const&& noexcept -> Next
    {
        this->WriteComma();

        this->os_.get().put('k');
        internal::SerializeLength(this->os_.get(), key.GetLength());
        const auto value = key.GetValue();
        this->os_.get().write(value.data(), value.size());

        return Next(this->os_.get());
    }

  private:
    /// \brief Adds a comma to the stream, if necessary
    /// \details
    /// - If another element was serialized before, add a comma.
    void WriteComma() const noexcept
    {
        if (this->serializer_state_ == SerializerState::kNonEmpty)
        {
            this->os_.get().put(',');
        }
    }

    /// \brief Output stream to write into
    WriterType os_;

    /// \brief Serializer state
    SerializerState serializer_state_;
};

}  // namespace vajson
}  // namespace json
}  // namespace score

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H
