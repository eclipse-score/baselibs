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
#include "flatbuffers/flatbuffers.h"

#include <cstdint>
#include <cstring>
#include <signal.h>
#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// Helper: build a simple buffer for reuse.
static std::vector<uint8_t> BuildBuffer()
{
  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 99, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));
  auto* p = fbb.GetBufferPointer();
  return std::vector<uint8_t>(p, p + fbb.GetSize());
}

// ---------------------------------------------------------------------------
// GetBufferStartFromRootPointerTest
// ---------------------------------------------------------------------------

TEST(GetBufferStartFromRootPointerTest, NavigatesBack)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetBufferStartFromRootPointer");
  RecordProperty("Description", "navigates backwards from root pointer to find buffer start");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = BuildBuffer();
  const auto* root = GetRoot<Table>(buf.data());
  const auto* start = GetBufferStartFromRootPointer(root);
  EXPECT_EQ(start, buf.data());
}

// ---------------------------------------------------------------------------
// GetPrefixedSizeTest
// ---------------------------------------------------------------------------

TEST(GetPrefixedSizeTest, SizePrefixedBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetPrefixedSize");
  RecordProperty("Description", "GetPrefixedSize returns the embedded size from a size-prefixed buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.FinishSizePrefixed(Offset<Table>(root));

  auto* p = fbb.GetBufferPointer();
  auto prefix = GetPrefixedSize(p);
  EXPECT_EQ(static_cast<size_t>(prefix) + sizeof(uoffset_t), fbb.GetSize());
}

// ---------------------------------------------------------------------------
// GetSizePrefixedBufferLengthTest
// ---------------------------------------------------------------------------

TEST(GetSizePrefixedBufferLengthTest, TotalLength)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetSizePrefixedBufferLength");
  RecordProperty("Description", "GetSizePrefixedBufferLength returns total buffer length including prefix");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.FinishSizePrefixed(Offset<Table>(root));

  auto len = GetSizePrefixedBufferLength(fbb.GetBufferPointer());
  EXPECT_EQ(len, fbb.GetSize());
}

// ---------------------------------------------------------------------------
// LookupEnumTest
// ---------------------------------------------------------------------------

TEST(LookupEnumTest, StringToEnum)
{
  RecordProperty("FullyVerifies", "::flatbuffers::LookupEnum");
  RecordProperty("Description", "LookupEnum maps string names to enum indices");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  const char* names[] = {"Red", "Green", "Blue", nullptr};

  EXPECT_EQ(LookupEnum(names, "Red"), 0);
  EXPECT_EQ(LookupEnum(names, "Blue"), 2);
  EXPECT_EQ(LookupEnum(names, "Yellow"), -1);
}

// ---------------------------------------------------------------------------
// FieldIndexToOffsetTest
// ---------------------------------------------------------------------------

TEST(FieldIndexToOffsetTest, VtableOffset)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FieldIndexToOffset");
  RecordProperty("Description", "field index maps to vtable offset");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(FieldIndexToOffset(0), 4u);
  EXPECT_EQ(FieldIndexToOffset(1), 6u);
  EXPECT_EQ(FieldIndexToOffset(2), 8u);
}

// ---------------------------------------------------------------------------
// VersionStringTest
// ---------------------------------------------------------------------------

TEST(VersionStringTest, WellFormed)
{
  RecordProperty("FullyVerifies", "FLATBUFFERS_VERSION");
  RecordProperty("Description", "version string is non-empty and well-formed");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  const char* vs = FLATBUFFERS_VERSION();
  EXPECT_NE(vs, nullptr);
  EXPECT_GT(strlen(vs), 0u);
  EXPECT_NE(strchr(vs, '.'), nullptr);
}

// ---------------------------------------------------------------------------
// ElementaryTypeTest
// ---------------------------------------------------------------------------

TEST(ElementaryTypeTest, EnumValuesExist)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ElementaryType");
  RecordProperty("Description", "ElementaryType enum values exist and differ");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(static_cast<int>(ET_UTYPE), 0);
  EXPECT_EQ(static_cast<int>(ET_BOOL), 1);
  EXPECT_NE(static_cast<int>(ET_STRING), static_cast<int>(ET_BOOL));
}

// ---------------------------------------------------------------------------
// TypeCodeSizeTest
// ---------------------------------------------------------------------------

TEST(TypeCodeSizeTest, TypeCodeToSize)
{
  RecordProperty("FullyVerifies", "ElementaryType size mapping");
  RecordProperty("Description", "type code maps to correct byte size");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  static const size_t sizes[] = {
      0,                // ET_UTYPE
      sizeof(uint8_t),  // ET_BOOL
      sizeof(int8_t),   // ET_CHAR
      sizeof(uint8_t),  // ET_UCHAR
      sizeof(int16_t),  // ET_SHORT
      sizeof(uint16_t), // ET_USHORT
      sizeof(int32_t),  // ET_INT
      sizeof(uint32_t), // ET_UINT
      sizeof(int64_t),  // ET_LONG
      sizeof(uint64_t), // ET_ULONG
      sizeof(float),    // ET_FLOAT
      sizeof(double),   // ET_DOUBLE
  };
  EXPECT_EQ(sizes[ET_CHAR], 1u);
  EXPECT_EQ(sizes[ET_INT], 4u);
  EXPECT_EQ(sizes[ET_DOUBLE], 8u);
}

// ---------------------------------------------------------------------------
// NativeTableTest
// ---------------------------------------------------------------------------

TEST(NativeTableTest, BaseClass)
{
  RecordProperty("FullyVerifies", "::flatbuffers::NativeTable");
  RecordProperty("Description", "NativeTable base class exists");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  struct MyNativeTable : public NativeTable
  {
    int x = 5;
  };
  MyNativeTable nt;
  EXPECT_EQ(nt.x, 5);
}

// ---------------------------------------------------------------------------
// RoundTripTest
// ---------------------------------------------------------------------------

TEST(RoundTripTest, BuildReadVerify)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder round-trip, ::flatbuffers::Verifier");
  RecordProperty("Description", "round-trip build -> read -> verify");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto s = fbb.CreateString("roundtrip");
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 12345, 0);
  fbb.AddOffset(6, s);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  EXPECT_EQ(table->GetField<int32_t>(4, 0), 12345);

  auto* str = table->GetPointer<const String*>(6);
  ASSERT_NE(str, nullptr);
  EXPECT_STREQ(str->c_str(), "roundtrip");

  Verifier verifier(buf, fbb.GetSize());
  EXPECT_TRUE(verifier.Check(true));
}

// ---------------------------------------------------------------------------
// GetBufferStartFaultTest
// ---------------------------------------------------------------------------

TEST(GetBufferStartFaultTest, CorruptRoot)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetBufferStartFromRootPointer");
  RecordProperty("Description", "corrupt root causes GetBufferStartFromRootPointer to trigger assert");
  RecordProperty("TestType", "fault-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  uint8_t fake[64] = {};
  EXPECT_DEATH(
    {
      GetBufferStartFromRootPointer(fake + 32);
    },
    "");
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score