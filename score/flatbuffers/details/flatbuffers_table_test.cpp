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
#include "flatbuffers/table.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#include "flatbuffers/flatbuffer_builder.h"
#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// Helper: build a table with scalar fields.
struct SimpleTableHelper
{
  static std::vector<uint8_t> Build(int32_t f0, int16_t f1, uint8_t f2)
  {
    FlatBufferBuilder fbb(256);
    auto start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, f0, 0);
    fbb.AddElement<int16_t>(6, f1, 0);
    fbb.AddElement<uint8_t>(8, f2, 0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    auto* ptr = fbb.GetBufferPointer();
    return std::vector<uint8_t>(ptr, ptr + fbb.GetSize());
  }
};

// ---------------------------------------------------------------------------
// TableGetVTableTest
// ---------------------------------------------------------------------------

TEST(TableGetVTableTest, Pointer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetVTable");
  RecordProperty("Description", "GetVTable returns a pointer within the buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(42, 7, 1);
  auto* table = GetRoot<Table>(buf.data());
  ASSERT_NE(table, nullptr);
  auto* vtable = table->GetVTable();
  EXPECT_NE(vtable, nullptr);
  EXPECT_GE(vtable, buf.data());
  EXPECT_LT(vtable, buf.data() + buf.size());
}

// ---------------------------------------------------------------------------
// TableGetOptionalFieldOffsetTest
// ---------------------------------------------------------------------------

TEST(TableGetOptionalFieldOffsetTest, PresentAbsent)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetOptionalFieldOffset");
  RecordProperty("Description", "present field returns non-zero offset, absent returns 0");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(42, 7, 1);
  auto* table = GetRoot<Table>(buf.data());

  EXPECT_NE(table->GetOptionalFieldOffset(4), 0);
  EXPECT_NE(table->GetOptionalFieldOffset(6), 0);
  EXPECT_NE(table->GetOptionalFieldOffset(8), 0);
  EXPECT_EQ(table->GetOptionalFieldOffset(10), static_cast<voffset_t>(0));
  EXPECT_EQ(table->GetOptionalFieldOffset(1000), static_cast<voffset_t>(0));
}

// ---------------------------------------------------------------------------
// TableGetFieldTest
// ---------------------------------------------------------------------------

TEST(TableGetFieldTest, DefaultOrDefault)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetField<T>");
  RecordProperty("Description", "GetField returns stored value or provided default");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    auto buf = SimpleTableHelper::Build(42, -3, 255);
    auto* table = GetRoot<Table>(buf.data());
    EXPECT_EQ(table->GetField<int32_t>(4, 0), 42);
    EXPECT_EQ(table->GetField<int16_t>(6, 0), static_cast<int16_t>(-3));
    EXPECT_EQ(table->GetField<uint8_t>(8, 0), 255);
  }
  {
    auto buf = SimpleTableHelper::Build(42, 7, 1);
    auto* table = GetRoot<Table>(buf.data());
    EXPECT_EQ(table->GetField<int32_t>(10, 999), 999);
  }
}

TEST(TableGetFieldTest, ForceDefaults)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetField<T>");
  RecordProperty("Description", "zero values with ForceDefaults are stored and retrievable");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
  fbb.ForceDefaults(true);
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 0, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  EXPECT_EQ(table->GetField<int32_t>(4, 99), 0);
}

// ---------------------------------------------------------------------------
// TableSetFieldTest
// ---------------------------------------------------------------------------

TEST(TableSetFieldTest, ModifyInPlace)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::SetField<T>");
  RecordProperty("Description", "SetField modifies the value in-place");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(10, 20, 30);
  auto* table = GetMutableRoot<Table>(buf.data());

  bool ok = table->SetField<int32_t>(4, 100, 0);
  EXPECT_TRUE(ok);
  EXPECT_EQ(table->GetField<int32_t>(4, 0), 100);

  ok = table->SetField<int32_t>(10, 0, 0);
  EXPECT_TRUE(ok);
  ok = table->SetField<int32_t>(10, 42, 0);
  EXPECT_FALSE(ok);
}

// ---------------------------------------------------------------------------
// TableCheckFieldTest
// ---------------------------------------------------------------------------

TEST(TableCheckFieldTest, PresentAbsent)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::CheckField");
  RecordProperty("Description", "CheckField returns true for present fields, false for absent");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(1, 2, 3);
  auto* table = GetRoot<Table>(buf.data());
  EXPECT_TRUE(table->CheckField(4));
  EXPECT_FALSE(table->CheckField(10));
}

// ---------------------------------------------------------------------------
// TableGetOptionalTest
// ---------------------------------------------------------------------------

TEST(TableGetOptionalTest, PresentAbsent)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetOptional<T, U>");
  RecordProperty("Description", "present field returns Optional with value, absent returns empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(42, 7, 1);
  auto* table = GetRoot<Table>(buf.data());

  auto opt = table->GetOptional<int32_t, int32_t>(4);
  EXPECT_TRUE(opt.has_value());
  EXPECT_EQ(*opt, 42);

  auto opt2 = table->GetOptional<int32_t, int32_t>(10);
  EXPECT_FALSE(opt2.has_value());
}

// ---------------------------------------------------------------------------
// TableGetAddressOfTest
// ---------------------------------------------------------------------------

TEST(TableGetAddressOfTest, PresentAbsent)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetAddressOf");
  RecordProperty("Description", "GetAddressOf returns non-null for present field, null for absent");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto buf = SimpleTableHelper::Build(42, 7, 1);
  auto* table = GetMutableRoot<Table>(buf.data());

  EXPECT_NE(table->GetAddressOf(4), nullptr);
  EXPECT_EQ(table->GetAddressOf(10), nullptr);

  const Table* ctable = table;
  EXPECT_NE(ctable->GetAddressOf(4), nullptr);
  EXPECT_EQ(ctable->GetAddressOf(10), nullptr);
}

// ---------------------------------------------------------------------------
// TableGetPointerTest
// ---------------------------------------------------------------------------

TEST(TableGetPointerTest, OffsetField)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetPointer<T>");
  RecordProperty("Description", "GetPointer for offset fields");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto str = fbb.CreateString("test_string");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, str);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);

  const String* s = table->GetPointer<const String*>(4);
  ASSERT_NE(s, nullptr);
  EXPECT_STREQ(s->c_str(), "test_string");

  const String* s2 = table->GetPointer<const String*>(6);
  EXPECT_EQ(s2, nullptr);
}

// ---------------------------------------------------------------------------
// TableGetVectorPointerOrEmptyTest
// ---------------------------------------------------------------------------

TEST(TableGetVectorPointerOrEmptyTest, NoOptionalField)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Table::GetVectorPointerOrEmpty<T>");
  RecordProperty("Description", "absent vector field returns empty vector (not nullptr)");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);

  const Vector<int32_t>* v = table->GetVectorPointerOrEmpty<int32_t>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 0u);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score