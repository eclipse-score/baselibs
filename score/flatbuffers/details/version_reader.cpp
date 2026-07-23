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

#include "score/flatbuffers/version_reader.hpp"
#include "score/flatbuffers/details/version_reader_impl.hpp"

#include <vector>

static_assert(score::flatbuffers::BufferVersionInfo::kIdentifierLength == ::flatbuffers::kFileIdentifierLength,
              "BufferVersionInfo identifier length must match FlatBuffers file identifier length");

namespace score
{
namespace flatbuffers
{

// ─── Implementation ───────────────────────────────────────────────────────────

namespace
{

inline score::Result<BufferVersionInfo> GetVersionVecImpl(const std::vector<uint8_t>& buf) noexcept
{
    if (!buf.empty())
    {
        return detail::GetVersionImpl<>(score::cpp::span<const uint8_t>{buf.data(), buf.size()});
    }
    else
    {
        // Avoid calling GetVersionImpl with a null/zero-size span.
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }
}

inline score::Result<void> VerifyVersionImpl(const score::cpp::span<const uint8_t> buf,
                                             const BufferVersionInfo& expected,
                                             const VersionMatchMode mode) noexcept
{
    const auto result = detail::GetVersionImpl<>(buf);
    if (!result.has_value())
    {
        return score::MakeUnexpected<void>(result.error());
    }

    const auto& actual = result.value();

    const bool match = (mode == VersionMatchMode::kMinorMinimum) ? ((actual.identifier() == expected.identifier()) &&
                                                                    (actual.major_version == expected.major_version) &&
                                                                    (actual.minor_version >= expected.minor_version))
                                                                 : (actual == expected);

    if (!match)
    {
        return MakeUnexpected(ErrorCode::kVersionMismatch);
    }

    return {};
}

inline score::Result<void> VerifyVersionImpl(const std::vector<uint8_t>& buf,
                                             const BufferVersionInfo& expected,
                                             const VersionMatchMode mode) noexcept
{
    if (!buf.empty())
    {
        return VerifyVersionImpl(score::cpp::span<const uint8_t>{buf.data(), buf.size()}, expected, mode);
    }
    else
    {
        // Avoid calling VerifyVersionImpl with a null/zero-size span.
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }
}

}  // namespace

// ─── VersionReader class ──────────────────────────────────────────────────────

score::Result<BufferVersionInfo> VersionReader::GetVersion(const score::cpp::span<const uint8_t> buf) noexcept
{
    return detail::GetVersionImpl<>(buf);
}

score::Result<BufferVersionInfo> VersionReader::GetVersion(const std::vector<uint8_t>& buf) noexcept
{
    return GetVersionVecImpl(buf);
}

score::Result<void> VersionReader::VerifyVersion(const score::cpp::span<const uint8_t> buf,
                                                 const BufferVersionInfo& expected,
                                                 const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

score::Result<void> VersionReader::VerifyVersion(const std::vector<uint8_t>& buf,
                                                 const BufferVersionInfo& expected,
                                                 const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

// ─── Convenience free functions ──────────────────────────────────────────────

score::Result<BufferVersionInfo> GetVersion(const score::cpp::span<const uint8_t> buf) noexcept
{
    return detail::GetVersionImpl<>(buf);
}

score::Result<BufferVersionInfo> GetVersion(const std::vector<uint8_t>& buf) noexcept
{
    return GetVersionVecImpl(buf);
}

score::Result<void> VerifyVersion(const score::cpp::span<const uint8_t> buf,
                                  const BufferVersionInfo& expected,
                                  const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

score::Result<void> VerifyVersion(const std::vector<uint8_t>& buf,
                                  const BufferVersionInfo& expected,
                                  const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

}  // namespace flatbuffers
}  // namespace score
