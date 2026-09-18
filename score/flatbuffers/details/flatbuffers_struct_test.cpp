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
#include "flatbuffers/struct.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#include "flatbuffers/flatbuffer_builder.h"
#include "gtest/gtest.h"

namespace flatbuffers
{

namespace test
{

}  // namespace test
}  // namespace flatbuffers

// We define a simple packed struct outside the test namespace scope
// to avoid issues with #pragma pack inside class/namespace scope.
// Layout: { int32_t x (offset 0), int16_t y (offset 4), padding 2 bytes, int32_t z (offset 8) }
// Total size: 12, alignment: 4

#pragma pack(push, 1)
struct __attribute__((aligned(4))) TestVec3 FLATBUFFERS_FINAL_CLASS
{
 private:
  int32_t x_;
  int16_t y_;
  int16_t pad0_;
  int32_t z_;

 public:
  TestVec3() : x_(0), y_(0), pad0_(0), z_(0) {}
  TestVec3(int32_t x, int16_t y, int32_t z)
    : x_(::flatbuffers::EndianScalar(x)),
      y_(::flatbuffers::EndianScalar(y)),
      pad0_(0),
      z_(::flatbuffers::EndianScalar(z))
  {}

  int32_t x() const
  {
    return ::flatbuffers::EndianScalar(x_);
  }
  int16_t y() const
  {
    return ::flatbuffers::EndianScalar(y_);
  }
  int32_t z() const
  {
    return ::flatbuffers::EndianScalar(z_);
  }
};
#pragma pack(pop)
static_assert(sizeof(TestVec3) == 12, "compiler breaks packing rules");

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// ---------------------------------------------------------------------------
// StructInTableTest
// ---------------------------------------------------------------------------

TEST(StructInTableTest, Embedded)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetStruct<T>");
  RecordProperty("Description", "a struct embedded inline in a table is retrievable");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  TestVec3 vec(100, -5, 999);

  auto start = fbb.StartTable();
  fbb.AddStruct(4, &vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  ASSERT_NE(table, nullptr);

  const TestVec3* s = table->GetStruct<const TestVec3*>(4);
  ASSERT_NE(s, nullptr);
  EXPECT_EQ(s->x(), 100);
  EXPECT_EQ(s->y(), static_cast<int16_t>(-5));
  EXPECT_EQ(s->z(), 999);
}

// ---------------------------------------------------------------------------
// StructGetFieldTest
// ---------------------------------------------------------------------------

TEST(StructGetFieldTest, ScalarFields)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Struct::GetField<T>");
  RecordProperty("Description", "reading raw scalar fields from a Struct pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  TestVec3 vec(42, 7, -1);

  auto start = fbb.StartTable();
  fbb.AddStruct(4, &vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  const auto* s = reinterpret_cast<const Struct*>(table->GetStruct<const TestVec3*>(4));
  ASSERT_NE(s, nullptr);

  int32_t field_x = s->GetField<int32_t>(0);
  EXPECT_EQ(field_x, 42);
  int16_t field_y = s->GetField<int16_t>(4);
  EXPECT_EQ(field_y, static_cast<int16_t>(7));
  int32_t field_z = s->GetField<int32_t>(8);
  EXPECT_EQ(field_z, -1);
}

// ---------------------------------------------------------------------------
// StructBoundaryTest
// ---------------------------------------------------------------------------

TEST(StructBoundaryTest, MinMaxZero)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetStruct<T>");
  RecordProperty("Description", "struct boundary values: int32_t max, int16_t min, 0");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
  TestVec3 vec(std::numeric_limits<int32_t>::max(), std::numeric_limits<int16_t>::min(), 0);

  auto start = fbb.StartTable();
  fbb.AddStruct(4, &vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  const TestVec3* s = table->GetStruct<const TestVec3*>(4);
  ASSERT_NE(s, nullptr);
  EXPECT_EQ(s->x(), std::numeric_limits<int32_t>::max());
  EXPECT_EQ(s->y(), std::numeric_limits<int16_t>::min());
  EXPECT_EQ(s->z(), 0);
}

// ---------------------------------------------------------------------------
// StructAbsentFieldTest
// ---------------------------------------------------------------------------

TEST(StructAbsentFieldTest, ReturnsNullptr)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetStruct<T>");
  RecordProperty("Description", "absent struct field returns nullptr");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  const TestVec3* s = table->GetStruct<const TestVec3*>(4);
  EXPECT_EQ(s, nullptr);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score