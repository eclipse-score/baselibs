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

#include "score/json/internal/writer/serializer.h"

#include "score/json/internal/model/error.h"
#include "score/json/internal/writer/vajson/escaped_json_string.h"
#include "score/json/internal/writer/vajson/formatting_writer.h"

#include <score/assert.hpp>

#include <charconv>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <sstream>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace score::json::internal::writer
{
namespace
{

using score::json::vajson::FormattingWriter;
using score::json::vajson::internal::EscapedJsonString;

void WriteText(std::ostream& out_stream, const std::string_view text) noexcept
{
    static_cast<void>(out_stream.write(text.data(), static_cast<std::streamsize>(text.size())));
}

void WriteString(std::ostream& out_stream, const std::string_view value) noexcept
{
    static_cast<void>(out_stream.put('"'));
    out_stream << EscapedJsonString{value};
    static_cast<void>(out_stream.put('"'));
}

template <typename T>
void WriteArithmetic(std::ostream& out_stream, const T value) noexcept
{
    // Longer than the longest output: 24 characters for double, 20 for std::int64_t.
    constexpr std::size_t kBufferSize{64U};
    std::array<char, kBufferSize> buffer{};
    const auto conversion_result{std::to_chars(buffer.data(), buffer.data() + buffer.size(), value)};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(conversion_result.ec == std::errc{},
                                                "Could not convert number to textual representation.");
    WriteText(out_stream,
              std::string_view{buffer.data(), static_cast<std::size_t>(conversion_result.ptr - buffer.data())});
}

void WriteNumber(std::ostream& out_stream, const Number& number) noexcept
{
    number.Visit([&out_stream](const auto value) noexcept {
        using Stored = std::decay_t<decltype(value)>;
        if constexpr (std::is_floating_point_v<Stored>)
        {
            // RFC 8259, section 6 has no representation for infinity and NaN.
            if (std::isfinite(value))
            {
                WriteArithmetic(out_stream, value);
            }
            else
            {
                out_stream.setstate(std::ios_base::failbit);
            }
        }
        else if constexpr (std::is_unsigned_v<Stored>)
        {
            WriteArithmetic(out_stream, static_cast<std::uint64_t>(value));
        }
        else
        {
            WriteArithmetic(out_stream, static_cast<std::int64_t>(value));
        }
    });
}

void WriteValue(FormattingWriter& writer, const Any& value) noexcept;

// NOLINTNEXTLINE(misc-no-recursion) stack depth equals the nesting depth of the written Any
void WriteObject(FormattingWriter& writer, const Object& object) noexcept
{
    writer.BeginContainer('{');
    bool written_before{false};
    for (const auto& [key, value] : object)
    {
        writer.BeginElement(written_before);
        WriteString(writer.Stream(), key.GetAsStringView());
        writer.WriteNameSeparator();
        WriteValue(writer, value);
        written_before = true;
    }
    writer.EndContainer('}');
}

// NOLINTNEXTLINE(misc-no-recursion) stack depth equals the nesting depth of the written Any
void WriteList(FormattingWriter& writer, const List& list) noexcept
{
    writer.BeginContainer('[');
    bool written_before{false};
    for (const auto& value : list)
    {
        writer.BeginElement(written_before);
        WriteValue(writer, value);
        written_before = true;
    }
    writer.EndContainer(']');
}

// NOLINTNEXTLINE(misc-no-recursion) stack depth equals the nesting depth of the written Any
void WriteValue(FormattingWriter& writer, const Any& value) noexcept
{
    if (const auto object = value.As<Object>(); object.has_value())
    {
        WriteObject(writer, object->get());
    }
    else if (const auto list = value.As<List>(); list.has_value())
    {
        WriteList(writer, list->get());
    }
    else if (const auto string_value = value.As<std::string_view>(); string_value.has_value())
    {
        WriteString(writer.Stream(), *string_value);
    }
    else if (const auto number = value.As<Number>(); number.has_value())
    {
        WriteNumber(writer.Stream(), number->get());
    }
    else if (const auto boolean = value.As<bool>(); boolean.has_value())
    {
        WriteText(writer.Stream(), *boolean ? "true" : "false");
    }
    else
    {
        // Null is the only alternative of Any left.
        WriteText(writer.Stream(), "null");
    }
}

template <typename T>
score::Result<void> SerializeToStreamInternal(std::ostream& out_stream, const T& json_data, const Formatting formatting)
{
    FormattingWriter writer{out_stream, formatting};
    if constexpr (std::is_same_v<T, Object>)
    {
        WriteObject(writer, json_data);
    }
    else if constexpr (std::is_same_v<T, List>)
    {
        WriteList(writer, json_data);
    }
    else
    {
        WriteValue(writer, json_data);
    }

    // A default constructed Result<void> already carries the success state.
    score::Result<void> result{};
    if (out_stream.fail())
    {
        result = score::MakeUnexpected(Error::kUnknownError, "JSON writer failed to write to stream");
    }

    return result;
}

template <typename T>
score::Result<std::string> SerializeToBufferInternal(const T& json_data, const Formatting formatting)
{
    std::ostringstream out_stream{};

    // and_then keeps a single exit point: the buffer is only extracted once serialization succeeded,
    // and any error is forwarded unchanged.
    return SerializeToStreamInternal(out_stream, json_data, formatting)
        .and_then([&out_stream](auto&&...) -> score::Result<std::string> {
            return out_stream.str();
        });
}

}  // namespace

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::Object& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::List& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<void> SerializeToStream(std::ostream& out_stream,
                                      const score::json::Any& json_data,
                                      const score::json::Formatting formatting)
{
    return SerializeToStreamInternal(out_stream, json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::Object& json_data,
                                             const score::json::Formatting formatting)
{
    return SerializeToBufferInternal(json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::List& json_data,
                                             const score::json::Formatting formatting)
{
    return SerializeToBufferInternal(json_data, formatting);
}

score::Result<std::string> SerializeToBuffer(const score::json::Any& json_data,
                                             const score::json::Formatting formatting)
{
    return SerializeToBufferInternal(json_data, formatting);
}

}  // namespace score::json::internal::writer
