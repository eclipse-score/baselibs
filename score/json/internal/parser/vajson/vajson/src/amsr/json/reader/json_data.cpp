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
#include "amsr/json/reader/json_data.h"
#include <array>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>

#include "score/filesystem/filestream/file_factory.h"
#include "score/filesystem/filestream/file_stream.h"
#include "score/filesystem/path.h"

#include "amsr/json/reader/internal/json_ops.h"
namespace amsr {
namespace json {
/*!
 * \internal
 * - Initialize the stream buffer from the given input stream.
 * - Set the capacity for the key and string buffer to the values defined in the static configuration.
 * - Parse the BOM.
 * \endinternal
 */
JsonData::JsonData(std::istream& input_stream) noexcept : stream_{input_stream} {

  this->current_key_.reserve(internal::config::kKeyBufferSize);

  this->current_buffer_.reserve(internal::config::kStringBufferSize);
  this->ParseBom();
}

/*!
 * \internal
 * - Initialize the stream buffer from the given input stream.
 * - Store the given input stream.
 * \endinternal
 */


JsonData::JsonData(std::unique_ptr<std::istream> input_stream) noexcept : JsonData(*input_stream) {

  this->owned_stream_ = std::move(input_stream);
}

/*!
 * \internal
 * - Create the input stream from the file.
 * - If the file has been opened successfully.
 *   - Then create & return the JsonData object.
 * - Else return a JsonErrc containing the original error message.
 * \endinternal
 */
auto JsonData::FromFile(std::string_view const path) noexcept -> Result<JsonData> {
  // Open file using score filesystem
  score::filesystem::FileFactory factory{};
  score::filesystem::Path file_path{std::string(path.data(), path.size())};
  
  auto file_result = factory.Open(file_path, std::ios::in);
  auto result = MakeErrorResult<JsonData>(JsonErrc::kStreamFailure, "Could not open file");
  if (file_result.has_value()) {
    result.emplace(JsonData{std::move(file_result.value())});
  }
  
  return result;
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(std::string_view const buffer) noexcept -> Result<JsonData> {
  return JsonData::FromBuffer(score::cpp::span<char const>{buffer.data(), buffer.size()});
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(score::safecpp::zstring_view const buffer) noexcept -> Result<JsonData> {
  return JsonData::FromBuffer(score::cpp::span<char const>{buffer.data(), buffer.size()});
}

/*!
 * \internal
 * - Create the input stream from a buffer.
 * - Create & return the JsonData object.
 * \endinternal
 */
auto JsonData::FromBuffer(score::cpp::span<char const> const buffer) noexcept -> Result<JsonData> {
  // Create istringstream from the buffer
  std::unique_ptr<std::istream> iss{
    std::make_unique<std::istringstream>(std::string(buffer.data(), buffer.size()))
  };
  return Result<JsonData>{JsonData{std::move(iss)}};
}

/*!
 * \internal
 * - Get the current position from the stream.
 * - Store the DepthCounter and the current position internally.
 * - Return the Result of the operation.
 * \endinternal
 */
auto JsonData::Snap() noexcept -> Result<score::Blank> {
  std::streampos pos = this->GetStream().tellg();
  auto result = MakeErrorResult<score::Blank>(JsonErrc::kStreamFailure, "JsonData::Snap: Could not get stream position.");
  if (!this->GetStream().fail()) {
    result.emplace(score::Blank{});
    this->depth_counter_backup_ = this->depth_counter_;
    this->pos_backup_ = static_cast<std::uint64_t>(pos);
    this->has_backup_ = true;
  }

  return result;
}

/*!
 * \internal
 * - Check that a snapshot is available.
 * - Check that the stream position contained in the snapshot is small enough to be passed to seekg().
 * - Seek to the stream position.
 * - Check that the current position matches the requested seek position.
 * - Restore the internal DepthCounter using the stack and counter from the snapshot.
 * - Return the Result of the operation.
 * \endinternal
 */
auto JsonData::Restore() noexcept -> Result<score::Blank> {
  Result<score::Blank> result{MakeErrorResult<score::Blank>(JsonErrc::kStreamFailure, "JsonData::Restore: No snapshot available.")};

  if (this->has_backup_) {
    std::uint64_t const pos{this->pos_backup_};

    if (pos > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
      result = MakeErrorResult<score::Blank>(JsonErrc::kStreamFailure, "JsonData::Restore: Stream position exceeds max seek count.");
    } else {
      // Seek to position
      this->stream_.get().seekg(static_cast<std::streampos>(pos), std::ios::beg);

      if (this->stream_.get().fail()) {
        result = MakeErrorResult<score::Blank>(JsonErrc::kStreamFailure, "Unable to restore original position.");
      } else {
        // Verify position
        std::uint64_t const curr{static_cast<std::uint64_t>(this->stream_.get().tellg())};
        if (curr != this->pos_backup_) {
          result = MakeErrorResult<score::Blank>(JsonErrc::kStreamFailure, "Unable to restore original position.");
        } else {
          this->depth_counter_ = std::move(this->depth_counter_backup_);
          this->has_backup_ = false;
          result = Result<score::Blank>{score::Blank{}};
        }
      }
    }
  }

  return result;
}

/*!
 * \internal
 * - If the first three bytes of the document are the UTF-8 BOM:
 *   - Set the encoding type.
 *   - Move the stream buffer past the BOM.
 * \endinternal
 */
void JsonData::ParseBom() noexcept {
  constexpr std::array<char const, 3> kUtf8Bom{'\xEF', '\xBB', '\xBF'};
  std::string_view const view{kUtf8Bom.data(), kUtf8Bom.size()};

  internal::JsonOps ops{*this};

  Result<bool> const result{ops.ReadString(view)};

  if (result.has_value()) {
    if (result.value() == true) {
      // It worked, so it was a utf-8 BOM.
      encoding_ = EncodingType::kUtf8;
    }
  }
}

}  // namespace json
}  // namespace amsr
