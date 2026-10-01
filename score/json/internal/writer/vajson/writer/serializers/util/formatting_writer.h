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
/// \brief Output stream wrapper that emits the structural characters and the optional insignificant whitespace.

#ifndef SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_FORMATTING_WRITER_H
#define SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_FORMATTING_WRITER_H

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace score::json::vajson
{
/// \brief Layout of the JSON representation emitted by the vaJSON serializer
enum class VajsonFormatting : std::uint8_t
{
    /// \brief No insignificant whitespace is emitted
    kCompact,
    /// \brief The output is indented and spread over multiple lines to be human readable
    kPrettyPrint,
};

/// \brief Writes the structural characters of a JSON document, cf. RFC 8259 section 2
/// \details In compact mode no insignificant whitespace is emitted.
///     In pretty print mode every array element and object member is placed on its own line, indented by four spaces
///     per nesting level, and the name separator is followed by a single space.
///     Empty arrays and objects are written as `[]` and `{}` in both modes.
///     The serializers share one instance per document and only reference it, hence it must outlive them.
class FormattingWriter final
{
  public:
    /// \brief Constructs a FormattingWriter
    /// \param[in] stream Output stream to write into. It must outlive this instance.
    /// \param[in] formatting Whether insignificant whitespace is emitted to make the output human readable.
    FormattingWriter(std::ostream& stream, const VajsonFormatting formatting) noexcept
        : stream_{stream}, formatting_{formatting}
    {
    }

    FormattingWriter(const FormattingWriter&) = delete;
    FormattingWriter(FormattingWriter&&) = delete;
    auto operator=(const FormattingWriter&) -> FormattingWriter& = delete;
    auto operator=(FormattingWriter&&) -> FormattingWriter& = delete;
    ~FormattingWriter() noexcept = default;

    /// \brief Returns the underlying output stream to write scalar values into
    /// \return The output stream passed on construction.
    auto Stream() const noexcept -> std::ostream&
    {
        return stream_;
    }

    /// \brief Writes the begin of an array or object and enters its nesting level
    /// \param[in] begin The structural character opening the container, either '[' or '{'.
    void BeginContainer(const char begin) noexcept
    {
        static_cast<void>(stream_.put(begin));
        ++depth_;
        container_empty_ = true;
    }

    /// \brief Writes the separation in front of an array element or object member
    /// \param[in] preceded_by_element Whether another element of the same container was written before.
    void BeginElement(const bool preceded_by_element) noexcept
    {
        if (preceded_by_element)
        {
            static_cast<void>(stream_.put(','));
        }
        WriteLineBreak();
        container_empty_ = false;
    }

    /// \brief Writes the name separator between an object member's name and its value
    void WriteNameSeparator() const noexcept
    {
        static_cast<void>(stream_.put(':'));
        if (formatting_ == VajsonFormatting::kPrettyPrint)
        {
            static_cast<void>(stream_.put(' '));
        }
    }

    /// \brief Leaves the nesting level of an array or object and writes its end
    /// \details The enclosing container, if any, is non-empty afterwards, since the closed container is one of its
    ///     elements.
    /// \param[in] end The structural character closing the container, either ']' or '}'.
    void EndContainer(const char end) noexcept
    {
        --depth_;
        if (!container_empty_)
        {
            WriteLineBreak();
        }
        static_cast<void>(stream_.put(end));
        container_empty_ = false;
    }

  private:
    /// \brief Starts a new line indented to the current nesting level, if pretty printing is enabled
    void WriteLineBreak() const noexcept
    {
        if (formatting_ == VajsonFormatting::kPrettyPrint)
        {
            static_cast<void>(stream_.put('\n'));
            for (std::size_t level{0U}; level < depth_; ++level)
            {
                static_cast<void>(
                    stream_.write(kIndentation.data(), static_cast<std::streamsize>(kIndentation.size())));
            }
        }
    }

    /// \brief Indentation written per nesting level
    static constexpr std::string_view kIndentation{"    "};

    /// \brief Output stream to write into
    std::ostream& stream_;

    /// \brief Whether insignificant whitespace is emitted
    VajsonFormatting formatting_;

    /// \brief Number of arrays and objects that enclose the current position
    std::size_t depth_{0U};

    /// \brief Whether no element was written into the innermost open container yet
    bool container_empty_{false};
};

}  // namespace score::json::vajson

#endif  // SCORE_LIB_JSON_INTERNAL_WRITER_VAJSON_WRITER_SERIALIZERS_UTIL_FORMATTING_WRITER_H
