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
#include "score/flatbuffers/common/buffer_version_envelope_generated.h"
#include "score/flatbuffers/details/version_reader_impl.hpp"

#include "score/flatbuffers/buffer_version_info.hpp"
#include "score/flatbuffers/error.hpp"
#include "score/flatbuffers/i_version_reader.hpp"

#include <score/span.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace score
{

namespace flatbuffers
{

namespace unit_test
{

// ─── Buffer builder helpers ────────────────────────────────────────────────────
//
// These helpers use the generated FlatBuffers builder API to construct in-memory
// buffers.

/// Build a structurally valid versioned buffer with the given 4-char @p identifier,
/// @p major and @p minor version.
static std::vector<uint8_t> BuildValidBuffer(const char* identifier, uint16_t major, uint16_t minor)
{
    ::flatbuffers::FlatBufferBuilder fbb;
    const auto type_marker = fbb.CreateString("BV");
    const auto version = score::flatbuffers::CreateBufferVersion(fbb, type_marker, major, minor);
    const auto envelope = score::flatbuffers::CreateBufferVersionEnvelope(fbb, version);
    fbb.Finish(envelope, identifier);
    const uint8_t* ptr = fbb.GetBufferPointer();
    return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
}

/// Build a structurally valid versioned buffer whose file-identifier bytes [4..7] are
/// zeroed after construction, producing a null-byte identifier (\0\0\0\0).
///
/// '\0' < ' ', so the printable-ASCII check in GetVersionImpl rejects it.
/// The FlatBuffers builder itself rejects null-byte identifiers via
/// strlen(identifier) == kFileIdentifierLength, so the null bytes must be injected
/// post-construction. Note: in a real schema-without-identifier buffer those bytes
/// contain implementation-defined data — not necessarily null — but the printable-
/// ASCII check covers that case too.
static std::vector<uint8_t> BuildBufferWithNullIdentifier(uint16_t major, uint16_t minor)
{
    // Use a valid placeholder identifier; the identifier bytes will be zeroed below.
    auto buf = BuildValidBuffer("XXXX", major, minor);
    // FlatBuffer layout: bytes [0..3] = root offset, bytes [4..7] = file identifier.
    constexpr std::size_t kIdentifierOffset = sizeof(::flatbuffers::uoffset_t);
    std::fill(buf.begin() + static_cast<std::ptrdiff_t>(kIdentifierOffset),
              buf.begin() + static_cast<std::ptrdiff_t>(kIdentifierOffset + ::flatbuffers::kFileIdentifierLength),
              uint8_t{0});
    return buf;
}

/// Build an envelope buffer that carries NO version_info field.
/// The FlatBuffers verifier accepts it (version_info is optional in the schema).
static std::vector<uint8_t> BuildBufferWithoutVersionInfo()
{
    ::flatbuffers::FlatBufferBuilder fbb;
    const auto envelope = score::flatbuffers::CreateBufferVersionEnvelope(fbb);
    fbb.Finish(envelope);
    const uint8_t* ptr = fbb.GetBufferPointer();
    return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
}

/// Build a buffer where version_info.type_marker is absent (null offset).
static std::vector<uint8_t> BuildBufferWithNullMarker()
{
    ::flatbuffers::FlatBufferBuilder fbb;
    // CreateBufferVersion without a type_marker: pass 0 as the offset.
    score::flatbuffers::BufferVersionBuilder bvb(fbb);
    bvb.add_major_version(1U);
    bvb.add_minor_version(0U);
    const auto version = bvb.Finish();
    const auto envelope = score::flatbuffers::CreateBufferVersionEnvelope(fbb, version);
    fbb.Finish(envelope);
    const uint8_t* ptr = fbb.GetBufferPointer();
    return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
}

/// Build a buffer whose version_info.type_marker is NOT the sentinel "BV".
static std::vector<uint8_t> BuildBufferWithWrongMarker(const char* wrong_marker)
{
    ::flatbuffers::FlatBufferBuilder fbb;
    const auto marker = fbb.CreateString(wrong_marker);
    const auto version = score::flatbuffers::CreateBufferVersion(fbb, marker, 1U, 0U);
    const auto envelope = score::flatbuffers::CreateBufferVersionEnvelope(fbb, version);
    fbb.Finish(envelope);
    const uint8_t* ptr = fbb.GetBufferPointer();
    return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
}

// ─── Fixture ──────────────────────────────────────────────────────────────────

class VersionReaderTest : public ::testing::Test
{
  protected:
    std::unique_ptr<score::flatbuffers::IVersionReader> reader_ = std::make_unique<score::flatbuffers::VersionReader>();
    score::flatbuffers::IVersionReader& version_reader_{*reader_};
};

// ─── GetVersion – failure cases ───────────────────────────────────────────────

TEST_F(VersionReaderTest, GetVersionVectorRejectsEmptyVector)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned for an empty std::vector without invoking the span overload");

    const auto result_vec = version_reader_.GetVersion(std::vector<uint8_t>{});
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(std::vector<uint8_t>{});
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionSpanRejectsNullPointer)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kNullDataPointer is returned when span.data() is null");
    const score::cpp::span<const uint8_t> null_span{nullptr, 64U};

    const auto result_span = version_reader_.GetVersion(null_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));

    const auto result_free_span = score::flatbuffers::GetVersion(null_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));
}

TEST_F(VersionReaderTest, GetVersionRejectsGarbageBuffer)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned for an all-zeros buffer that fails FlatBuffers verification");

    const std::vector<uint8_t> garbage(64U, 0U);
    const score::cpp::span<const uint8_t> garbage_span{garbage.data(), garbage.size()};

    const auto result_vec = version_reader_.GetVersion(garbage);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(garbage_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(garbage);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(garbage_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsTooSmallBuffer)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "kVerificationFailed is returned for a buffer that is too small to hold a valid FlatBuffer root offset");

    const std::vector<uint8_t> tiny = {0x00, 0x00, 0x00, 0x00};
    const score::cpp::span<const uint8_t> tiny_span{tiny.data(), tiny.size()};

    const auto result_vec = version_reader_.GetVersion(tiny);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(tiny_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(tiny);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(tiny_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsBufferWithoutVersionInfo)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "kVersionInfoNotPresent is returned when the envelope passes verification but version_info field is absent");

    const auto buf = BuildBufferWithoutVersionInfo();
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result_vec = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionInfoNotPresent));

    const auto result_span = version_reader_.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionInfoNotPresent));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionInfoNotPresent));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionInfoNotPresent));
}

TEST_F(VersionReaderTest, GetVersionRejectsWrongTypeMarker)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned when type_marker is present but not the sentinel value 'BV'");

    const auto buf = BuildBufferWithWrongMarker("XX");
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result_vec = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsNullTypeMarker)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "kVerificationFailed is returned when type_marker is absent (null offset) in the version_info table");

    const auto buf = BuildBufferWithNullMarker();
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result_vec = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsEmptyTypeMarker)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned when type_marker is an empty string instead of 'BV'");

    const auto buf = BuildBufferWithWrongMarker("");
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result_vec = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// Injection helper: simulates a hypothetical future regression where
// GetBufferVersionEnvelope returns null for a verified buffer.
// Covers the defensive `envelope == nullptr` branch.
static const score::flatbuffers::BufferVersionEnvelope* ReturnNullEnvelope(const void* /*buf*/) noexcept
{
    return nullptr;
}

TEST_F(VersionReaderTest, GetVersionReturnsNullDataPointerWhenEnvelopeIsNull)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kNullDataPointer is returned when the envelope getter is injected to return nullptr, "
                   "covering the defensive branch that is otherwise structurally unreachable");

    // Any valid buffer will do: we only need it to pass the verifier so the injected getter is reached.
    const auto buf = BuildValidBuffer("TEST", 1U, 0U);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result = score::flatbuffers::detail::GetVersionImpl<ReturnNullEnvelope>(buf_span);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));
}

TEST_F(VersionReaderTest, GetVersionRejectsBufferWithNullIdentifier)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned when the 4 identifier bytes at buffer positions [4..7] are all "
                   "zero, as produced by a schema without a file_identifier directive");

    const auto buf = BuildBufferWithNullIdentifier(1U, 0U);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};

    const auto result_vec = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

struct VersionBufferParam
{
    char identifier[5];
    uint16_t major;
    uint16_t minor;
    const char* name;
};

class InvalidIdentifierTest : public ::testing::TestWithParam<VersionBufferParam>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    InvalidIdentifier,
    InvalidIdentifierTest,
    ::testing::Values(
        // Invalid identifier byte boundaries: \x01 (minimum non-null byte) and \x1F (maximum byte).
        // GetVersion preserves all byte values verbatim; the 4-byte string_view returned by
        // identifier() is compared by value, not by printability.
        // VerifyVersion is NOT expected to be called with non-printable expected identifiers
        // in production — the parameterised mismatch tests cover rejection of such buffers.
        VersionBufferParam{"\x01\x01\x01\x01", 0U,         0U,              "MinInvalidByteIdentifier"},
        VersionBufferParam{"\x1F\x1F\x1F\x1F", UINT16_MAX, UINT16_MAX,      "MaxInvalidByteIdentifier"}
    ),
    [](const ::testing::TestParamInfo<VersionBufferParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(InvalidIdentifierTest, GetVersionRejectsBufferWithInvalidIdentifier)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "GetVersion rejects a buffer whose identifier bytes are all non-printable ASCII (< 0x20)");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.GetVersion(buf);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = reader.GetVersion(buf_span);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// ─── Null-byte identifier – library constraint documentation ─────────────────
//
// FlatBuffers uses strlen() to validate the file_identifier before writing it
// into the buffer (see FlatBufferBuilderImpl::Finish).  A \x00 byte in any
// position makes strlen() return a value shorter than 4, causing an assertion
// failure (abort) inside the library.  It is therefore impossible to produce a
// versioned buffer with a \x00 byte in the identifier.
//
// The death test below documents this hard constraint: the buffer cannot be
// constructed, so GetVersion / VerifyVersion can never observe a \x00 identifier.
TEST(VersionBufferNullByteIdentifierTest, BuildingBufferWithNullByteIdentifierAborts)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "BuildValidBuffer aborts inside FlatBuffers when the identifier contains a \\x00 byte");

    // The assertion message from FlatBuffers:
    //   strlen(file_identifier) == kFileIdentifierLength
    ASSERT_DEATH(BuildValidBuffer("\x00\x00\x00\x00", 0U, 0U), "");
}

// ─── GetVersion – success / boundary values ───────────────────────────────────

class VersionBufferParamTest : public ::testing::TestWithParam<VersionBufferParam>
{
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    BoundaryValues,
    VersionBufferParamTest,
    ::testing::Values(
        // Identifier byte boundaries: \x20 (minimum non-null byte) and \xFF (maximum byte).
        // Sanity check ensures that bytes [4..7] are printable ASCII characters (>= \x20).
        // GetVersion preserves all byte values verbatim; the 4-byte string_view returned by
        // identifier() is compared by value.
        // VerifyVersion is NOT expected to be called with non-printable expected identifiers
        // in production — the parameterised mismatch tests cover rejection of such buffers.
        VersionBufferParam{"    ", 0U,          0U,                   "MinMinVersion"},
        VersionBufferParam{"    ", 0U,          1U,                   "MinMajorMinorOne"},
        VersionBufferParam{"    ", 1U,          0U,                   "MajorOneMinMinor"},
        VersionBufferParam{"DEMO", 2U,          3U,                   "TypicalValues"},
        VersionBufferParam{"~~~~", UINT16_MAX,  UINT16_MAX - 1U,      "MaxMajorNearMaxMinor"},
        VersionBufferParam{"~~~~", UINT16_MAX,  UINT16_MAX,           "MaxMaxVersion"},
        VersionBufferParam{"\xFF\xFF\xFF\xFF", UINT16_MAX, UINT16_MAX,      "MaxByteIdentifier"}
    ),
    [](const ::testing::TestParamInfo<VersionBufferParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(VersionBufferParamTest, ReadsIdentifierAndVersionCorrectly)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "GetVersion correctly extracts identifier, major and minor from a built buffer");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.GetVersion(buf);
    ASSERT_TRUE(result_vec.has_value());
    EXPECT_EQ(result_vec.value().identifier(), std::string_view(p.identifier, 4U));
    EXPECT_EQ(result_vec.value().major_version, p.major);
    EXPECT_EQ(result_vec.value().minor_version, p.minor);

    const auto result_span = reader.GetVersion(buf_span);
    ASSERT_TRUE(result_span.has_value());
    EXPECT_EQ(result_span.value().identifier(), std::string_view(p.identifier, 4U));
    EXPECT_EQ(result_span.value().major_version, p.major);
    EXPECT_EQ(result_span.value().minor_version, p.minor);

    const auto result_free_vec = score::flatbuffers::GetVersion(buf);
    ASSERT_TRUE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.value().identifier(), std::string_view(p.identifier, 4U));
    EXPECT_EQ(result_free_vec.value().major_version, p.major);
    EXPECT_EQ(result_free_vec.value().minor_version, p.minor);

    const auto result_free_span = score::flatbuffers::GetVersion(buf_span);
    ASSERT_TRUE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.value().identifier(), std::string_view(p.identifier, 4U));
    EXPECT_EQ(result_free_span.value().major_version, p.major);
    EXPECT_EQ(result_free_span.value().minor_version, p.minor);
}

// ─── VerifyVersion – invalid version ──────────────────────────────────────────

TEST_F(VersionReaderTest, VerifyVersionVectorRejectsEmptyVector)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty(
        "Description",
        "VerifyVersion(vector) returns kVerificationFailed for an empty vector without calling the span overload");

    const score::flatbuffers::BufferVersionInfo expected{"TEST", 1U, 0U};

    const auto result_vec = version_reader_.VerifyVersion(std::vector<uint8_t>{}, expected);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::VerifyVersion(std::vector<uint8_t>{}, expected);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, VerifyVersionPropagatesGetVersionError)
{
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion propagates the error returned by GetVersion when the buffer is invalid");

    const std::vector<uint8_t> garbage(64U, 0U);
    const score::cpp::span<const uint8_t> garbage_span{garbage.data(), garbage.size()};
    const score::flatbuffers::BufferVersionInfo expected{"TEST", 1U, 0U};

    const auto result_vec = version_reader_.VerifyVersion(garbage, expected);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_span = version_reader_.VerifyVersion(garbage_span, expected);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_vec = score::flatbuffers::VerifyVersion(garbage, expected);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result_free_span = score::flatbuffers::VerifyVersion(garbage_span, expected);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// ─── VerifyVersion ────────────────────────────────────────────────────────────
// All parameterised VerifyVersion tests reuse VersionBufferParam / BoundaryValues.

TEST_P(VersionBufferParamTest, VerifyVersionExactMatchSucceeds)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kExact) returns success when identifier, major and minor all match exactly");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, p.major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected);
    EXPECT_TRUE(result_vec.has_value());

    const auto result_span = reader.VerifyVersion(buf_span, expected);
    EXPECT_TRUE(result_span.has_value());

    const auto result_free_vec = score::flatbuffers::VerifyVersion(buf, expected);
    EXPECT_TRUE(result_free_vec.has_value());

    const auto result_free_span = score::flatbuffers::VerifyVersion(buf_span, expected);
    EXPECT_TRUE(result_free_span.has_value());
}

TEST_P(VersionBufferParamTest, VerifyVersionFailsOnMajorMismatch)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kExact) returns kVersionMismatch when expected major differs from actual");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // Guaranteed ≠ p.major: use 42 unless p.major is already 42, then use 21.
    const uint16_t wrong_major = (p.major == 42U) ? 21U : 42U;
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, wrong_major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span = reader.VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec = score::flatbuffers::VerifyVersion(buf, expected);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span = score::flatbuffers::VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionBufferParamTest, VerifyVersionFailsOnMinorMismatch)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kExact) returns kVersionMismatch when expected minor differs from actual");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // Guaranteed ≠ p.minor: use 42 unless p.minor is already 42, then use 21.
    const uint16_t wrong_minor = (p.minor == 42U) ? 21U : 42U;
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, p.major, wrong_minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span = reader.VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec = score::flatbuffers::VerifyVersion(buf, expected);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span = score::flatbuffers::VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionBufferParamTest, VerifyVersionFailsOnIdentifierMismatch)
{
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kExact) returns kVersionMismatch when the 4-char file identifier does not match");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // "MISM" is not used by any param identifier, so it never matches p.identifier.
    const score::flatbuffers::BufferVersionInfo expected{"MISM", p.major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span = reader.VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec = score::flatbuffers::VerifyVersion(buf, expected);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span = score::flatbuffers::VerifyVersion(buf_span, expected);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionBufferParamTest, VerifyVersionMinorMinimumSucceedsWhenMinorEquals)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kMinorMinimum) succeeds when actual minor equals the expected minimum");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, p.major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_vec.has_value());

    const auto result_span =
        reader.VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_span.has_value());

    const auto result_free_vec =
        score::flatbuffers::VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_free_vec.has_value());

    const auto result_free_span =
        score::flatbuffers::VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_free_span.has_value());
}

TEST_P(VersionBufferParamTest, VerifyVersionMinorMinimumSucceedsWhenActualMinorIsHigher)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kMinorMinimum) succeeds when actual minor is strictly greater than expected minimum");

    const auto& p = GetParam();
    if (p.minor == 0U)
    {
        GTEST_SKIP() << "minor == 0: no value below it can serve as minimum";
    }
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // Expected minimum is one below actual – always strictly less than p.minor.
    const uint16_t min_minor = static_cast<uint16_t>(p.minor - 1U);
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, p.major, min_minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_vec.has_value());

    const auto result_span =
        reader.VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_span.has_value());

    const auto result_free_vec =
        score::flatbuffers::VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_free_vec.has_value());

    const auto result_free_span =
        score::flatbuffers::VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result_free_span.has_value());
}

TEST_P(VersionBufferParamTest, VerifyVersionMinorMinimumFailsWhenActualMinorIsLower)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty(
        "Description",
        "VerifyVersion(kMinorMinimum) returns kVersionMismatch when actual minor is below the required minimum");

    const auto& p = GetParam();
    if (p.minor == UINT16_MAX)
    {
        GTEST_SKIP() << "minor == UINT16_MAX: no value above it can serve as minimum";
    }
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // Expected minimum is one above actual – always strictly greater than p.minor.
    const uint16_t min_minor = static_cast<uint16_t>(p.minor + 1U);
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, p.major, min_minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span =
        reader.VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec =
        score::flatbuffers::VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span =
        score::flatbuffers::VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionBufferParamTest, VerifyVersionMinorMinimumFailsOnIdentifierMismatch)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty(
        "Description",
        "VerifyVersion(kMinorMinimum) returns kVersionMismatch when the 4-char file identifier does not match, "
        "regardless of whether major and minor satisfy the minimum");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // "MISM" is not used by any param identifier, so it never matches p.identifier.
    const score::flatbuffers::BufferVersionInfo expected{"MISM", p.major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span =
        reader.VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec =
        score::flatbuffers::VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span =
        score::flatbuffers::VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionBufferParamTest, VerifyVersionMinorMinimumFailsOnMajorMismatch)
{
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description",
                   "VerifyVersion(kMinorMinimum) returns kVersionMismatch when the major version does not match, "
                   "even though the identifier matches and minor satisfies the minimum");

    const auto& p = GetParam();
    const auto buf = BuildValidBuffer(p.identifier, p.major, p.minor);
    const score::cpp::span<const uint8_t> buf_span{buf.data(), buf.size()};
    // Guaranteed ≠ p.major: use 42 unless p.major is already 42, then use 21.
    const uint16_t wrong_major = (p.major == 42U) ? 21U : 42U;
    const score::flatbuffers::BufferVersionInfo expected{p.identifier, wrong_major, p.minor};
    score::flatbuffers::VersionReader reader;

    const auto result_vec = reader.VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_vec.has_value());
    EXPECT_EQ(result_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_span =
        reader.VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_span.has_value());
    EXPECT_EQ(result_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_vec =
        score::flatbuffers::VerifyVersion(buf, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_vec.has_value());
    EXPECT_EQ(result_free_vec.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result_free_span =
        score::flatbuffers::VerifyVersion(buf_span, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result_free_span.has_value());
    EXPECT_EQ(result_free_span.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score
