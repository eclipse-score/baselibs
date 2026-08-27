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

#ifndef SCORE_LIB_FLATBUFFERS_BUFFER_VERSION_INFO_HPP
#define SCORE_LIB_FLATBUFFERS_BUFFER_VERSION_INFO_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace score
{
namespace flatbuffers
{

/// @brief Data extracted from a conforming FlatBuffer using the universal
///        buffer identification convention.
///
/// The field names mirror the `BufferVersion` FlatBuffers table defined in
/// `buffer_version.fbs` and the 4-char `file_identifier`.
///
/// Buffers passed to `IVersionReader::GetVersion` and
/// `IVersionReader::VerifyVersion` must have
/// `version_info:score.flatbuffers.BufferVersion` as field 0 of their root
/// table, with version values explicitly stored in the binary.
/// This is guaranteed when the buffer was produced by e.g. the
/// `serialize_versioned_buffer` Bazel rule.
class BufferVersionInfo
{
  public:
    /// Number of characters in a FlatBuffers file identifier.
    static constexpr std::size_t kIdentifierLength = 4U;

    /// @brief Convenience constructor.
    ///
    /// @param id    Exactly a 4-character string literal (e.g. `"DEMO"`).
    /// @param major Major version number.
    /// @param minor Minor version number.
    constexpr BufferVersionInfo(const char (&id)[kIdentifierLength + 1U], uint16_t major, uint16_t minor) noexcept
        : major_version{major}, minor_version{minor}, identifier_{id[0], id[1], id[2], id[3]}
    {
    }

    /// @brief Default constructor – leaves all fields zero-initialised.
    BufferVersionInfo() noexcept : major_version{0U}, minor_version{0U}, identifier_{} {}

    uint16_t major_version;
    uint16_t minor_version;

    /// @brief Returns a view of the 4-char file identifier.
    std::string_view identifier() const noexcept;

    /// @brief Equality: identifier, major_version, and minor_version must all match.
    bool operator==(const BufferVersionInfo& rhs) const noexcept;

    /// @brief Inequality: true when any field differs.
    bool operator!=(const BufferVersionInfo& rhs) const noexcept;

  private:
    char identifier_[kIdentifierLength];
};

/// @brief Controls how `IVersionReader::VerifyVersion` compares the minor version.
enum class VersionMatchMode
{
    /// Both `major_version` and `minor_version` must match exactly.
    kExact,
    /// `major_version` must match exactly; `minor_version` of the buffer must
    /// be greater than or equal to the expected value (backwards-compatible
    /// minimum-version check).
    kMinorMinimum,
};

}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_BUFFER_VERSION_INFO_HPP
