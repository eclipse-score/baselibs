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
#include "flatbuffers/buffer.h"

#include <cstring>
#include <limits>
#include <string>
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

// ===========================================================================
// Offset
// ===========================================================================

// ---------------------------------------------------------------------------
// OffsetDefaultTest
// ---------------------------------------------------------------------------

TEST(OffsetDefaultTest, Null)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Offset<T>::IsNull, ::flatbuffers::Offset<T>::o");
  RecordProperty("Description", "default Offset is null (o == 0)");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Offset<void> off;
  EXPECT_TRUE(off.IsNull());
  EXPECT_EQ(off.o, 0u);
}

// ---------------------------------------------------------------------------
// OffsetNonNullTest
// ---------------------------------------------------------------------------

TEST(OffsetNonNullTest, NotNull)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Offset<T>::IsNull, ::flatbuffers::Offset<T>::o");
  RecordProperty("Description", "non-zero Offset is not null, including max value");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    Offset<void> off(1);
    EXPECT_FALSE(off.IsNull());
    EXPECT_EQ(off.o, 1u);
  }
  {
    Offset<void> off(std::numeric_limits<uoffset_t>::max());
    EXPECT_FALSE(off.IsNull());
  }
}

// ---------------------------------------------------------------------------
// OffsetUnionTest
// ---------------------------------------------------------------------------

TEST(OffsetUnionTest, UntypedConversion)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Offset<T>::Union");
  RecordProperty("Description", "Union() returns untyped Offset preserving value");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Offset<int> typed(42);
  Offset<> untyped = typed.Union();
  EXPECT_EQ(untyped.o, 42u);
}

// ---------------------------------------------------------------------------
// OffsetSizeTest
// ---------------------------------------------------------------------------

TEST(OffsetSizeTest, Width)
{
  RecordProperty("FullyVerifies", "sizeof(Offset<T>)");
  RecordProperty("Description", "sizeof(Offset) == 4 bytes");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(sizeof(Offset<>), 4u);
}

// ===========================================================================
// Offset64
// ===========================================================================

// ---------------------------------------------------------------------------
// Offset64DefaultTest
// ---------------------------------------------------------------------------

TEST(Offset64DefaultTest, Null)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Offset64<T>::IsNull, ::flatbuffers::Offset64<T>::o");
  RecordProperty("Description", "default Offset64 is null");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Offset64<void> off;
  EXPECT_TRUE(off.IsNull());
  EXPECT_EQ(off.o, static_cast<uoffset64_t>(0));
}

// ---------------------------------------------------------------------------
// Offset64NonNullTest
// ---------------------------------------------------------------------------

TEST(Offset64NonNullTest, NotNull)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Offset64<T>::IsNull");
  RecordProperty("Description", "Offset64 with value is not null");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Offset64<void> off(1);
  EXPECT_FALSE(off.IsNull());
}

// ---------------------------------------------------------------------------
// Offset64SizeTest
// ---------------------------------------------------------------------------

TEST(Offset64SizeTest, Width)
{
  RecordProperty("FullyVerifies", "sizeof(Offset64<T>)");
  RecordProperty("Description", "sizeof(Offset64) == 8 bytes");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(sizeof(Offset64<>), 8u);
}

// ===========================================================================
// Offset traits
// ===========================================================================

// ---------------------------------------------------------------------------
// OffsetTraitsTest
// ---------------------------------------------------------------------------

TEST(OffsetTraitsTest, TypeTraits)
{
  RecordProperty("FullyVerifies", "::flatbuffers::is_specialisation_of_Offset, ::flatbuffers::is_specialisation_of_Offset64");
  RecordProperty("Description", "traits correctly identify Offset and Offset64 specializations");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE((is_specialisation_of_Offset<Offset<int>>::value));
  EXPECT_TRUE((is_specialisation_of_Offset<Offset<void>>::value));
  EXPECT_FALSE((is_specialisation_of_Offset<int>::value));
  EXPECT_FALSE((is_specialisation_of_Offset<Offset64<int>>::value));

  EXPECT_TRUE((is_specialisation_of_Offset64<Offset64<int>>::value));
  EXPECT_FALSE((is_specialisation_of_Offset64<Offset<int>>::value));
  EXPECT_FALSE((is_specialisation_of_Offset64<int>::value));
}

// ===========================================================================
// StringLessThan
// ===========================================================================

// ---------------------------------------------------------------------------
// StringLessThanTest
// ---------------------------------------------------------------------------

TEST(StringLessThanTest, Comparison)
{
  RecordProperty("FullyVerifies", "::flatbuffers::StringLessThan");
  RecordProperty("Description", "StringLessThan compares length-prefixed strings lexicographically");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_TRUE(StringLessThan("abc", 3, "abd", 3));
  EXPECT_FALSE(StringLessThan("abd", 3, "abc", 3));
  EXPECT_TRUE(StringLessThan("ab", 2, "abc", 3));
  EXPECT_FALSE(StringLessThan("abc", 3, "ab", 2));
  EXPECT_TRUE(StringLessThan("", 0, "a", 1));
  EXPECT_FALSE(StringLessThan("a", 1, "", 0));
  EXPECT_FALSE(StringLessThan("", 0, "", 0));
}

// ===========================================================================
// BufferIdentifier
// ===========================================================================

// ---------------------------------------------------------------------------
// BufferIdentifierTest
// ---------------------------------------------------------------------------

TEST(BufferIdentifierTest, IdentifierCheck)
{
  RecordProperty("FullyVerifies", "::flatbuffers::BufferHasIdentifier, ::flatbuffers::GetBufferIdentifier");
  RecordProperty("Description", "BufferHasIdentifier and GetBufferIdentifier check buffer magic");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root), "TEST");

  auto buf = fbb.GetBufferPointer();
  EXPECT_TRUE(BufferHasIdentifier(buf, "TEST"));
  EXPECT_FALSE(BufferHasIdentifier(buf, "XXXX"));

  const char* id = GetBufferIdentifier(buf);
  EXPECT_EQ(memcmp(id, "TEST", 4), 0);
}

// ===========================================================================
// GetRoot / GetMutableRoot
// ===========================================================================

// ---------------------------------------------------------------------------
// GetRootTest
// ---------------------------------------------------------------------------

TEST(GetRootTest, Access)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetRoot<T>, ::flatbuffers::GetMutableRoot<T>");
  RecordProperty("Description", "GetRoot and GetMutableRoot return pointers to root table");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto buf = fbb.GetBufferPointer();
  const Table* t = GetRoot<Table>(buf);
  EXPECT_NE(t, nullptr);

  Table* mt = GetMutableRoot<Table>(buf);
  EXPECT_NE(mt, nullptr);

  EXPECT_EQ(reinterpret_cast<const uint8_t*>(t), reinterpret_cast<const uint8_t*>(mt));
}

// ===========================================================================
// IndirectHelper
// ===========================================================================

// ---------------------------------------------------------------------------
// IndirectHelperScalarTest
// ---------------------------------------------------------------------------

TEST(IndirectHelperScalarTest, Read)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<T>::Read");
  RecordProperty("Description", "IndirectHelper reads scalars at various indices");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  uint32_t arr[] = {10, 20, 30};
  auto* p = reinterpret_cast<const uint8_t*>(arr);
  EXPECT_EQ(IndirectHelper<uint32_t>::Read(p, 0), EndianScalar(10u));
  EXPECT_EQ(IndirectHelper<uint32_t>::Read(p, 1), EndianScalar(20u));
  EXPECT_EQ(IndirectHelper<uint32_t>::Read(p, 2), EndianScalar(30u));
}

TEST(IndirectHelperScalarTest, MutableRead)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<T>::Read");
  RecordProperty("Description", "mutable IndirectHelper Read returns same value as const Read");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  uint32_t arr[] = {100, 200};
  auto* p = reinterpret_cast<uint8_t*>(arr);
  EXPECT_EQ(IndirectHelper<uint32_t>::Read(p, 0), EndianScalar(100u));
  EXPECT_EQ(IndirectHelper<uint32_t>::Read(p, 1), EndianScalar(200u));
}

// ---------------------------------------------------------------------------
// IndirectHelperOffsetTest
// ---------------------------------------------------------------------------

TEST(IndirectHelperOffsetTest, Read)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<Offset<T>>::Read");
  RecordProperty("Description", "follows 32-bit offset indirection to reach a String");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto s0 = fbb.CreateString("alpha");
  auto s1 = fbb.CreateString("beta");
  std::vector<Offset<String>> offsets = {s0, s1};
  auto vec = fbb.CreateVector(offsets);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<Offset<String>>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 2u);

  const String* str0 = v->Get(0);
  const String* str1 = v->Get(1);
  ASSERT_NE(str0, nullptr);
  ASSERT_NE(str1, nullptr);
  EXPECT_STREQ(str0->c_str(), "alpha");
  EXPECT_STREQ(str1->c_str(), "beta");
}

TEST(IndirectHelperOffsetTest, MutableRead)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<Offset<T>>::Read");
  RecordProperty("Description", "mutable Read via raw data pointer returns same object as const Get");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto s0 = fbb.CreateString("hello");
  std::vector<Offset<String>> offsets = {s0};
  auto vec = fbb.CreateVector(offsets);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetMutableRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<Offset<String>>*>(4);
  ASSERT_NE(v, nullptr);

  const String* str = v->Get(0);
  ASSERT_NE(str, nullptr);
  EXPECT_STREQ(str->c_str(), "hello");

  auto* data = const_cast<uint8_t*>(v->Data());
  String* mstr = IndirectHelper<Offset<String>>::Read(data, 0);
  ASSERT_NE(mstr, nullptr);
  EXPECT_EQ(reinterpret_cast<const uint8_t*>(str), reinterpret_cast<const uint8_t*>(mstr));
}

// ===========================================================================
// IndirectHelper for structs
// ===========================================================================

#pragma pack(push, 1)
struct TestPoint FLATBUFFERS_FINAL_CLASS
{
  int16_t x_;
  int16_t y_;

 public:
  TestPoint() : x_(0), y_(0) {}
  TestPoint(int16_t x, int16_t y) : x_(EndianScalar(x)), y_(EndianScalar(y)) {}
  int16_t x() const
  {
    return EndianScalar(x_);
  }
  int16_t y() const
  {
    return EndianScalar(y_);
  }
};
#pragma pack(pop)
static_assert(sizeof(TestPoint) == 4, "TestPoint must be 4 bytes");

TEST(IndirectHelperStructTest, Read)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<const TestPoint*>::Read");
  RecordProperty("Description", "reads structs stored inline at various indices");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  TestPoint pts[] = {TestPoint(1, 2), TestPoint(3, 4), TestPoint(5, 6)};
  auto* p = reinterpret_cast<const uint8_t*>(pts);

  const TestPoint* r0 = IndirectHelper<const TestPoint*>::Read(p, 0);
  const TestPoint* r1 = IndirectHelper<const TestPoint*>::Read(p, 1);
  const TestPoint* r2 = IndirectHelper<const TestPoint*>::Read(p, 2);

  EXPECT_EQ(r0->x(), 1);
  EXPECT_EQ(r0->y(), 2);
  EXPECT_EQ(r1->x(), 3);
  EXPECT_EQ(r1->y(), 4);
  EXPECT_EQ(r2->x(), 5);
  EXPECT_EQ(r2->y(), 6);
}

TEST(IndirectHelperStructTest, MutableRead)
{
  RecordProperty("FullyVerifies", "::flatbuffers::IndirectHelper<TestPoint*>::Read");
  RecordProperty("Description", "mutable IndirectHelper Read returns writable pointer to same data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  TestPoint pts[] = {TestPoint(10, 20), TestPoint(30, 40)};
  auto* p = reinterpret_cast<uint8_t*>(pts);

  TestPoint* r0 = IndirectHelper<TestPoint*>::Read(p, 0);
  TestPoint* r1 = IndirectHelper<TestPoint*>::Read(p, 1);

  EXPECT_EQ(r0->x(), 10);
  EXPECT_EQ(r1->x(), 30);

  EXPECT_EQ(reinterpret_cast<uint8_t*>(r0), p);
  EXPECT_EQ(reinterpret_cast<uint8_t*>(r1), p + sizeof(TestPoint));
}

// ===========================================================================
// GetMutableSizePrefixedRoot
// ===========================================================================

// ---------------------------------------------------------------------------
// GetMutableSizePrefixedRootTest
// ---------------------------------------------------------------------------

TEST(GetMutableSizePrefixedRootTest, Access)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetMutableSizePrefixedRoot<T>");
  RecordProperty("Description", "returns mutable pointer into a size-prefixed buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  fbb.FinishSizePrefixed(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();

  const Table* ct = GetSizePrefixedRoot<Table>(buf);
  ASSERT_NE(ct, nullptr);
  EXPECT_EQ(ct->GetField<int32_t>(4, 0), 42);

  Table* mt = GetMutableSizePrefixedRoot<Table>(buf);
  ASSERT_NE(mt, nullptr);
  EXPECT_EQ(mt->GetField<int32_t>(4, 0), 42);

  EXPECT_EQ(reinterpret_cast<const uint8_t*>(ct), reinterpret_cast<const uint8_t*>(mt));

  bool ok = mt->SetField<int32_t>(4, 99, 0);
  EXPECT_TRUE(ok);
  EXPECT_EQ(ct->GetField<int32_t>(4, 0), 99);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score