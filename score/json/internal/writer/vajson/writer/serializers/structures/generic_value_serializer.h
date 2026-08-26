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
/// \brief Serializer for generic JSON value types.
/// \details Provides serializers for Null, Bool, Number, String, Array, and Object types.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H

#include <charconv>
#include <array>
#include <system_error>

#include "score/json/internal/parser/vajson/vajson_impl/util/json_error_domain.h"
#include "score/json/internal/parser/vajson/vajson_impl/util/types.h"
#include "score/json/internal/writer/vajson/writer/serializers/structures/serializer.h"
#include "score/json/internal/writer/vajson/writer/serializers/util/escaped_json_string.h"
#include "score/json/internal/writer/vajson/writer/serializers/util/length_serializer.h"
#include "score/json/internal/writer/vajson/writer/types/array_type.h"
#include "score/json/internal/writer/vajson/writer/types/basic_types.h"
#include "score/json/internal/writer/vajson/writer/types/bin_types.h"
#include "score/json/internal/writer/vajson/writer/types/object_type.h"

namespace score
{
namespace json
{
namespace vajson
{
/// \brief A serializer for JSON value types
/// \tparam Return Type of the return value of a << operation. Must be one of the following types: - Unit: Serializer
///     has no follow-up state (outermost element). - Self or GenericValueSerializer<T>: Next element is another value
///     (e.g. inside arrays). - KeySerializer: Next element is a key.
template <typename Return>
class GenericValueSerializer final
{
  public:
    /// \brief Type of the return value
    /// \details Set the type of the return value to be either its own type GenericValueSerializer (for arrays or a
    ///     specified type) or the type specified by Return.
    using Next = typename std::conditional_t<std::is_same<Return, Self>::value, GenericValueSerializer, Return>;

    /// \brief Constructs a GenericValueSerializer from an output stream
    /// \details Do not create an instance of GenericValueSerializer directly, use the aliases in
    ///     score/json/internal/writer/vajson/writer/serializers/structures/serializer.h
    /// \param[in] os Output stream to write into.
    /// \param[in] state of the Serializer.
    /// \param[in] bom The BOM type to write.
    explicit GenericValueSerializer(WriterType os,
                                    SerializerState state = SerializerState::kEmpty,
                                    EncodingType bom = EncodingType::kNone) noexcept
        : os_(os), serializer_state_{state}
    {
        this->WriteBom(bom);
    }

    /// \brief Default move constructor
    GenericValueSerializer(GenericValueSerializer&&) noexcept = default;

    /// \brief Default move assignment
    /// \return A reference to the moved into object.
    auto operator=(GenericValueSerializer&&) & noexcept -> GenericValueSerializer& = default;

    // Deleted copy constructor/assignment operator.
    GenericValueSerializer(const GenericValueSerializer&) = delete;
    auto operator=(const GenericValueSerializer&) -> GenericValueSerializer& = delete;

    /// \brief Default DTOR
    ~GenericValueSerializer() noexcept = default;

    /// \brief Serializes a null value
    /// \return The succeeding serializer.
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JNullType) && noexcept -> Next
    {
        return this->Serialize([this]() noexcept {
            constexpr auto null_str = "null"sv;
            this->os_.get().write(null_str.data(), null_str.size());
        });
    }

    /// \brief Serializes a boolean value
    /// \param[in] b Boolean value to serialize.
    /// \return The succeeding serializer.
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JBoolType b) && noexcept -> Next
    {
        const std::string_view value{b.value ? "true" : "false"};
        // NOLINTNEXTLINE(whitespace/line_length)
        // coverity[autosar_cpp14_m8_5_1_violation]
        return this->Serialize([this, value]() noexcept {
            this->os_.get().write(value.data(), static_cast<std::streamsize>(value.size()));
        });
    }

    /// \brief Serializes a number value
    /// \tparam T Type of number.
    /// \param[in] number value to serialize.
    /// \return The succeeding serializer.
    template <typename T>
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JNumberType<T> number) && noexcept -> Next
    {
        return this->Serialize([this, number]() noexcept {
            // Buffer size: max 24 chars for double, ~20 for int64, extra space for safety
            std::array<char, 64> buffer{};
            T value = static_cast<T>(number.GetValue());

            const auto conversion_result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

            AssertCondition(conversion_result.ec == std::errc{},
                            "GenericValueSerializer: Could not convert number to textual representation.");

            this->os_.get().write(buffer.data(), static_cast<std::streamsize>(conversion_result.ptr - buffer.data()));
        });
    }

    /// \brief Serializes a string value
    /// \details
    /// - Add quotes to the begin and end of the string.
    /// - Serialize the escaped string as a JSON string value.
    /// \param[in] string value to serialize.
    /// \return The succeeding serializer.
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JStringType string) && noexcept -> Next
    {
        return this->Serialize([this, string]() noexcept {
            this->os_.get().put('"');
            this->os_.get() << internal::EscapedJsonString(string);
            this->os_.get().put('"');
        });
    }

    /// \brief Serializes a binary string value
    /// \details
    /// - Add a 's' to denote the following value as a string.
    /// - Serialize the length of the string value as four bytes big endian.
    /// - Write the string value.
    /// \param[in] string value to serialize.
    /// \return The succeeding serializer.
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JBinStringType string) && noexcept -> Next
    {
        return this->Serialize([this, string]() noexcept {
            this->os_.get().put('s');
            internal::SerializeLength(this->os_.get(), string.GetLength());
            const auto& value = string.GetValue();
            this->os_.get().write(value.data(), static_cast<std::streamsize>(value.size()));
        });
    }

    /// \brief Serializes a series of serializable values
    /// \details
    /// - Add an opening square bracket.
    /// - Serialize every value as a JSON value.
    /// - Add a closing square bracket.
    /// \tparam Fn Type of serializer function.
    /// \param[in] tuple to serialize.
    /// \return The succeeding serializer.
    /// \pre The function contained in the argument does not throw any exceptions
    template <typename Fn>
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JArrayType<Fn> tuple) && noexcept -> Next
    {
        return this->Serialize([this, tuple]() noexcept {
            this->os_.get().put('[');
            static_cast<void>(tuple.fn(ArrayStart(this->os_.get())));
            this->os_.get().put(']');
        });
    }

    /// \brief Serializes a binary value
    /// \details
    /// - Add a 'b' to denote the following value as binary.
    /// - Serialize the length of the binary value as four bytes big endian.
    /// - Write the binary value.
    /// \param[in] bin Value to serialize.
    /// \return The succeeding serializer.
    // coverity[autosar_cpp14_m9_3_3_violation]
    auto operator<<(JBinType bin) && noexcept -> Next
    {
        return this->Serialize([this, bin]() noexcept {
            this->os_.get().put('b');
            internal::SerializeLength(this->os_.get(), bin.GetLength());
            const auto& value = bin.GetValue();
            this->os_.get().write(value.data(), static_cast<std::streamsize>(value.size()));
        });
    }

    /// \brief Serializes an object
    /// \tparam Fn Type of serializer function.
    /// \param[in] object to serialize.
    /// \return The succeeding serializer.
    /// \pre The function contained in the argument does not throw any exceptions
    template <typename Fn>
    auto operator<<(JObjectType<Fn> object) && noexcept -> Next;

  private:
    /// \brief Serializes a value
    /// \details
    /// - If another element was serialized before:
    /// - Add a comma.
    /// - Execute the given serializer function.
    /// \tparam Fn Type of function.
    /// \param[in] fn Serializer call function.
    /// \return The succeeding serializer.
    /// \pre The passed function does not throw any exceptions
    template <typename Fn>
    auto Serialize(Fn&& fn) const noexcept -> Next
    {
        if (this->serializer_state_ == SerializerState::kNonEmpty)
        {
            this->os_.get().put(',');
        }
        std::forward<Fn>(fn)();
        return Next(this->os_.get(), SerializerState::kNonEmpty);
    }

    /// \brief Writes the requested BOM type
    /// \details
    /// - Write the requested BOM to the stream.
    /// \param[in] type The BOM type to write.
    void WriteBom(EncodingType type) const noexcept
    {
        if (type == EncodingType::kUtf8)
        {
            constexpr std::string_view kUtf8Bom{"\xEF\xBB\xBF"sv};
            this->os_.get().write(kUtf8Bom.data(), kUtf8Bom.size());
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

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_STRUCTURES_GENERIC_VALUE_SERIALIZER_H
