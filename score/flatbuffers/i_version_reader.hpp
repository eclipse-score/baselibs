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

#ifndef SCORE_LIB_FLATBUFFERS_I_VERSION_READER_HPP
#define SCORE_LIB_FLATBUFFERS_I_VERSION_READER_HPP

#include "score/flatbuffers/buffer_version_info.hpp"
#include "score/flatbuffers/error.hpp"
#include "score/result/result.h"

#include <score/span.hpp>

#include <cstdint>
#include <vector>

namespace score
{
namespace flatbuffers
{

/// @brief Abstract interface for reading and verifying the version information
///        embedded in a FlatBuffer that follows the common buffer
///        identification convention.
///
/// Buffers passed to `IVersionReader::GetVersion` and `IVersionReader::VerifyVersion`
/// must have `version_info:score.flatbuffers.BufferVersion` as field 0 of their
/// root table, with version values explicitly stored in the binary.
/// This is guaranteed when the buffer was produced by e.g.
/// the `serialize_versioned_buffer` Bazel rule.
///
/// ### Dependency injection
/// Components that need testability should accept `IVersionReader&` in their
/// constructor and receive a `VersionReader` instance in production code.
///
/// If dependency injection is not required (e.g. standalone tools or simple
/// one-off call sites), use the free functions `GetVersion` / `VerifyVersion`
/// declared in `score/flatbuffers/version_reader.hpp` instead.
class IVersionReader
{
  public:
    virtual ~IVersionReader() = default;

    /// @brief Reads the 4-char file_identifier and the `BufferVersion`
    ///        values from a raw FlatBuffer.
    ///
    /// @param[in] buf  View over the FlatBuffer binary data.
    ///
    /// @returns `BufferVersionInfo` on success, or a `score::result::Error`
    ///          on failure.
    virtual score::Result<BufferVersionInfo> GetVersion(score::cpp::span<const uint8_t> buf) noexcept = 0;

    /// @brief Reads the 4-char file_identifier and the `BufferVersion`
    ///        values from a vector-backed FlatBuffer.
    ///
    /// @param[in] buf  FlatBuffer binary data.
    ///
    /// @returns `BufferVersionInfo` on success, or a `score::result::Error`
    ///          on failure.
    virtual score::Result<BufferVersionInfo> GetVersion(const std::vector<uint8_t>& buf) noexcept = 0;

    /// @brief Reads the buffer version and verifies it matches @p expected.
    ///
    /// Both the `file_identifier` (4-char tag) and the `major_version` /
    /// `minor_version` fields are compared against @p expected.
    ///
    /// @param[in] buf      View over the FlatBuffer binary data.
    /// @param[in] expected The `BufferVersionInfo` the caller considers valid.
    /// @param[in] mode     `VersionMatchMode::kExact` (default) requires an
    ///                     exact match on both version fields; `kMinorMinimum`
    ///                     accepts any minor version >= the expected value
    ///                     while still requiring an exact major version match.
    ///
    /// @returns `score::Result<void>` on success, or a `score::result::Error`
    ///          if the buffer is invalid or the version does not match.
    virtual score::Result<void> VerifyVersion(score::cpp::span<const uint8_t> buf,
                                              const BufferVersionInfo& expected,
                                              VersionMatchMode mode = VersionMatchMode::kExact) noexcept = 0;

    /// @brief Reads the buffer version and verifies it matches @p expected.
    ///
    /// @param[in] buf      FlatBuffer binary data.
    /// @param[in] expected The `BufferVersionInfo` the caller considers valid.
    /// @param[in] mode     Version comparison mode.
    ///
    /// @returns `score::Result<void>` on success, or a `score::result::Error`
    ///          if the buffer is invalid or the version does not match.
    virtual score::Result<void> VerifyVersion(const std::vector<uint8_t>& buf,
                                              const BufferVersionInfo& expected,
                                              VersionMatchMode mode = VersionMatchMode::kExact) noexcept = 0;

  protected:
    IVersionReader() = default;
};

}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_I_VERSION_READER_HPP
