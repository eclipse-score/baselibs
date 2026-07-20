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
#include "score/filesystem/path.h"
#include "score/flatbuffers/buffer_version_info.hpp"
#include "score/flatbuffers/common/buffer_version_envelope_generated.h"
#include "score/flatbuffers/i_version_reader.hpp"
#include "score/flatbuffers/load_buffer.hpp"
#include "score/flatbuffers/test/testdata/test_versioned_dedup_node_generated.h"

#include <flatbuffers/flatbuffer_builder.h>
#include <score/span.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

/// @brief Print the vtable of a FlatBuffer table.
/// @param name Name of the table (for logging).
/// @param buf_start Pointer to the start of the buffer (for calculating offsets).
/// @param table Pointer to the table whose vtable is to be printed.
/// @param print_fields Whether to print the individual field offsets (default: true).
void PrintVTable(const std::string_view name,
                 const uint8_t* buf_start,
                 const uint8_t* table,
                 const bool print_fields = true)
{
    const auto soffset = ::flatbuffers::ReadScalar<int32_t>(table);
    const uint8_t* vtable = table - soffset;  // vtable = table_ptr - soffset

    // vtable layout: [vtable_size][table_object_size][field_loc0][field_loc1]...
    const auto vtable_size = ::flatbuffers::ReadScalar<uint16_t>(vtable);
    const auto table_obj_size = ::flatbuffers::ReadScalar<uint16_t>(vtable + 2U);
    std::printf("  %-6s @+%02td: soffset=%-5d  vtable@+%02td {vtable_size=%u, table_obj_size=%u",
                name.data(),
                table - buf_start,
                soffset,
                vtable - buf_start,
                vtable_size,
                table_obj_size);
    if (print_fields)
    {
        for (uint16_t vtable_field_offset = 4U; vtable_field_offset < vtable_size; vtable_field_offset += 2U)
        {
            std::printf(", field_loc[%u]=%u",
                        (vtable_field_offset - 4U) / 2U,
                        ::flatbuffers::ReadScalar<uint16_t>(vtable + vtable_field_offset));
        }
    }
    else
    {
        std::printf(", ...");
    }
    std::printf("}\n");
}

/// Integration test for `VersionReader` / `IVersionReader`.
///
///   - The test fixture owns a `score::flatbuffers::VersionReader` (concrete
///     implementation) as a member and exposes it via the abstract
///     `IVersionReader&` reference.
///   - Each test calls the `IVersionReader` interface and the free function.
///   - The overloads with the vector parameter (buf) are tested since all public
///     APIs are forwarding to the span-based implementation.
///
class VersionReaderTest : public ::testing::Test
{

  protected:
    std::vector<uint8_t> data_;

    /// Concrete implementation – owned by the fixture, lifetime matches the test.
    score::flatbuffers::VersionReader reader_;

    /// All tests access functionality through the abstract interface,
    /// mirroring how production code should depend on IVersionReader.
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

// clang-format off
/// Equivalence classes for `GetVersion`
///
/// | EC# | Input                                    | Pointer | Buffer content / size          | Expected result              | Test case(s)                                       |
/// |-----|------------------------------------------|---------|--------------------------------|------------------------------|----------------------------------------------------|
/// | EC1 | Null data pointer                        | null    | size = 64                      | Err: kNullDataPointer        | GetVersionRejectsNullPointer                       |
/// | EC2 | Empty buffer                             | valid   | size = 0                       | Err: kVerificationFailed     | GetVersionRejectsEmptyBuffer                       |
/// | EC3 | Buffer too small to hold version info    | valid   | size = 4 (4 zero bytes)        | Err: kVerificationFailed     | GetVersionRejectsTooSmallBuffer                    |
/// | EC4 | Buffer with invalid FlatBuffer content   | valid   | 64 zero bytes                  | Err: kVerificationFailed     | GetVersionRejectsInvalidBuffer                     |
/// | EC5 | version_info not in field 0 of root table| valid   | valid FlatBuffer, wrong field  | Err: kVerificationFailed     | GetVersionRejectsBufferWithVersionInfoInWrongField |
/// | EC6 | Valid buffer with version info           | valid   | valid FlatBuffer               | Ok: BufferVersionInfo        | ReadBufferVersionFromBinary                        |
/// | EC7 | Normal schema without file_identifier    | valid   | valid FlatBuffer               | Err: kVerificationFailed     | GetVersionRejectsBufferWithMissingIdentifier       |
/// | EC8 | No file_identifier wide schema           | valid   | valid FlatBuffer               | Ok: BufferVersionInfo        | MissingIdentifierNotDetectedWideSchema             |
/// | EC9 | No file_identifier dedup vtable          | valid   | valid FlatBuffer               | Ok: BufferVersionInfo        | MissingIdentifierNotDetectedDeduplicatedRootVtable |
// clang-format on

TEST_F(VersionReaderTest, GetVersionRejectsNullPointer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kNullDataPointer is returned if null pointer is passed instead of buffer data");

    const auto result = version_reader_.GetVersion(score::cpp::span<const uint8_t>{nullptr, 64U});
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));

    const auto result2 = score::flatbuffers::GetVersion(score::cpp::span<const uint8_t>{nullptr, 64U});
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));
}

TEST_F(VersionReaderTest, GetVersionRejectsEmptyBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer is empty");

    const auto result = version_reader_.GetVersion(std::vector<uint8_t>{});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(std::vector<uint8_t>{});
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsTooSmallBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer is too small to contain version info");

    const std::vector<uint8_t> tiny_buf = {0, 0, 0, 0};
    const auto result = version_reader_.GetVersion(tiny_buf);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(tiny_buf);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsInvalidBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer does not contain valid version info");

    const std::vector<uint8_t> bad_buf(64, 0);
    const auto result = version_reader_.GetVersion(bad_buf);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(bad_buf);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsBufferWithMissingIdentifier)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned when the schema has no file_identifier directive "
                   "(bytes [4,8) contain implementation-defined data, not a valid identifier)");

    auto load_result = score::flatbuffers::LoadBuffer(
        score::filesystem::Path{"score/flatbuffers/test/testdata/versioned_no_identifier_buffer.bin"});
    ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed for versioned_no_identifier_buffer.bin";
    const auto& buf = load_result.value();

    // This reaction is just a demonstration it cannot be guaranteed for all schemas as it depends
    // on the implementation-defined bytes in the buffer.
    const auto result = version_reader_.GetVersion(buf);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(buf);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// clang-format off
// EC8: The printable-ASCII guard rejects buffers without a file_identifier by
// checking that bytes [4..7] are all >= 0x20.  
// Without a file_identifier those bytes typically hold the root vtable header 
// (vtable_size, table_obj_size) — small integers that almost always contain
// control bytes, so the guard reliably catches practical schemas.
//
// The guard fails once the root table is wide enough: vtable_size increases by 2
// per declared field.  At 8214 fields the header bytes are all >= 0x20 and the
// buffer passes the check with garbage identifier bytes.  The FlatBuffers verifier
// accepts such a buffer because extra fields beyond version_info are
// schema-forward-compatible.
//
// vtable_size    = 4 + 8214×2 = 16432 = 0x4030 => bytes [4..5] = 0x30 0x40
// table_obj_size = 8224       = 0x2020         => bytes [6..7] = 0x20 0x20
//
// As such wide root tables are unusual in practice, the guards limitations are acceptable to fail
// as the goal is to detect a missing file_identifier in the common case.
// clang-format on
TEST_F(VersionReaderTest, MissingIdentifierNotDetectedWideSchema)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "GetVersion succeeds (readability guard bypassed) for a buffer "
                   "whose root table has >= 8214 declared fields and no file_identifier. "
                   "The vtable header bytes [4..7] all can be >= 0x20 and therefore bypass the guard. "
                   "Accepted guard limitation.");

    constexpr ::flatbuffers::voffset_t num_fields = 8214;

    ::flatbuffers::FlatBufferBuilder fbb;
    const auto type_marker = fbb.CreateString("BV");
    const auto version = score::flatbuffers::CreateBufferVersion(fbb, type_marker, 1U, 0U);

    const ::flatbuffers::uoffset_t root_start = fbb.StartTable();
    fbb.AddOffset(::flatbuffers::FieldIndexToOffset(0), version);
    for (::flatbuffers::voffset_t f = 1; f < num_fields; ++f)
    {
        fbb.AddElement<uint8_t>(::flatbuffers::FieldIndexToOffset(f), 0xFFU, 0U);
    }
    const auto root = fbb.EndTable(root_start);
    fbb.Finish(::flatbuffers::Offset<::flatbuffers::Table>(root));  // NO file_identifier

    const uint8_t* ptr = fbb.GetBufferPointer();
    const std::vector<uint8_t> buf(ptr, ptr + fbb.GetSize());

    // --- raw buffer dump (first 10 bytes) ---
    std::printf("Buffer (%zu B):", buf.size());
    for (size_t i = 0; i < 10U; ++i)
    {
        std::printf(" %02x", buf[i]);
    }
    std::printf(" ...\n");

    // --- raw vtable dump ---
    std::printf("table entry + vtable (without fields):\n");
    const auto root_table_offset = ::flatbuffers::ReadScalar<::flatbuffers::uoffset_t>(ptr);
    const auto* const root_table_ptr = reinterpret_cast<const uint8_t*>(ptr + root_table_offset);
    PrintVTable("root", ptr, root_table_ptr, false);

    const auto result = version_reader_.GetVersion(buf);
    ASSERT_TRUE(result.has_value()) << "GetVersion must succeed (guard bypassed by wide-schema buffer)";
    // The 'identifier' is the raw vtable header bytes "0@  " (vtable_size=0x4030, table_obj_size=0x2020).
    EXPECT_EQ(result.value().identifier(), std::string("0@  "));
    // The actual version data is correctly extracted from the nested BufferVersion table.
    EXPECT_EQ(result.value().major_version, 1U);
    EXPECT_EQ(result.value().minor_version, 0U);

    const auto result2 = score::flatbuffers::GetVersion(buf);
    ASSERT_TRUE(result2.has_value()) << "free-function GetVersion must succeed (guard bypassed by wide-schema buffer)";
    // The 'identifier' is the raw vtable header bytes "0@  " (vtable_size=0x4030, table_obj_size=0x2020).
    EXPECT_EQ(result2.value().identifier(), std::string("0@  "));
    // The actual version data is correctly extracted from the nested BufferVersion table.
    EXPECT_EQ(result2.value().major_version, 1U);
    EXPECT_EQ(result2.value().minor_version, 0U);
}

// clang-format off
// EC9: The printable-ASCII guard rejects buffers without a file_identifier by
// checking that bytes [4..7] are all >= 0x20.
// Without a file_identifier those bytes typically hold the root vtable header
// (vtable_size, table_obj_size) - see EC8.
// In case of a recursive root table, the root vtable can be deduplicated against 
// a child vtable. In this case bytes [4..7] contain the negative soffset to the 
// child vtable, which is almost always < 0x20.
//
// As such deduplicated root vtables are unusual in practice, the guards limitations are acceptable to fail
// as the goal is to detect a missing file_identifier in the common case.
// clang-format on
TEST_F(VersionReaderTest, MissingIdentifierNotDetectedDeduplicatedRootVtable)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "GetVersion succeeds (readability guard bypassed) for a buffer "
                   "whose root table has a deduplicated vtable and no file_identifier. "
                   "The table soffset bytes [4..7] are negative and therefore bypass the guard. "
                   "Accepted guard limitation.");

    auto load_result = score::flatbuffers::LoadBuffer(
        score::filesystem::Path{"score/flatbuffers/test/testdata/versioned_dedup_node_buffer.bin"});
    ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed for versioned_dedup_node_buffer.bin";
    const std::vector<uint8_t>& buf = load_result.value();

    // --- raw buffer dump ---
    std::printf("Buffer (%zu B):", buf.size());
    for (auto& buf_byte : buf)
    {
        std::printf(" %02x", buf_byte);
    }
    std::printf("\n");

    // --- raw vtable dump ---
    const auto* const root_node = my_component::dedup_node::GetNode(buf.data());
    ASSERT_NE(root_node, nullptr) << "GetNode must succeed: buffer is valid FlatBuffer";
    std::printf("table entry + vtable overview:\n");
    PrintVTable("root", buf.data(), reinterpret_cast<const uint8_t*>(root_node));

    const auto* const child1_node = root_node->child();
    ASSERT_NE(child1_node, nullptr) << "root must have a child node";
    PrintVTable("child1", buf.data(), reinterpret_cast<const uint8_t*>(child1_node));

    const auto* const child2_node = child1_node->child();
    ASSERT_NE(child2_node, nullptr) << "child1 must have a child node";
    PrintVTable("child2", buf.data(), reinterpret_cast<const uint8_t*>(child2_node));

    // Report whether deduplication of root vtable occured.
    const auto root_soffset = ::flatbuffers::ReadScalar<int32_t>(reinterpret_cast<const uint8_t*>(root_node));
    ASSERT_LT(root_soffset, 0) << "dedup: root reuses an earlier vtable -> negative soffset";

    const auto result = version_reader_.GetVersion(buf);
    ASSERT_TRUE(result.has_value()) << "GetVersion must succeed (guard bypassed by vtable-dedup soffset)";
    EXPECT_EQ(result.value().major_version, 1U);
    EXPECT_EQ(result.value().minor_version, 0U);
    EXPECT_EQ(result.value().identifier(), std::string("\xE0\xFF\xFF\xFF"))
        << "dedup: root table soffset bytes [4..7] are treated as identifier";

    const auto result2 = score::flatbuffers::GetVersion(buf);
    ASSERT_TRUE(result2.has_value())
        << "free-function GetVersion must succeed (guard bypassed by vtable-dedup soffset)";
    EXPECT_EQ(result2.value().major_version, 1U);
    EXPECT_EQ(result2.value().minor_version, 0U);
    EXPECT_EQ(result2.value().identifier(), std::string("\xE0\xFF\xFF\xFF"))
        << "dedup: root table soffset bytes [4..7] are treated as identifier";
}

// ---------------------------------------------------------------------------
// Parameterized fixture for wrong-field placement buffers:
// ---------------------------------------------------------------------------
struct WrongFieldParam
{
    std::string bin_path;
    std::string name;  // used as the GoogleTest instance suffix
};

class VersionReaderWrongFieldTest : public ::testing::TestWithParam<WrongFieldParam>
{
  protected:
    score::flatbuffers::VersionReader reader_;
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    WrongFieldBuffers,
    VersionReaderWrongFieldTest,
    ::testing::Values(
        WrongFieldParam{
            "score/flatbuffers/test/testdata/versioned_wrong_field.bin",
            "WrongField"
        },
        WrongFieldParam{
            "score/flatbuffers/test/testdata/versioned_wrong_field_similar_type.bin",
            "WrongFieldSimilarType"
        }
    ),
    [](const ::testing::TestParamInfo<WrongFieldParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(VersionReaderWrongFieldTest, GetVersionRejectsBufferWithVersionInfoInWrongField)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned if version_info is not in field 0 of the root table");

    const auto& param = GetParam();
    auto load_result = score::flatbuffers::LoadBuffer(score::filesystem::Path{param.bin_path});
    ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed for " << param.bin_path;
    const auto& wrong_field_buf = load_result.value();

    const auto result = version_reader_.GetVersion(wrong_field_buf);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(wrong_field_buf);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// ---------------------------------------------------------------------------
// Parameterized fixture for version verification
// ---------------------------------------------------------------------------
struct VersionBufferParam
{
    std::string bin_path;
    score::flatbuffers::BufferVersionInfo version_info;
    std::string name;  // used as the GoogleTest instance suffix
};

class VersionReaderBufferParamTest : public ::testing::TestWithParam<VersionBufferParam>
{
  protected:
    void SetUp() override
    {
        const auto& param = GetParam();
        auto load_result = score::flatbuffers::LoadBuffer(score::filesystem::Path{param.bin_path});
        ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed: " << param.bin_path;
        data_ = std::move(load_result).value();
    }

    std::vector<uint8_t> data_;
    score::flatbuffers::VersionReader reader_;
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

// clang-format off
/// Boundary values for `GetVersion` (via `ReadBufferVersionFromBinary`)
///
/// Fields under test: identifier (4-char ASCII), major_version (uint16), minor_version (uint16).
///
/// | BV# | Instance name     | identifier   | major_version  | minor_version  | Boundary                                          |
/// |-----|-------------------|--------------|----------------|----------------|---------------------------------------------------|
/// | BV1 | Normal            | "DEMO"       | 2              | 3              | Representative valid mid-range value              |
/// | BV2 | NormalExplicitIds | "DEMO"       | 2              | 3              | Same as BV1 but with explicit FlatBuffer field IDs|
/// | BV3 | MinVersion        | "    " (0x20)| 0              | 0              | All fields at minimum and printable-ASCII boundary|
/// | BV4 | MinRangeVersion   | " !\"#"      | 0              | 1              | Identifier at lower printable-ASCII boundary      |
/// | BV5 | MaxRangeVersion   | "{|}~"       | UINT16_MAX     | 65534          | Identifier at upper printable-ASCII boundary      |
/// | BV6 | MaxVersion        | "~~~~"(0x7E) | UINT16_MAX     | UINT16_MAX     | All fields at maximum and printable-ASCII boundary|
///
/// Expected result for all instances: version info is correctly read from buffer and matches the given values.
///
INSTANTIATE_TEST_SUITE_P(
    AllVersionBuffers,
    VersionReaderBufferParamTest,
    ::testing::Values(
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_buffer.bin",              {"DEMO",  2U,     3U},     "Normal"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_buffer_explicit_ids.bin", {"DEMO",  2U,     3U},     "NormalExplicitIds"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_min_buffer.bin",          {"    ",  0U,     0U},     "MinVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_min_range_buffer.bin",    {" !\"#", 0U,     1U},     "MinRangeVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_max_range_buffer.bin",    {"{|}~",  UINT16_MAX, 65534U}, "MaxRangeVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_max_buffer.bin",          {"~~~~",  UINT16_MAX, UINT16_MAX}, "MaxVersion"}
    ),
    [](const ::testing::TestParamInfo<VersionBufferParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(VersionReaderBufferParamTest, ReadBufferVersionFromBinary)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "version info is correctly read from buffer");

    const auto& param = GetParam();
    const auto result = version_reader_.GetVersion(data_);

    ASSERT_TRUE(result.has_value()) << "GetVersion failed";

    const auto& info = result.value();

    EXPECT_EQ(info.identifier(), param.version_info.identifier());
    EXPECT_EQ(info.major_version, param.version_info.major_version);
    EXPECT_EQ(info.minor_version, param.version_info.minor_version);
}

// clang-format off
/// Equivalence classes for `VerifyVersion`
///
/// Fields under test: identifier, major_version, minor_version, VersionMatchMode.
///
/// | EC#  | Match mode    | identifier              | major_version      | minor_version                            | Expected result       | Test case(s)                                                      |
/// |------|---------------|-------------------------|--------------------|------------------------------------------|-----------------------|-------------------------------------------------------------------|
/// | EC1  | kExactMatch   | Matching                | Matching           | Matching                                 | Ok                    | VerifyVersionSucceedsOnMatch (BV1–BV6)                            |
/// | EC2  | kExactMatch   | Non-matching ("XXXX")   | Matching           | Matching                                 | Err: kVersionMismatch | VerifyVersionFailsOnIdentifierMismatch (BV1–BV6)                  |
/// | EC3  | kExactMatch   | Matching                | Non-matching (99)  | Matching                                 | Err: kVersionMismatch | VerifyVersionFailsOnMajorMismatch (BV1–BV6)                       |
/// | EC4  | kExactMatch   | Matching                | Matching           | Non-matching (~actual)                   | Err: kVersionMismatch | VerifyVersionFailsOnMinorMismatch (BV1–BV6)                       |
/// | EC5  | kExactMatch   | Non-matching ("XXXX")   | Non-matching (99)  | Non-matching (99)                        | Err: kVersionMismatch | VerifyVersionFailsOnVersionInfoMismatch (BV1–BV6)                 |
/// | EC6  | kMinorMinimum | Matching                | Matching           | actual >= expected (expected = 0)        | Ok                    | VerifyVersionMinorMinimumSucceedsWhenActualMinorIsHigher (BV1–BV6)|
/// | EC7  | kMinorMinimum | Matching                | Matching           | actual == expected (exact match per BV)  | Ok (boundary edge)    | VerifyVersionMinorMinimumSucceedsWhenAllEqual (BV1–BV6)           |
/// | EC8  | kMinorMinimum | Non-matching ("XXXX")   | Matching           | actual >= expected (expected = 0)        | Err: kVersionMismatch | VerifyVersionMinorMinimumFailsOnIdentifierMismatch (BV1–BV6)      |
/// | EC9  | kMinorMinimum | Matching                | Non-matching (99)  | actual >= expected (expected = 0)        | Err: kVersionMismatch | VerifyVersionMinorMinimumFailsOnMajorMismatch (BV1–BV6)           |
/// | EC10 | kMinorMinimum | Matching                | Matching           | actual < expected (UINT16_MAX)           | Err: kVersionMismatch | VerifyVersionMinorMinimumFailsWhenActualMinorIsLower (BV1–BV4)    |
/// | EC11 | kMinorMinimum | Non-matching ("XXXX")   | Non-matching (99)  | actual < expected (expected = UINT16_MAX)| Err: kVersionMismatch | VerifyVersionMinorMinimumFailsWhenAllWrong (BV1–BV6)              |
///
/// All parameterized `VerifyVersion` tests below (EC1–EC11) are executed against the full
/// boundary value set BV1–BV6 defined in the `GetVersion` boundary value table above.
///
// clang-format on

TEST_P(VersionReaderBufferParamTest, VerifyVersionSucceedsOnMatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "VerifyVersion returns success if version info matches expected");

    const auto& param = GetParam();

    const auto result = version_reader_.VerifyVersion(data_, param.version_info);
    EXPECT_TRUE(result.has_value());

    const auto result2 = score::flatbuffers::VerifyVersion(data_, param.version_info);
    EXPECT_TRUE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnIdentifierMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "VerifyVersion returns error if only identifier does not match expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    expected = {"XXXX",
                param.version_info.major_version,
                param.version_info.minor_version};  // wrong identifier; major and minor remain correct

    const auto result = version_reader_.VerifyVersion(data_, expected);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnMajorMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "VerifyVersion returns error if only major version does not match expected");

    score::flatbuffers::BufferVersionInfo expected = GetParam().version_info;
    expected.major_version = 99U;  // wrong major; minor and identifier remain correct

    const auto result = version_reader_.VerifyVersion(data_, expected);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnMinorMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "VerifyVersion returns error if only minor version does not match expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    // Bitwise complement guarantees a different value for any uint16_t actual minor version
    expected.minor_version = static_cast<uint16_t>(~param.version_info.minor_version);

    const auto result = version_reader_.VerifyVersion(data_, expected);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnVersionInfoMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "VerifyVersion returns error if identifier, major, and minor version are all wrong");

    // identifier="XXXX", major=99, minor=99 are wrong for every BV instance;
    // minor=99 avoids an accidental match with BV4 (actual minor=1) that minor=1 would cause.
    score::flatbuffers::BufferVersionInfo expected = {"XXXX", 99U, 99U};

    const auto result = version_reader_.VerifyVersion(data_, expected);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumSucceedsWhenActualMinorIsHigher)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns success if actual minor version is higher than expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    // For the minimum version buffers we can only prove minor==0.
    // All other will confirm that minor>=0 and therefore valid.
    expected.minor_version = 0;

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result.has_value());

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumSucceedsWhenAllEqual)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns success if actual minor equals the expected minimum "
                   "(actual == expected)");

    const auto& param = GetParam();
    // Set expected minor to the exact value stored in the buffer so actual == expected for each BV instance.
    score::flatbuffers::BufferVersionInfo expected = param.version_info;

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result.has_value());

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumFailsOnIdentifierMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns error if identifier does not match, even if major and "
                   "minor satisfy the condition");

    const auto& param = GetParam();
    // wrong identifier; major matches; minor=0 so actual >= 0 always satisfies the minimum condition
    score::flatbuffers::BufferVersionInfo expected = {"XXXX", param.version_info.major_version, 0U};

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumFailsOnMajorMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns error if major version does not match, even if minor "
                   "satisfies the minimum");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    expected.major_version = 99U;  // wrong major
    expected.minor_version = 0U;   // actual >= 0 always, so minor condition is satisfied

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumFailsWhenActualMinorIsLower)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns error if actual minor version is lower than expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    expected.minor_version = UINT16_MAX;

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    if (param.version_info.minor_version == UINT16_MAX)
    {
        // If the actual minor version is at maximum we can't expect a higher value
        // Test fallbacks to expected minor == actual (UINT16_MAX) which is valid.
        EXPECT_TRUE(result.has_value());
    }
    else
    {
        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
    }

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    if (param.version_info.minor_version == UINT16_MAX)
    {
        // If the actual minor version is at maximum we can't expect a higher value
        // Test fallbacks to expected minor == actual (UINT16_MAX) which is valid.
        EXPECT_TRUE(result2.has_value());
    }
    else
    {
        ASSERT_FALSE(result2.has_value());
        EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
    }
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumFailsWhenAllWrong)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns error if both major and minor version do not match");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    expected.major_version = 99U;          // wrong major
    expected.minor_version = UINT16_MAX;   // actual < UINT16_MAX for BV1–BV5; major mismatch causes failure regardless
    expected = {"XXXX", 99U, UINT16_MAX};  // identifier, major, and minor all wrong

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVersionMismatch));
}
