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
#include <algorithm>
#include <array>
#include <cstddef>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/filesystem/filesystem.h"
#include "score/filesystem/path.h"

#include "score/json/internal/parser/vajson/json_ops.h"
namespace score
{
namespace json
{
namespace vajson
{
namespace
{
constexpr std::size_t kReadChunkSize{std::size_t{64U} * 1024U};
}  // namespace

/// @internal
/// - Initialize the stream buffer from the given input stream.
/// - Set the capacity for the string buffer to the value defined in the static configuration.
/// - Parse the BOM.
/// @endinternal
JsonData::JsonData(std::istream& input_stream) noexcept : stream_{input_stream}
{
    this->current_buffer_.reserve(internal::config::kStringBufferSize);
    this->ParseBom();
}

/// @internal
/// - Initialize the stream buffer from the given input stream.
/// - Store the given input stream.
/// @endinternal

JsonData::JsonData(std::unique_ptr<std::istream> input_stream) noexcept : JsonData(*input_stream)
{

    this->owned_stream_ = std::move(input_stream);
}

/// @internal
/// - Return a JsonErrc if the path does not refer to a regular file.
/// - Create the input stream from the file.
/// - If the file has been opened successfully.
///   - Reserve the buffer for the reported file size, capped at max_size.
///   - Read the file in chunks, returning a JsonErrc once it exceeds max_size.
///   - Then create & return the JsonData object.
/// - Else return a JsonErrc containing the original error message.
/// @endinternal
auto JsonData::FromFile(const std::string_view path, const std::size_t max_size) noexcept -> Result<JsonData>
{
    // Open file using score filesystem
    score::filesystem::FileFactory factory{};
    score::filesystem::Path file_path{std::string(path.data(), path.size())};

    // Opening a FIFO or device could block or never reach end of file.
    const auto is_regular_file{score::filesystem::StandardFilesystem{}.IsRegularFile(file_path)};
    if (!is_regular_file.has_value())
    {
        return MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Could not open file");
    }
    if (!is_regular_file.value())
    {
        return MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Not a regular file");
    }

    auto file_result{factory.Open(file_path, std::ios::in | std::ios::binary)};
    auto result{MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Could not open file")};
    if (file_result.has_value())
    {
        // Parse an in-memory copy because the reader's seekg calls are expensive on a file but cheap on a stringbuf.
        std::istream& file{*file_result.value()};
        file.seekg(0, std::ios::end);
        const std::streamoff reported_size{file.tellg()};
        file.seekg(0, std::ios::beg);
        if (file.fail() || (reported_size < 0))
        {
            return MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Could not read file");
        }
        std::string content{};
        // The reported size only avoids reallocations; the limit applies to the bytes actually read.
        content.reserve(std::min(static_cast<std::size_t>(reported_size), max_size));
        std::string chunk(kReadChunkSize, '\0');
        while (file.good())
        {
            static_cast<void>(file.read(chunk.data(), static_cast<std::streamsize>(chunk.size())));
            const auto count{static_cast<std::size_t>(file.gcount())};
            if (count > (max_size - content.size()))
            {
                return MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "File too large");
            }
            static_cast<void>(content.append(chunk.data(), count));
        }
        if (file.bad())
        {
            return MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Could not read file");
        }
        std::unique_ptr<std::istream> stream{std::make_unique<std::istringstream>(std::move(content))};
        result.emplace(JsonData{std::move(stream)});
    }

    return result;
}

/// @internal
/// - Create the input stream from a buffer.
/// - Create & return the JsonData object.
/// @endinternal
auto JsonData::FromBuffer(const std::string_view buffer) noexcept -> Result<JsonData>
{
    return JsonData::FromBuffer(score::cpp::span<const char>{buffer.data(), buffer.size()});
}

/// @internal
/// - Create the input stream from a buffer.
/// - Create & return the JsonData object.
/// @endinternal
auto JsonData::FromBuffer(const score::safecpp::zstring_view buffer) noexcept -> Result<JsonData>
{
    return JsonData::FromBuffer(score::cpp::span<const char>{buffer.data(), buffer.size()});
}

/// @internal
/// - Create the input stream from a buffer.
/// - Create & return the JsonData object.
/// @endinternal
auto JsonData::FromBuffer(const score::cpp::span<const char> buffer) noexcept -> Result<JsonData>
{
    // Create istringstream from the buffer
    std::unique_ptr<std::istream> iss{std::make_unique<std::istringstream>(std::string(buffer.data(), buffer.size()))};
    return Result<JsonData>{JsonData{std::move(iss)}};
}

/// @internal
/// - If the first three bytes of the document are the UTF-8 BOM:
///   - Set the encoding type.
///   - Move the stream buffer past the BOM.
/// - If the stream is too short for a BOM check:
///   - Ensure the stream state is cleared and position is restored.
/// @endinternal
void JsonData::ParseBom() noexcept
{
    constexpr std::array<const char, 3> kUtf8Bom{'\xEF', '\xBB', '\xBF'};
    const std::string_view view{kUtf8Bom.data(), kUtf8Bom.size()};

    internal::JsonOps ops{*this};

    const Result<bool> result{ops.ReadString(view)};

    if (result.has_value())
    {
        if (result.value() == true)
        {
            // It worked, so it was a utf-8 BOM.
            encoding_ = EncodingType::kUtf8;
        }
    }
    else
    {
        // ReadString failed (e.g., stream shorter than BOM).
        // Ensure stream is in a usable state for subsequent parsing.
        auto& stream{this->stream_.get()};
        stream.clear(stream.rdstate() & ~(std::ios::failbit | std::ios::eofbit));
        stream.seekg(0, std::ios::beg);
    }
}

}  // namespace vajson
}  // namespace json
}  // namespace score
