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
 *        \brief  json data
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "score/json/internal/parser/vajson/reader/json_data.h"
#include <array>
#include <istream>
#include <memory>
#include <sstream>
#include <utility>

#include "score/json/internal/parser/vajson/reader/internal/json_ops.h"
namespace score
{
namespace json
{
namespace vajson
{
/*!
 * \internal
 * - Initialize the stream buffer from the given input stream.
 * - Set the capacity for the string buffer to the value defined in the static configuration.
 * - Parse the BOM.
 * \endinternal
 */
JsonData::JsonData(std::istream& input_stream) noexcept : stream_{input_stream}
{
    this->current_buffer_.reserve(internal::config::kStringBufferSize);
    this->ParseBom();
}

/*!
 * \internal
 * - Initialize the stream buffer from the given input stream.
 * - Store the given input stream.
 * \endinternal
 */

JsonData::JsonData(std::unique_ptr<std::istream> input_stream) noexcept : JsonData(*input_stream)
{

    this->owned_stream_ = std::move(input_stream);
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(const std::string_view buffer) noexcept -> Result<JsonData>
{
    return JsonData::FromBuffer(score::cpp::span<const char>{buffer.data(), buffer.size()});
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(const score::safecpp::zstring_view buffer) noexcept -> Result<JsonData>
{
    return JsonData::FromBuffer(score::cpp::span<const char>{buffer.data(), buffer.size()});
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(const score::cpp::span<const char> buffer) noexcept -> Result<JsonData>
{
    // Create istringstream from the buffer
    std::unique_ptr<std::istream> iss{std::make_unique<std::istringstream>(std::string(buffer.data(), buffer.size()))};
    return Result<JsonData>{JsonData{std::move(iss)}};
}

/*!
 * \internal
 * - If the first three bytes of the document are the UTF-8 BOM:
 *   - Set the encoding type.
 *   - Move the stream buffer past the BOM.
 * - If the stream is too short for a BOM check:
 *   - Ensure the stream state is cleared and position is restored.
 * \endinternal
 */
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
        auto& stream = this->stream_.get();
        stream.clear(stream.rdstate() & ~(std::ios::failbit | std::ios::eofbit));
        stream.seekg(0, std::ios::beg);
    }
}

}  // namespace vajson
}  // namespace json
}  // namespace score
