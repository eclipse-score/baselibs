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

#ifndef SCORE_LIB_FLATBUFFERS_VERSION_READER_IMPL_HPP
#define SCORE_LIB_FLATBUFFERS_VERSION_READER_IMPL_HPP

/// @file version_reader_impl.hpp
///
/// Internal implementation header — NOT part of the public API.
///
/// Listed in the `flatbufferutils` Bazel target's `srcs` (like
/// `load_buffer_internal.hpp`) so that `version_reader.cpp` and the unit-test
/// translation unit can both include it without it becoming a public header.
///
/// The `GetVersionImpl` function template lives here rather than in an
/// anonymous namespace inside `version_reader.cpp` so that tests can
/// instantiate it with a custom `GetEnvelope` function pointer, covering
/// the otherwise-unreachable `envelope == nullptr` defensive branch.

#include "score/flatbuffers/buffer_version_info.hpp"
#include "score/flatbuffers/common/buffer_version_envelope_generated.h"
#include "score/flatbuffers/error.hpp"

#include "flatbuffers/base.h"

#include <score/span.hpp>

#include <cstdint>
#include <cstring>

namespace score
{
namespace flatbuffers
{
namespace detail
{

/// @brief Core span-based implementation of GetVersion.
///
/// The @p GetEnvelope template parameter is the injection point for unit tests.
template <auto GetEnvelope = ::score::flatbuffers::GetBufferVersionEnvelope>
inline score::Result<BufferVersionInfo> GetVersionImpl(const score::cpp::span<const uint8_t> buf) noexcept
{
    if (buf.data() == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNullDataPointer);
    }

    // Structural integrity check using the generated Verifier for BufferVersionEnvelope.
    // This verifies root offset, vtable, and the nested version table in one call.
    ::flatbuffers::Verifier verifier(buf.data(), buf.size());
    if (!::score::flatbuffers::VerifyBufferVersionEnvelopeBuffer(verifier))
    {
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }

    const auto* envelope = GetEnvelope(buf.data());
    if (envelope == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNullDataPointer);
    }

    const auto* version = envelope->version_info();
    if (version == nullptr)
    {
        return MakeUnexpected(ErrorCode::kVersionInfoNotPresent);
    }

    // Reject buffers where version_info was placed in the wrong vtable slot (user fbs).
    // The FlatBuffers verifier is type-blind and would otherwise accept another
    // field's bytes as a valid (all-default) BufferVersion table.
    // type_marker presence and correct value ensure no accidental match with a similar
    // table-based field.
    const auto* marker = version->type_marker();
    // NOTE: The "BV" literal must be kept in sync with the type_marker value in
    // inject_buffer_version.py. Changing it here without updating that script
    // (or vice versa) will cause buffer version validation to fail at runtime.
    if ((marker == nullptr) || (marker->string_view() != "BV"))
    {
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }

    // Guard: verify that bytes [4..7] are all printable ASCII (>= 0x20).
    // At this point the structural check above has confirmed a valid BufferVersion in field 0,
    // but the buffer may still lack a file_identifier directive.
    // Because the binary format has no explicit "has identifier" flag, this check is best-effort
    // and cannot detect every case of a missing file_identifier.
    //
    // When no file_identifier is present, bytes [4..7] hold implementation-defined data.
    // For buffers produced by the official toolchain (flatc or generated code), two known
    // cases can bypass the guard:
    // - Root vtable header: for small root tables this typically contains control bytes (< 0x20)
    //   and is rejected. Once the field count exceeds 8214, all four header bytes can be >= 0x20
    //   and the guard is bypassed.
    // - Deduplicated root vtable soffset: when the root table reuses an earlier vtable, the
    //   soffset is negative and its bytes are all >= 0x20, so the guard is bypassed.
    //   Deduplication requires child tables whose vtable layout, data sizes, and alignment all
    //   match the root table.
    //
    // This guard is complementary to the structural integrity check above, which ensures that
    // version_info is present and valid.
    const auto* const raw_id = ::flatbuffers::GetBufferIdentifier(buf.data());
    for (std::size_t i = 0U; i < static_cast<std::size_t>(::flatbuffers::kFileIdentifierLength); ++i)
    {
        if (static_cast<unsigned char>(raw_id[i]) < static_cast<unsigned char>(' '))
        {
            return MakeUnexpected(ErrorCode::kVerificationFailed);
        }
    }

    // GetBufferIdentifier returns a non-null-terminated pointer into the buffer.
    // The identifier is always exactly 4 characters (kFileIdentifierLength).
    // Zero-initialize the array so id[kFileIdentifierLength] remains '\0' after memcpy.
    char id[::flatbuffers::kFileIdentifierLength + 1U]{};
    std::memcpy(id, raw_id, ::flatbuffers::kFileIdentifierLength);

    return BufferVersionInfo{id, version->major_version(), version->minor_version()};
}

}  // namespace detail
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_VERSION_READER_IMPL_HPP
