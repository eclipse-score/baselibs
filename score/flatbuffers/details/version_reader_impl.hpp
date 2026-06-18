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
#include <string_view>

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
    if ((marker == nullptr) || (std::string_view{marker->c_str()} != "BV"))
    {
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }

    // GetBufferIdentifier returns a non-null-terminated pointer into the buffer.
    // The identifier is always exactly 4 characters (kFileIdentifierLength).
    // Zero-initialize the array so id[kFileIdentifierLength] remains '\0' after memcpy.
    char id[::flatbuffers::kFileIdentifierLength + 1U]{};
    std::memcpy(id, ::flatbuffers::GetBufferIdentifier(buf.data()), ::flatbuffers::kFileIdentifierLength);

    return BufferVersionInfo{id, version->major_version(), version->minor_version()};
}

}  // namespace detail
}  // namespace flatbuffers
}  // namespace score

#endif  // SCORE_LIB_FLATBUFFERS_VERSION_READER_IMPL_HPP
