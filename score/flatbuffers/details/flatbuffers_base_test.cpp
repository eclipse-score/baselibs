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
#include "flatbuffers/base.h"

#include <cstring>
#include <limits>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// ---------------------------------------------------------------------------
// BaseVersionMacrosTest
// Tests FLATBUFFERS_VERSION macros and FLATBUFFERS_VERSION().
// ---------------------------------------------------------------------------

TEST(BaseVersionMacrosTest, VersionInfo)
{
  RecordProperty("FullyVerifies", "FLATBUFFERS_VERSION_MAJOR, FLATBUFFERS_VERSION_MINOR, FLATBUFFERS_VERSION_REVISION, FLATBUFFERS_VERSION");
  RecordProperty("Description", "FLATBUFFERS_VERSION macros return non-negative values; version string has format X.Y.Z");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_GE(FLATBUFFERS_VERSION_MAJOR, 0);
  EXPECT_GE(FLATBUFFERS_VERSION_MINOR, 0);
  EXPECT_GE(FLATBUFFERS_VERSION_REVISION, 0);

  const char* v = FLATBUFFERS_VERSION();
  EXPECT_NE(v, nullptr);
  EXPECT_GE(strlen(v), 5u);

  int dot_count = 0;
  for (const char* p = v; *p; ++p)
  {
    if (*p == '.')
      ++dot_count;
  }
  EXPECT_EQ(dot_count, 2);
}

// ---------------------------------------------------------------------------
// BaseTypeSizesTest
// Tests fundamental type sizes.
// ---------------------------------------------------------------------------

TEST(BaseTypeSizesTest, WireFormatTypes)
{
  RecordProperty("FullyVerifies", "uoffset_t, soffset_t, uoffset64_t, soffset64_t, voffset_t");
  RecordProperty("Description", "wire-format type sizes match expected widths");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  EXPECT_EQ(sizeof(uoffset_t), 4u);
  EXPECT_EQ(sizeof(soffset_t), 4u);
  EXPECT_EQ(sizeof(uoffset64_t), 8u);
  EXPECT_EQ(sizeof(soffset64_t), 8u);
  EXPECT_EQ(sizeof(voffset_t), 2u);
}

// ---------------------------------------------------------------------------
// BaseEndianSwapTest
// Tests EndianSwap<T>.
// ---------------------------------------------------------------------------

TEST(BaseEndianSwapTest, SwapValues)
{
  RecordProperty("FullyVerifies", "::flatbuffers::EndianSwap");
  RecordProperty("Description", "EndianSwap reverses byte order for 1, 2, 4 and 8 byte values");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  EXPECT_EQ(EndianSwap(static_cast<uint8_t>(0x00)), static_cast<uint8_t>(0x00));
  EXPECT_EQ(EndianSwap(static_cast<uint8_t>(0xFF)), static_cast<uint8_t>(0xFF));
  EXPECT_EQ(EndianSwap(static_cast<uint8_t>(0x42)), static_cast<uint8_t>(0x42));

  EXPECT_EQ(EndianSwap(static_cast<uint16_t>(0x0000)), static_cast<uint16_t>(0x0000));
  EXPECT_EQ(EndianSwap(static_cast<uint16_t>(0xFFFF)), static_cast<uint16_t>(0xFFFF));
  EXPECT_EQ(EndianSwap(static_cast<uint16_t>(0x0102)), static_cast<uint16_t>(0x0201));

  EXPECT_EQ(EndianSwap(static_cast<uint32_t>(0x00000000u)), static_cast<uint32_t>(0x00000000u));
  EXPECT_EQ(EndianSwap(static_cast<uint32_t>(0xFFFFFFFFu)), static_cast<uint32_t>(0xFFFFFFFFu));
  EXPECT_EQ(EndianSwap(static_cast<uint32_t>(0x01020304u)), static_cast<uint32_t>(0x04030201u));

  EXPECT_EQ(EndianSwap(static_cast<uint64_t>(0x0000000000000000ULL)), static_cast<uint64_t>(0x0000000000000000ULL));
  EXPECT_EQ(EndianSwap(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL)), static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL));
  EXPECT_EQ(EndianSwap(static_cast<uint64_t>(0x0102030405060708ULL)), static_cast<uint64_t>(0x0807060504030201ULL));

  EXPECT_EQ(EndianSwap(EndianSwap(static_cast<uint16_t>(0x1234))), static_cast<uint16_t>(0x1234));
  EXPECT_EQ(EndianSwap(EndianSwap(static_cast<uint32_t>(0xDEADBEEFu))), static_cast<uint32_t>(0xDEADBEEFu));
  EXPECT_EQ(EndianSwap(EndianSwap(static_cast<uint64_t>(0x123456789ABCDEF0ULL))), static_cast<uint64_t>(0x123456789ABCDEF0ULL));
}

// ---------------------------------------------------------------------------
// BaseEndianScalarTest
// Tests EndianScalar<T>.
// ---------------------------------------------------------------------------

TEST(BaseEndianScalarTest, SwapValue)
{
  RecordProperty("FullyVerifies", "::flatbuffers::EndianScalar");
  RecordProperty("Description", "EndianScalar applies correct byte swap based on platform endianness");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

#if FLATBUFFERS_LITTLEENDIAN
  EXPECT_EQ(EndianScalar(static_cast<uint32_t>(0x01020304u)), static_cast<uint32_t>(0x01020304u));
#else
  EXPECT_EQ(EndianScalar(static_cast<uint32_t>(0x01020304u)), EndianSwap(static_cast<uint32_t>(0x01020304u)));
#endif
  EXPECT_EQ(EndianScalar(EndianScalar(static_cast<uint32_t>(42u))), static_cast<uint32_t>(42u));
}

// ---------------------------------------------------------------------------
// BaseReadWriteScalarTest
// Tests ReadScalar<T>, WriteScalar<T>.
// ---------------------------------------------------------------------------

TEST(BaseReadWriteScalarTest, ScalarReadWrite)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ReadScalar, ::flatbuffers::WriteScalar");
  RecordProperty("Description", "ReadScalar and WriteScalar round-trip values for various scalar types");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    uint8_t buf[4] = {};
    WriteScalar(buf, static_cast<uint32_t>(0xDEADBEEFu));
    EXPECT_EQ(ReadScalar<uint32_t>(buf), static_cast<uint32_t>(0xDEADBEEFu));
  }
  {
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    WriteScalar(buf, static_cast<uint32_t>(0u));
    EXPECT_EQ(ReadScalar<uint32_t>(buf), static_cast<uint32_t>(0u));
  }
  {
    uint8_t buf[2] = {};
    WriteScalar(buf, static_cast<int16_t>(-1));
    EXPECT_EQ(ReadScalar<int16_t>(buf), static_cast<int16_t>(-1));
  }
  {
    uint8_t buf[2] = {};
    WriteScalar(buf, std::numeric_limits<int16_t>::min());
    EXPECT_EQ(ReadScalar<int16_t>(buf), std::numeric_limits<int16_t>::min());
  }
  {
    uint8_t buf[8] = {};
    WriteScalar(buf, std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(ReadScalar<uint64_t>(buf), std::numeric_limits<uint64_t>::max());
  }
  {
    uint8_t buf[4] = {};
    float expected = 3.14f;
    WriteScalar(buf, expected);
    float result = ReadScalar<float>(buf);
    EXPECT_EQ(0, memcmp(&result, &expected, sizeof(float)));
  }
}

// ---------------------------------------------------------------------------
// BasePaddingBytesTest
// Tests PaddingBytes.
// ---------------------------------------------------------------------------

TEST(BasePaddingBytesTest, PaddingCalculation)
{
  RecordProperty("FullyVerifies", "::flatbuffers::PaddingBytes");
  RecordProperty("Description", "PaddingBytes computes correct padding for alignment");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  EXPECT_EQ(PaddingBytes(0, 1), 0u);
  EXPECT_EQ(PaddingBytes(1, 1), 0u);
  EXPECT_EQ(PaddingBytes(100, 1), 0u);

  EXPECT_EQ(PaddingBytes(0, 4), 0u);
  EXPECT_EQ(PaddingBytes(1, 4), 3u);
  EXPECT_EQ(PaddingBytes(2, 4), 2u);
  EXPECT_EQ(PaddingBytes(3, 4), 1u);
  EXPECT_EQ(PaddingBytes(4, 4), 0u);

  EXPECT_EQ(PaddingBytes(0, 8), 0u);
  EXPECT_EQ(PaddingBytes(1, 8), 7u);
  EXPECT_EQ(PaddingBytes(7, 8), 1u);
  EXPECT_EQ(PaddingBytes(8, 8), 0u);
}

// ---------------------------------------------------------------------------
// BaseVerifyAlignmentRequirementsTest
// Tests VerifyAlignmentRequirements.
// ---------------------------------------------------------------------------

TEST(BaseVerifyAlignmentRequirementsTest, AlignmentChecks)
{
  RecordProperty("FullyVerifies", "::flatbuffers::VerifyAlignmentRequirements");
  RecordProperty("Description", "VerifyAlignmentRequirements accepts powers of two and rejects others");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE(VerifyAlignmentRequirements(1));
  EXPECT_TRUE(VerifyAlignmentRequirements(2));
  EXPECT_TRUE(VerifyAlignmentRequirements(4));
  EXPECT_TRUE(VerifyAlignmentRequirements(8));
  EXPECT_TRUE(VerifyAlignmentRequirements(16));
  EXPECT_TRUE(VerifyAlignmentRequirements(FLATBUFFERS_MAX_ALIGNMENT));

  EXPECT_FALSE(VerifyAlignmentRequirements(0));
  EXPECT_FALSE(VerifyAlignmentRequirements(3));
  EXPECT_FALSE(VerifyAlignmentRequirements(5));
  EXPECT_FALSE(VerifyAlignmentRequirements(6));
  EXPECT_FALSE(VerifyAlignmentRequirements(static_cast<size_t>(FLATBUFFERS_MAX_ALIGNMENT) * 2));

  EXPECT_TRUE(VerifyAlignmentRequirements(4, 2));
  EXPECT_TRUE(VerifyAlignmentRequirements(4, 4));
  EXPECT_FALSE(VerifyAlignmentRequirements(2, 4));
}

// ---------------------------------------------------------------------------
// BaseIsTheSameAsTest
// Tests IsTheSameAs<T>.
// ---------------------------------------------------------------------------

TEST(BaseIsTheSameAsTest, Comparison)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IsTheSameAs");
  RecordProperty("Description", "IsTheSameAs compares values for equality");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE(IsTheSameAs(0, 0));
  EXPECT_TRUE(IsTheSameAs(42, 42));
  EXPECT_FALSE(IsTheSameAs(0, 1));
  EXPECT_TRUE(IsTheSameAs(-1, -1));
  EXPECT_FALSE(IsTheSameAs(-1, 1));
  EXPECT_TRUE(IsTheSameAs(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()));
  EXPECT_TRUE(IsTheSameAs(1.0f, 1.0f));
  EXPECT_FALSE(IsTheSameAs(1.0f, 2.0f));
}

// ---------------------------------------------------------------------------
// BaseIsInRangeTest
// Tests IsInRange<T>.
// ---------------------------------------------------------------------------

TEST(BaseIsInRangeTest, RangeChecks)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IsInRange");
  RecordProperty("Description", "IsInRange checks if a value falls within a closed range");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  EXPECT_TRUE(IsInRange(0, 0, 10));
  EXPECT_TRUE(IsInRange(10, 0, 10));
  EXPECT_TRUE(IsInRange(5, 0, 10));
  EXPECT_FALSE(IsInRange(-1, 0, 10));
  EXPECT_FALSE(IsInRange(11, 0, 10));
  EXPECT_TRUE(IsInRange(5, 5, 5));
  EXPECT_FALSE(IsInRange(4, 5, 5));
  EXPECT_FALSE(IsInRange(6, 5, 5));
}

// ---------------------------------------------------------------------------
// BaseIsOutRangeTest
// Tests IsOutRange<T>.
// ---------------------------------------------------------------------------

TEST(BaseIsOutRangeTest, OutOfRangeChecks)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IsOutRange");
  RecordProperty("Description", "IsOutRange returns true when value is outside the given range");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_FALSE(IsOutRange(0, 0, 10));
  EXPECT_FALSE(IsOutRange(10, 0, 10));
  EXPECT_TRUE(IsOutRange(-1, 0, 10));
  EXPECT_TRUE(IsOutRange(11, 0, 10));
}

// ---------------------------------------------------------------------------
// BaseIsConstTrueTest
// Tests IsConstTrue.
// ---------------------------------------------------------------------------

TEST(BaseIsConstTrueTest, ConstBoolValues)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IsConstTrue");
  RecordProperty("Description", "IsConstTrue evaluates constant true/false values");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE(IsConstTrue(true));
  EXPECT_FALSE(IsConstTrue(false));
  EXPECT_TRUE(IsConstTrue(1));
  EXPECT_FALSE(IsConstTrue(0));
}

// ---------------------------------------------------------------------------
// BaseFileIdentifierLengthTest
// Tests kFileIdentifierLength.
// ---------------------------------------------------------------------------

TEST(BaseFileIdentifierLengthTest, ConstantValue)
{
  RecordProperty("FullyVerifies", "kFileIdentifierLength");
  RecordProperty("Description", "kFileIdentifierLength equals 4");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(kFileIdentifierLength, 4u);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score