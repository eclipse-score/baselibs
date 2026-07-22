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

#ifndef SCORE_LIB_FLATBUFFERS_VERSION_READER_HPP
#define SCORE_LIB_FLATBUFFERS_VERSION_READER_HPP

#include "score/flatbuffers/i_version_reader.hpp"

namespace score
{
namespace flatbuffers
{

/// @brief Default implementation of `IVersionReader`.
///
/// Reads and verifies the `BufferVersionInfo` embedded in any FlatBuffer that
/// follows the common buffer identification convention (a root table with
/// `version_info:score.flatbuffers.BufferVersion` as field 0).
///
class VersionReader : public IVersionReader
{
  public:
    VersionReader() = default;
    ~VersionReader() override = default;

    /// @copydoc IVersionReader::GetVersion(score::cpp::span<const uint8_t>)
    score::Result<BufferVersionInfo> GetVersion(score::cpp::span<const uint8_t> buf) noexcept override;

    /// @copydoc IVersionReader::GetVersion(const std::vector<uint8_t>&)
    score::Result<BufferVersionInfo> GetVersion(const std::vector<uint8_t>& buf) noexcept override;

    /// @copydoc IVersionReader::VerifyVersion(score::cpp::span<const uint8_t>,
    ///          const BufferVersionInfo&, VersionMatchMode)
    score::Result<void> VerifyVersion(score::cpp::span<const uint8_t> buf,
                                      const BufferVersionInfo& expected,
                                      VersionMatchMode mode = VersionMatchMode::kExact) noexcept override;

    /// @copydoc IVersionReader::VerifyVersion(const std::vector<uint8_t>&,
    ///          const BufferVersionInfo&, VersionMatchMode)
    score::Result<void> VerifyVersion(const std::vector<uint8_t>& buf,
                                      const BufferVersionInfo& expected,
                                      VersionMatchMode mode = VersionMatchMode::kExact) noexcept override;
};

// ─── Convenience free functions ──────────────────────────────────────────────
// Use these when dependency injection is not needed, e.g. in standalone tools,
// integration tests, or one-off call sites.  For components that require
// testability via mocking, prefer accepting an `IVersionReader&` and passing a
// `VersionReader` instance at construction time instead.

/// @brief Reads the 4-char file_identifier and the `BufferVersion` values
///        from any FlatBuffer that conforms to the common buffer
///        identification convention.
///
/// @param[in] buf  View over the FlatBuffer binary data.
score::Result<BufferVersionInfo> GetVersion(score::cpp::span<const uint8_t> buf) noexcept;

/// @brief Reads the 4-char file_identifier and the `BufferVersion` values
///        from any FlatBuffer that conforms to the common buffer
///        identification convention.
///
/// @param[in] buf  FlatBuffer binary data, e.g. as returned by
///                 `score::flatbuffers::LoadBuffer`.
score::Result<BufferVersionInfo> GetVersion(const std::vector<uint8_t>& buf) noexcept;

/// @brief Reads the buffer version and verifies it matches @p expected.
///
/// @param[in] buf      View over the FlatBuffer binary data.
/// @param[in] expected The `BufferVersionInfo` the caller considers valid.
/// @param[in] mode     Comparison mode (exact or minor-minimum).
score::Result<void> VerifyVersion(score::cpp::span<const uint8_t> buf,
                                  const BufferVersionInfo& expected,
                                  VersionMatchMode mode = VersionMatchMode::kExact) noexcept;

/// @brief Reads the buffer version and verifies it matches @p expected.
///
/// @param[in] buf      FlatBuffer binary data, e.g. as returned by
///                     `score::flatbuffers::LoadBuffer`.
/// @param[in] expected The `BufferVersionInfo` the caller considers valid.
/// @param[in] mode     Comparison mode (exact or minor-minimum).
score::Result<void> VerifyVersion(const std::vector<uint8_t>& buf,
                                  const BufferVersionInfo& expected,
                                  VersionMatchMode mode = VersionMatchMode::kExact) noexcept;

}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_VERSION_READER_HPP
