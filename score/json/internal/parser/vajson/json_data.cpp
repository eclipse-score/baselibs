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
/// @file
/// @brief  json data

#include "score/json/internal/parser/vajson/json_data.h"
#include <array>
#include <cstddef>
#include <ios>
#include <istream>
#include <string>
#include <utility>
#include <vector>

#include "score/filesystem/error.h"
#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/filesystem/path.h"

#include "score/json/internal/parser/vajson/json_ops.h"
namespace score
{
namespace json
{
namespace vajson
{
/// @internal
/// - Initialize the unread input with the whole document.
/// - Set the capacity for the string buffer to the value defined in the static configuration.
/// - Parse the BOM.
/// @endinternal
JsonData::JsonData(const StringView document) noexcept : input_{document}
{
    this->current_buffer_.reserve(internal::config::kStringBufferSize);
    this->ParseBom();
}

/// @internal
/// - Store the document and initialize the unread input with it.
/// - Set the capacity for the string buffer to the value defined in the static configuration.
/// - Parse the BOM.
/// @endinternal
JsonData::JsonData(std::vector<char> document) noexcept
    : owned_document_{std::move(document)}, input_{owned_document_.data(), owned_document_.size()}
{
    this->current_buffer_.reserve(internal::config::kStringBufferSize);
    this->ParseBom();
}

/// @internal
/// - Open the file, returning a JsonErrc if it is not a regular file.
/// - If the file has been opened successfully:
///   - Read the whole file into memory.
///   - If the file has been read successfully, then create & return the JsonData object owning the content.
/// - Otherwise return a JsonErrc containing the error message.
/// @endinternal
auto JsonData::FromFile(const std::string_view path) noexcept -> Result<JsonData>
{
    // Open file using score filesystem
    score::filesystem::FileFactory factory{};
    const score::filesystem::Path file_path{std::string(path.data(), path.size())};

    auto file_result{factory.OpenRegularFile(file_path, std::ios::in | std::ios::binary)};
    if ((!file_result.has_value()) && (file_result.error() == score::filesystem::ErrorCode::kNotARegularFile))
    {
        return score::MakeUnexpected(JsonErrc::kStreamFailure, "Not a regular file");
    }
    Result<JsonData> result{score::MakeUnexpected(JsonErrc::kStreamFailure, "Could not open file")};
    if (file_result.has_value())
    {
        std::istream& file{*file_result.value()};
        file.seekg(0, std::ios::end);
        const std::streamoff size{file.tellg()};
        file.seekg(0, std::ios::beg);
        if (file.fail() || (size < 0))
        {
            return score::MakeUnexpected(JsonErrc::kStreamFailure, "Could not read file");
        }
        std::vector<char> content(static_cast<std::size_t>(size));
        // Read through the unformatted input layer so an I/O error sets badbit and a short read is detectable
        static_cast<void>(file.read(content.data(), size));
        if (file.bad() || (file.gcount() != size))
        {
            return score::MakeUnexpected(JsonErrc::kStreamFailure, "Could not read file");
        }
        result.emplace(JsonData{std::move(content)});
    }

    return result;
}

/// @internal
/// - Create & return the JsonData object reading from the buffer.
/// @endinternal
auto JsonData::FromBuffer(const std::string_view buffer) noexcept -> Result<JsonData>
{
    return Result<JsonData>{JsonData{buffer}};
}

/// @internal
/// - If the document starts with the UTF-8 BOM:
///   - Skip the BOM.
/// @endinternal
void JsonData::ParseBom() noexcept
{
    constexpr std::array<const char, 3> kUtf8Bom{'\xEF', '\xBB', '\xBF'};

    internal::JsonOps ops{*this};
    static_cast<void>(ops.ReadString(StringView{kUtf8Bom.data(), kUtf8Bom.size()}));
}

}  // namespace vajson
}  // namespace json
}  // namespace score
