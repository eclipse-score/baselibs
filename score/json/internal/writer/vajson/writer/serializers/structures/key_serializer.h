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
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"

namespace score::json::vajson
{
/// \brief A serializer for JSON keys
/// \details This class only allows adding a key into the object and always returns a value serializer to only allow a
///     value for the next concatenation operation.
class KeySerializer final
{
  public:
    /// \brief Serializer state after adding a key
    using Next = ObjectSerializerValue;

    /// \brief Constructs a KeySerializer from a writer
    /// \details Do not create an instance of KeySerializer directly, use the aliases in
    ///     score/json/internal/writer/vajson/writer/serializers/structures/serializer.h
    /// \param[in] os Writer to write into. It must outlive the serializer.
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
    /// - Add a comma, if another member was serialized before.
    /// - Start a new, indented line if pretty printing is enabled.
    /// - Serialize the key followed by the name separator.
    /// \param[in] key to serialize.
    /// \return The succeeding serializer.
    auto operator<<(JKeyType key) const&& noexcept -> Next
    {
        this->os_.get().BeginElement(this->serializer_state_ == SerializerState::kNonEmpty);

        this->os_.get().Stream().put('"');
        this->os_.get().Stream() << internal::EscapedJsonString(key);
        this->os_.get().Stream().put('"');
        this->os_.get().WriteNameSeparator();

        return Next(this->os_.get());
    }

  private:
    /// \brief Writer to write into
    WriterType os_;

    /// \brief Serializer state
    SerializerState serializer_state_;
};

}  // namespace score::json::vajson

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_KEY_SERIALIZER_H
