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
#include "flatbuffers/flatbuffer_builder.h"

#include <cstdint>
#include <cstring>
#include <limits>
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

// -----------------------
// FlatBufferBuilderBasicTest
// Tests construction.
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderBasicTest, DefaultConstruct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::FlatBufferBuilder");
  RecordProperty("Description", "newly-constructed builder has no data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  EXPECT_EQ(fbb.GetSize(), 0u);
}

TEST(FlatBufferBuilderBasicTest, CustomInitialSize)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::FlatBufferBuilder(size_t)");
  RecordProperty("Description", "custom initial size accepted, builder still has zero size");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb1(1);
  EXPECT_EQ(fbb1.GetSize(), 0u);

  FlatBufferBuilder fbb2(4096);
  EXPECT_EQ(fbb2.GetSize(), 0u);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateStringTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateStringTest, Normal)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateString");
  RecordProperty("Description", "string creation works for normal, empty, const std::string, char*, and long strings");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  {
    FlatBufferBuilder fbb;
    auto s = fbb.CreateString("hello");
    EXPECT_FALSE(s.IsNull());
  }
  {
    FlatBufferBuilder fbb;
    std::string str = "world";
    auto s = fbb.CreateString(str);
    EXPECT_FALSE(s.IsNull());
  }
  {
    FlatBufferBuilder fbb;
    auto s = fbb.CreateString("");
    auto start = fbb.StartTable();
    fbb.AddOffset(4, s);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* str = table->GetPointer<const String*>(4);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str->size(), 0u);
    EXPECT_STREQ(str->c_str(), "");
  }
  {
    FlatBufferBuilder fbb;
    std::string long_str(10000, 'Z');
    auto s = fbb.CreateString(long_str);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, s);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* str = table->GetPointer<const String*>(4);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str->size(), 10000u);
  }
  {
    FlatBufferBuilder fbb;
    char str[] = "mutable_string";
    auto s = fbb.CreateString(str);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, s);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* result = table->GetPointer<const String*>(4);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result->c_str(), "mutable_string");
    EXPECT_EQ(result->size(), 14u);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateStringFromPtrTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateStringFromPtrTest, FromStringPtr)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateString(const String*)");
  RecordProperty("Description", "CreateString(const String*) copies a String pointer or returns null for nullptr");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto off = fbb.CreateString(static_cast<const String*>(nullptr));
    EXPECT_EQ(off.o, 0u);
  }
  {
    FlatBufferBuilder fbb;
    auto s = fbb.CreateString("source_string");
    auto start = fbb.StartTable();
    fbb.AddOffset(4, s);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    const String* str_ptr = table->GetPointer<const String*>(4);
    ASSERT_NE(str_ptr, nullptr);

    FlatBufferBuilder fbb2;
    auto s2 = fbb2.CreateString(str_ptr);
    EXPECT_FALSE(s2.IsNull());
    auto start2 = fbb2.StartTable();
    fbb2.AddOffset(4, s2);
    auto root2 = fbb2.EndTable(start2);
    fbb2.Finish(Offset<Table>(root2));

    auto* buf2 = fbb2.GetBufferPointer();
    auto* table2 = GetRoot<Table>(buf2);
    const String* result = table2->GetPointer<const String*>(4);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result->c_str(), "source_string");
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorTest, Scalars)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVector");
  RecordProperty("Description", "CreateVector works for std::vector, plain array, initializer_list, empty, and null elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    std::vector<int32_t> data = {1, 2, 3, 4, 5};
    auto vec = fbb.CreateVector(data);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 5u);
    EXPECT_EQ(v->Get(0), 1);
    EXPECT_EQ(v->Get(4), 5);
  }
  {
    FlatBufferBuilder fbb;
    std::vector<int32_t> data;
    auto vec = fbb.CreateVector(data);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 0u);
  }
  {
    FlatBufferBuilder fbb;
    int32_t raw[] = {10, 20};
    auto vec = fbb.CreateVector(raw, 2);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 2u);
    EXPECT_EQ(v->Get(0), 10);
    EXPECT_EQ(v->Get(1), 20);
  }
  {
    FlatBufferBuilder fbb;
    auto vec = fbb.CreateVector<int32_t>({10, 20, 30});
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 3u);
    EXPECT_EQ(v->Get(0), 10);
    EXPECT_EQ(v->Get(2), 30);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorOfStringsTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorOfStringsTest, FromStrings)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfStrings");
  RecordProperty("Description", "CreateVectorOfStrings creates vector of string offsets from std::vector and iterators");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    std::vector<std::string> strs = {"alpha", "beta", "gamma"};
    auto vec = fbb.CreateVectorOfStrings(strs);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<Offset<String>>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 3u);
    EXPECT_STREQ(v->Get(0)->c_str(), "alpha");
    EXPECT_STREQ(v->Get(2)->c_str(), "gamma");
  }
  {
    FlatBufferBuilder fbb;
    std::vector<std::string> strs = {"one", "two", "three"};
    auto vec = fbb.CreateVectorOfStrings(strs.begin(), strs.end());
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<Offset<String>>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 3u);
    EXPECT_STREQ(v->Get(0)->c_str(), "one");
    EXPECT_STREQ(v->Get(2)->c_str(), "three");
  }
}

TEST(FlatBufferBuilderCreateVectorOfStringsTest, OfOffsetsRawArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVector(const Offset<T>*, size_t)");
  RecordProperty("Description", "CreateVector with raw array of Offset elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto s0 = fbb.CreateString("first");
  auto s1 = fbb.CreateString("second");
  Offset<String> offsets[2] = {s0, s1};
  auto vec = fbb.CreateVector(offsets, 2);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<Offset<String>>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 2u);
  EXPECT_STREQ(v->Get(0)->c_str(), "first");
  EXPECT_STREQ(v->Get(1)->c_str(), "second");
}

TEST(FlatBufferBuilderCreateVectorOfStringsTest, BoolVector)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVector(const std::vector<bool>&)");
  RecordProperty("Description", "vector<bool> serialises as vector of uint8_t");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  std::vector<bool> bools = {true, false, true, true, false};
  auto vec = fbb.CreateVector(bools);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<uint8_t>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 5u);
  EXPECT_EQ(v->Get(0), 1u);
  EXPECT_EQ(v->Get(1), 0u);
  EXPECT_EQ(v->Get(2), 1u);
  EXPECT_EQ(v->Get(3), 1u);
  EXPECT_EQ(v->Get(4), 0u);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorGeneratorTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorGeneratorTest, FuncAndStateful)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVector(size_t, std::function<T(size_t)>, CreateVector(size_t, F, S*))");
  RecordProperty("Description", "generator function and stateful generator function produce correct values");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto vec = fbb.CreateVector<int32_t>(5, [](size_t i) { return static_cast<int32_t>(i * i); });
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 5u);
    for (size_t i = 0; i < 5; i++)
    {
      EXPECT_EQ(v->Get(static_cast<uoffset_t>(i)), static_cast<int32_t>(i * i));
    }
  }
  {
    FlatBufferBuilder fbb;
    int base = 100;
    auto vec = fbb.CreateVector<int32_t>(4, [](size_t i, int* state) { return static_cast<int32_t>(*state + i); }, &base);
    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->size(), 4u);
    EXPECT_EQ(v->Get(0), 100);
    EXPECT_EQ(v->Get(3), 103);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderTableBuildTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderTableBuildTest, ScalarFields)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::StartTable, EndTable, AddElement");
  RecordProperty("Description", "building a table with scalar fields stores and reads back correctly");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  fbb.AddElement<int16_t>(6, -100, 0);
  fbb.AddElement<uint8_t>(8, 255, 0);
  fbb.AddElement<uint8_t>(10, 1, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);

  EXPECT_EQ(table->GetField<int32_t>(4, 0), 42);
  EXPECT_EQ(table->GetField<int16_t>(6, 0), static_cast<int16_t>(-100));
  EXPECT_EQ(table->GetField<uint8_t>(8, 0), 255);
  EXPECT_EQ(table->GetField<uint8_t>(10, 0), 1);
}

TEST(FlatBufferBuilderTableBuildTest, AddElementNoDefault)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::AddElement (two-argument overload)");
  RecordProperty("Description", "two-argument AddElement always stores the field regardless of value");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  EXPECT_TRUE(table->CheckField(4));
  EXPECT_EQ(table->GetField<int32_t>(4, 99), 0);
}

TEST(FlatBufferBuilderTableBuildTest, MultipleTables)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::StartTable, EndTable, AddOffset");
  RecordProperty("Description", "building multiple tables in the same buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;

  auto start1 = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 100, 0);
  auto sub1 = fbb.EndTable(start1);

  auto start2 = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 200, 0);
  auto sub2 = fbb.EndTable(start2);

  auto start_root = fbb.StartTable();
  fbb.AddOffset(4, Offset<>(sub1));
  fbb.AddOffset(6, Offset<>(sub2));
  auto root = fbb.EndTable(start_root);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* t1 = table->GetPointer<const Table*>(4);
  auto* t2 = table->GetPointer<const Table*>(6);
  ASSERT_NE(t1, nullptr);
  ASSERT_NE(t2, nullptr);
  EXPECT_EQ(t1->GetField<int32_t>(4, 0), 100);
  EXPECT_EQ(t2->GetField<int32_t>(4, 0), 200);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderForceDefaultsTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderForceDefaultsTest, ForceDefaults)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::ForceDefaults");
  RecordProperty("Description", "ForceDefaults controls whether default values are stored");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, 0, 0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_FALSE(table->CheckField(4));
  }
  {
    FlatBufferBuilder fbb;
    fbb.ForceDefaults(true);
    auto start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, 0, 0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_TRUE(table->CheckField(4));
    EXPECT_EQ(table->GetField<int32_t>(4, 99), 0);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderFinishTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderFinishTest, WithIdentifier)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::Finish");
  RecordProperty("Description", "Finish with a file identifier stores it in the buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root), "MYID");

  auto* buf = fbb.GetBufferPointer();
  EXPECT_TRUE(BufferHasIdentifier(buf, "MYID"));
  EXPECT_FALSE(BufferHasIdentifier(buf, "XXXX"));
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderFinishSizePrefixedTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderFinishSizePrefixedTest, Prefix)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::FinishSizePrefixed");
  RecordProperty("Description", "FinishSizePrefixed produces a size-prefixed buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  fbb.FinishSizePrefixed(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto size = fbb.GetSize();

  auto prefix = ReadScalar<uoffset_t>(buf);
  EXPECT_EQ(static_cast<size_t>(prefix + sizeof(uoffset_t)), size);

  auto* table = GetSizePrefixedRoot<Table>(buf);
  ASSERT_NE(table, nullptr);
  EXPECT_EQ(table->GetField<int32_t>(4, 0), 42);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderReleaseTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderReleaseTest, Reset)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::Release, ReleaseRaw, Reset, Clear");
  RecordProperty("Description", "Release / ReleaseRaw return buffer data; Reset / Clear for reuse");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto size_before = fbb.GetSize();
    EXPECT_GT(size_before, 0u);

    auto db = fbb.Release();
    EXPECT_EQ(db.size(), size_before);
    EXPECT_NE(db.data(), nullptr);
    EXPECT_EQ(fbb.GetSize(), 0u);
  }
  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    size_t size, offset;
    auto* raw = fbb.ReleaseRaw(size, offset);
    EXPECT_NE(raw, nullptr);
    EXPECT_GT(size, 0u);
    Deallocate(nullptr, raw, size);
  }
  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    EXPECT_GT(fbb.GetSize(), 0u);

    fbb.Clear();
    EXPECT_EQ(fbb.GetSize(), 0u);

    start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, 99, 0);
    root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_EQ(table->GetField<int32_t>(4, 0), 99);
  }
  {
    FlatBufferBuilder fbb(2048);
    auto start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, 42, 0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    EXPECT_GT(fbb.GetSize(), 0u);

    fbb.Reset();
    EXPECT_EQ(fbb.GetSize(), 0u);

    start = fbb.StartTable();
    fbb.AddElement<int32_t>(4, 77, 0);
    root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_EQ(table->GetField<int32_t>(4, 0), 77);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderMoveTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderMoveTest, ConstructAndAssign)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::FlatBufferBuilder(FlatBufferBuilder&&), operator=(FlatBufferBuilder&&)");
  RecordProperty("Description", "move constructor and move assignment transfer builder state");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto str = fbb.CreateString("test");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, str);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto size = fbb.GetSize();
  FlatBufferBuilder moved(std::move(fbb));
  EXPECT_EQ(moved.GetSize(), size);
}

TEST(FlatBufferBuilderMoveTest, Assign)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::operator=(FlatBufferBuilder&&)");
  RecordProperty("Description", "move assignment transfers builder state to another builder");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));
  auto size = fbb.GetSize();

  FlatBufferBuilder other;
  other = std::move(fbb);
  EXPECT_EQ(other.GetSize(), size);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderSwapTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderSwapTest, Exchanges)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::Swap");
  RecordProperty("Description", "Swap exchanges two builders' contents");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb1;
  {
    auto start = fbb1.StartTable();
    fbb1.AddElement<int32_t>(4, 1, 0);
    auto root = fbb1.EndTable(start);
    fbb1.Finish(Offset<Table>(root));
  }
  auto size1 = fbb1.GetSize();

  FlatBufferBuilder fbb2;
  {
    auto start = fbb2.StartTable();
    fbb2.AddElement<int32_t>(4, 2, 0);
    auto root = fbb2.EndTable(start);
    fbb2.Finish(Offset<Table>(root));
  }
  auto size2 = fbb2.GetSize();

  fbb1.Swap(fbb2);
  EXPECT_EQ(fbb1.GetSize(), size2);
  EXPECT_EQ(fbb2.GetSize(), size1);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderSharedStringTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderSharedStringTest, Deduplication)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateSharedString, CreateSharedStringStdString, CreateSharedStringFromStringPtr");
  RecordProperty("Description", "CreateSharedString deduplicates identical strings");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto s1 = fbb.CreateSharedString("duplicate");
    auto s2 = fbb.CreateSharedString("duplicate");
    auto s3 = fbb.CreateSharedString("different");
    EXPECT_EQ(s1.o, s2.o);
    EXPECT_NE(s1.o, s3.o);
  }
  {
    FlatBufferBuilder fbb;
    std::string s = "shared_std";
    auto off1 = fbb.CreateSharedString(s);
    auto off2 = fbb.CreateSharedString(s);
    EXPECT_EQ(off1.o, off2.o);
  }
  {
    FlatBufferBuilder fbb;
    auto off = fbb.CreateSharedString(static_cast<const String*>(nullptr));
    EXPECT_EQ(off.o, 0u);
  }
  {
    FlatBufferBuilder fbb;
    auto off1 = fbb.CreateSharedString("ptr_shared");
    auto start = fbb.StartTable();
    fbb.AddOffset(4, off1);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    const String* str_ptr = table->GetPointer<const String*>(4);
    ASSERT_NE(str_ptr, nullptr);
    EXPECT_STREQ(str_ptr->c_str(), "ptr_shared");
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderStructTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderStructTest, StartEndStruct)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::StartStruct, EndStruct");
  RecordProperty("Description", "struct alignment and offset tracking");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto off1 = fbb.StartStruct(8);
  EXPECT_EQ(off1, 0u);

  uint64_t val = 0x0102030405060708ULL;
  fbb.PushBytes(reinterpret_cast<const uint8_t*>(&val), sizeof(val));

  auto off2 = fbb.EndStruct();
  EXPECT_EQ(off2, static_cast<uoffset_t>(sizeof(val)));
  EXPECT_GT(off2, off1);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderAddOffset64Test
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderAddOffset64Test, Offset64)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::AddOffset (Offset64 overload)");
  RecordProperty("Description", "AddOffset with Offset64: null not stored, non-null stored");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder64 fbb;
    auto start = fbb.StartTable();
    Offset64<String> null_off{0};
    fbb.AddOffset(4, null_off);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_FALSE(table->CheckField(4));
  }
  {
    FlatBufferBuilder64 fbb;
    auto str = fbb.CreateString<Offset64>("test", 4);
    EXPECT_FALSE(str.IsNull());
    auto start = fbb.StartTable();
    fbb.AddOffset(4, str);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    EXPECT_GT(fbb.GetSize(), 0u);

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_TRUE(table->CheckField(4));
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderAddStructOffsetTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderAddStructOffsetTest, TracksOffset)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::AddStructOffset");
  RecordProperty("Description", "manually tracks a struct field at a given offset");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.Align(4);
  uint32_t val = 0x12345678;
  fbb.PushBytes(reinterpret_cast<const uint8_t*>(&val), sizeof(val));
  fbb.AddStructOffset(4, static_cast<uoffset_t>(fbb.GetSizeRelative32BitRegion()));
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  EXPECT_TRUE(table->CheckField(4));
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderReferToTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderReferToTest, UOffset64AndTemplated)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::ReferTo");
  RecordProperty("Description", "ReferTo computes relative offsets for uoffset64_t and templated overloads");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder64 fbb;
    const uint8_t zeros[16] = {};
    fbb.PushBytes(zeros, 16);
    auto result = fbb.ReferTo(static_cast<uoffset64_t>(8));
    EXPECT_EQ(result, static_cast<uoffset64_t>(16));
  }
  {
    FlatBufferBuilder fbb;
    auto r = fbb.ReferTo(static_cast<uoffset_t>(10), static_cast<uoffset_t>(20));
    EXPECT_EQ(r, 14u);
  }
  {
    FlatBufferBuilder fbb;
    auto r = fbb.ReferTo(static_cast<uoffset64_t>(5), static_cast<uoffset_t>(30));
    EXPECT_EQ(r, static_cast<uoffset64_t>(33));
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderEndTableDeprecatedTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderEndTableDeprecatedTest, Delegates)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::EndTable (deprecated two-argument overload)");
  RecordProperty("Description", "deprecated two-argument EndTable delegates to single-argument version");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start, 1);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  EXPECT_EQ(table->GetField<int32_t>(4, 0), 42);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderRequiredTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderRequiredTest, PresentField)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::Required");
  RecordProperty("Description", "Required does not assert when field is present");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto str = fbb.CreateString("required_field");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, str);
  auto root = fbb.EndTable(start);

  fbb.Required(Offset<Table>(root), 4);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* s = table->GetPointer<const String*>(4);
  ASSERT_NE(s, nullptr);
  EXPECT_STREQ(s->c_str(), "required_field");
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderFieldIndexToOffsetTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderFieldIndexToOffsetTest, VtableLayout)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FieldIndexToOffset");
  RecordProperty("Description", "field index 0 gives 4, field index n gives 4+n*2");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(FieldIndexToOffset(0), static_cast<voffset_t>(4));
  EXPECT_EQ(FieldIndexToOffset(1), static_cast<voffset_t>(6));
  EXPECT_EQ(FieldIndexToOffset(2), static_cast<voffset_t>(8));
  EXPECT_EQ(FieldIndexToOffset(10), static_cast<voffset_t>(24));
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderDedupVtablesTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderDedupVtablesTest, DisabledCreatesLargerBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::DedupVtables");
  RecordProperty("Description", "disabling dedup creates larger buffer for identical vtables");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb_dedup;
  {
    uoffset_t last = 0;
    for (int i = 0; i < 4; i++)
    {
      auto s = fbb_dedup.StartTable();
      fbb_dedup.AddElement<int32_t>(4, i + 1, 0);
      fbb_dedup.AddElement<int32_t>(6, i + 10, 0);
      fbb_dedup.AddElement<int32_t>(8, i + 100, 0);
      last = fbb_dedup.EndTable(s);
    }
    fbb_dedup.Finish(Offset<Table>(last));
  }
  auto size_dedup = fbb_dedup.GetSize();

  FlatBufferBuilder fbb_no_dedup;
  fbb_no_dedup.DedupVtables(false);
  {
    uoffset_t last = 0;
    for (int i = 0; i < 4; i++)
    {
      auto s = fbb_no_dedup.StartTable();
      fbb_no_dedup.AddElement<int32_t>(4, i + 1, 0);
      fbb_no_dedup.AddElement<int32_t>(6, i + 10, 0);
      fbb_no_dedup.AddElement<int32_t>(8, i + 100, 0);
      last = fbb_no_dedup.EndTable(s);
    }
    fbb_no_dedup.Finish(Offset<Table>(last));
  }
  auto size_no_dedup = fbb_no_dedup.GetSize();

  EXPECT_GT(size_no_dedup, size_dedup);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderPushFlatBufferTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderPushFlatBufferTest, PushesRawBytes)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::PushFlatBuffer");
  RecordProperty("Description", "PushFlatBuffer pushes raw bytes and marks buffer as finished");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder orig;
  auto start = orig.StartTable();
  orig.AddElement<int32_t>(4, 42, 0);
  auto root = orig.EndTable(start);
  orig.Finish(Offset<Table>(root));

  auto* orig_ptr = orig.GetBufferPointer();
  auto orig_size = orig.GetSize();

  FlatBufferBuilder fbb;
  fbb.PushFlatBuffer(orig_ptr, orig_size);

  auto* buf = fbb.GetBufferPointer();
  EXPECT_NE(buf, nullptr);
  EXPECT_EQ(fbb.GetSize(), orig_size);

  auto* table = GetRoot<Table>(buf);
  EXPECT_EQ(table->GetField<int32_t>(4, 0), 42);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderDataFreeFunctionTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderDataFreeFunctionTest, VectorOverloads)
{
  RecordProperty("FullyVerifies", "::flatbuffers::data (std::vector overloads)");
  RecordProperty("Description", "data returns &front() for non-empty, non-null for empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    std::vector<int32_t> v = {10, 20, 30};
    int32_t* ptr = data(v);
    EXPECT_EQ(ptr, &v.front());
    *ptr = 99;
    EXPECT_EQ(v[0], 99);
  }
  {
    const std::vector<int32_t> cv = {1, 2, 3};
    const int32_t* cptr = data(cv);
    EXPECT_EQ(cptr, &cv.front());
  }
  {
    std::vector<int32_t> empty;
    int32_t* ptr = data(empty);
    EXPECT_NE(ptr, nullptr);
  }
  {
    const std::vector<int32_t> empty;
    const int32_t* cptr = data(empty);
    EXPECT_NE(cptr, nullptr);
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderBufferSpanTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderBufferSpanTest, MatchesBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::GetBufferSpan");
  RecordProperty("Description", "GetBufferSpan returns span matching buffer pointer and size");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto span = fbb.GetBufferSpan();
  EXPECT_EQ(span.data(), fbb.GetBufferPointer());
  EXPECT_EQ(span.size(), fbb.GetSize());
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderGetCurrentBufferPointerTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderGetCurrentBufferPointerTest, BeforeAndAfterFinish)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::GetCurrentBufferPointer");
  RecordProperty("Description", "GetCurrentBufferPointer available before Finish, equals GetBufferPointer after");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);

  uint8_t* ptr_before = fbb.GetCurrentBufferPointer();
  EXPECT_NE(ptr_before, nullptr);

  fbb.Finish(Offset<Table>(root));
  EXPECT_EQ(fbb.GetCurrentBufferPointer(), fbb.GetBufferPointer());
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderBufferMinAlignmentTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderBufferMinAlignmentTest, ReflectsLargestScalar)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::GetBufferMinAlignment");
  RecordProperty("Description", "alignment reflects largest scalar written");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    fbb.AddElement<uint8_t>(4, 1, 0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    EXPECT_GE(fbb.GetBufferMinAlignment(), 1u);
  }
  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    fbb.AddElement<double>(4, 3.14, 0.0);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));
    EXPECT_GE(fbb.GetBufferMinAlignment(), sizeof(double));
  }
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderTemporaryPointersTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderTemporaryPointersTest, GetTemporaryPointer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::GetTemporaryPointer, ::flatbuffers::GetMutableTemporaryPointer");
  RecordProperty("Description", "temporary pointer functions return typed pointer into in-progress buffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto table_off = fbb.EndTable(start);
  Offset<Table> off(table_off);

  Table* mutable_ptr = GetMutableTemporaryPointer(fbb, off);
  ASSERT_NE(mutable_ptr, nullptr);
  EXPECT_EQ(mutable_ptr->GetField<int32_t>(4, 0), 42);

  const Table* const_ptr = GetTemporaryPointer(static_cast<const FlatBufferBuilder&>(fbb), off);
  ASSERT_NE(const_ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<const uint8_t*>(mutable_ptr), reinterpret_cast<const uint8_t*>(const_ptr));
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderSwapBufAllocatorTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderSwapBufAllocatorTest, PreservesData)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::SwapBufAllocator");
  RecordProperty("Description", "swapping allocators between builders preserves buffer data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb1;
  auto start = fbb1.StartTable();
  fbb1.AddElement<int32_t>(4, 77, 0);
  auto root = fbb1.EndTable(start);
  fbb1.Finish(Offset<Table>(root));
  auto size1 = fbb1.GetSize();

  FlatBufferBuilder fbb2;
  fbb1.SwapBufAllocator(fbb2);

  EXPECT_EQ(fbb1.GetSize(), size1);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderFileIdentifierLengthTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderFileIdentifierLengthTest, ConstantEqualsFour)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::kFileIdentifierLength");
  RecordProperty("Description", "kFileIdentifierLength equals 4");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(FlatBufferBuilder::kFileIdentifierLength + 0u, 4u);
  EXPECT_EQ(FlatBufferBuilder64::kFileIdentifierLength + 0u, 4u);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVector64Test
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVector64Test, Produces64BitVector)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder64::CreateVector64");
  RecordProperty("Description", "CreateVector64 produces a 64-bit offset vector");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder64 fbb;
  std::vector<float> vals = {1.0f, 2.0f, 3.0f};
  auto vec = fbb.CreateVector64(vals);
  EXPECT_FALSE(vec.IsNull());
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderReleaseOwnedAllocatorTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderReleaseOwnedAllocatorTest, DetachedBufferGetsAllocator)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::Release");
  RecordProperty("Description", "Release with owned allocator transfers ownership to DetachedBuffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(1024, new DefaultAllocator(), true);
  auto start = fbb.StartTable();
  fbb.AddElement<int32_t>(4, 42, 0);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  const auto size_before = fbb.GetSize();
  auto db = fbb.Release();
  EXPECT_EQ(db.size(), size_before);
  EXPECT_NE(db.data(), nullptr);
  EXPECT_EQ(fbb.GetSize(), 0u);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderForceVectorAlignmentTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderForceVectorAlignmentTest, AcceptsAlignment)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::ForceVectorAlignment");
  RecordProperty("Description", "ForceVectorAlignment specifies alignment for vectors");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  fbb.ForceVectorAlignment(4, sizeof(uint8_t), 16);
  auto vec = fbb.CreateVector<uint8_t>({1, 2, 3, 4});
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<uint8_t>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 4u);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderForceStringAlignmentTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderForceStringAlignmentTest, AcceptsAlignment)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::ForceStringAlignment");
  RecordProperty("Description", "ForceStringAlignment specifies alignment for strings");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  fbb.ForceStringAlignment(5, 8);
  auto s = fbb.CreateString("align_me");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, s);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* str = table->GetPointer<const String*>(4);
  ASSERT_NE(str, nullptr);
  EXPECT_STREQ(str->c_str(), "align_me");
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorOfStructsTest
// ---------------------------------------------------------------------------

#pragma pack(push, 1)
struct Point2D FLATBUFFERS_FINAL_CLASS
{
  int32_t x_;
  int32_t y_;

 public:
  Point2D() : x_(0), y_(0) {}
  Point2D(int32_t x, int32_t y) : x_(EndianScalar(x)), y_(EndianScalar(y)) {}
  int32_t x() const
  {
    return EndianScalar(x_);
  }
  int32_t y() const
  {
    return EndianScalar(y_);
  }
  bool KeyCompareLessThan(const Point2D* other) const
  {
    return x() < other->x();
  }
};
#pragma pack(pop)
static_assert(sizeof(Point2D) == 8, "Point2D must be 8 bytes");

struct NativePoint
{
  int32_t x;
  int32_t y;
};

static Point2D PackNativePoint(const NativePoint& src)
{
  return Point2D(src.x, src.y);
}

TEST(FlatBufferBuilderCreateVectorOfStructsTest, RawArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfStructs(const T*, size_t)");
  RecordProperty("Description", "CreateVectorOfStructs with raw struct array");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  Point2D pts[3] = {Point2D(1, 2), Point2D(3, 4), Point2D(5, 6)};
  auto vec = fbb.CreateVectorOfStructs(pts, 3);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 3u);
  EXPECT_EQ(v->Get(0)->x(), 1);
  EXPECT_EQ(v->Get(2)->x(), 5);
}

TEST(FlatBufferBuilderCreateVectorOfStructsTest, StdVector)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfStructs(const std::vector<T>&)");
  RecordProperty("Description", "CreateVectorOfStructs with std::vector of structs");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  std::vector<Point2D> pts = {Point2D(10, 20), Point2D(30, 40)};
  auto vec = fbb.CreateVectorOfStructs(pts);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 2u);
  EXPECT_EQ(v->Get(0)->x(), 10);
  EXPECT_EQ(v->Get(1)->x(), 30);
}

TEST(FlatBufferBuilderCreateVectorOfStructsTest, FillerFunc)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfStructs(size_t, std::function<void(size_t, T*)>)");
  RecordProperty("Description", "CreateVectorOfStructs with filler function");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto vec = fbb.CreateVectorOfStructs<Point2D>(3, [](size_t i, Point2D* pt) { *pt = Point2D(static_cast<int32_t>(i), static_cast<int32_t>(i * 2)); });
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 3u);
  EXPECT_EQ(v->Get(0)->x(), 0);
  EXPECT_EQ(v->Get(2)->x(), 2);
}

TEST(FlatBufferBuilderCreateVectorOfStructsTest, StatefulFiller)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfStructs(size_t, F, S*)");
  RecordProperty("Description", "CreateVectorOfStructs with stateful filler function");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  int32_t base = 5;
  auto vec = fbb.CreateVectorOfStructs<Point2D>(3, [](size_t i, Point2D* pt, int32_t* b) { *pt = Point2D(static_cast<int32_t>(i) + *b, 0); }, &base);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 3u);
  EXPECT_EQ(v->Get(0)->x(), 5);
  EXPECT_EQ(v->Get(2)->x(), 7);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorOfNativeStructsTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorOfNativeStructsTest, WithPackFunc)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfNativeStructs(const S*, size_t, pack_func)");
  RecordProperty("Description", "CreateVectorOfNativeStructs with pack function");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  NativePoint pts[3] = {{1, 2}, {3, 4}, {5, 6}};
  auto vec = fbb.CreateVectorOfNativeStructs<Point2D>(pts, 3, PackNativePoint);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 3u);
  EXPECT_EQ(v->Get(2)->y(), 6);
}

TEST(FlatBufferBuilderCreateVectorOfNativeStructsTest, StdVectorWithPackFunc)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfNativeStructs(const std::vector<S>&, pack_func)");
  RecordProperty("Description", "CreateVectorOfNativeStructs with std::vector and pack function");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  std::vector<NativePoint> pts = {{10, 20}, {30, 40}};
  auto vec = fbb.CreateVectorOfNativeStructs<Point2D>(pts, PackNativePoint);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 2u);
  EXPECT_EQ(v->Get(1)->x(), 30);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorOfSortedStructsTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorOfSortedStructsTest, RawArray)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfSortedStructs(T*, size_t)");
  RecordProperty("Description", "CreateVectorOfSortedStructs sorts structs by key before serialisation");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  Point2D pts[4] = {Point2D(40, 0), Point2D(10, 0), Point2D(30, 0), Point2D(20, 0)};
  auto vec = fbb.CreateVectorOfSortedStructs(pts, 4);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 4u);
  EXPECT_EQ(v->Get(0)->x(), 10);
  EXPECT_EQ(v->Get(3)->x(), 40);
}

TEST(FlatBufferBuilderCreateVectorOfSortedStructsTest, StdVector)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorOfSortedStructs(std::vector<T>*)");
  RecordProperty("Description", "CreateVectorOfSortedStructs modifies vector in-place");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  std::vector<Point2D> pts = {Point2D(5, 0), Point2D(2, 0), Point2D(8, 0)};
  auto vec = fbb.CreateVectorOfSortedStructs(&pts);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 3u);
  EXPECT_EQ(v->Get(1)->x(), 5);
  EXPECT_EQ(v->Get(2)->x(), 8);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateUninitializedVectorTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateUninitializedVectorTest, RawAndTyped)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateUninitializedVector");
  RecordProperty("Description", "CreateUninitializedVector reserves space for manual filling");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    uint8_t* data_ptr = nullptr;
    auto vec_offset = fbb.CreateUninitializedVector(4, sizeof(int32_t), alignof(int32_t), &data_ptr);
    ASSERT_NE(data_ptr, nullptr);
    auto* typed = reinterpret_cast<int32_t*>(data_ptr);
    typed[0] = 10; typed[1] = 20; typed[2] = 30; typed[3] = 40;

    auto start = fbb.StartTable();
    fbb.AddOffset(4, Offset<Vector<int32_t>>(vec_offset));
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<int32_t>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_EQ(v->Get(3), 40);
  }
  {
    FlatBufferBuilder fbb;
    float* data_ptr = nullptr;
    auto vec = fbb.CreateUninitializedVector(3u, &data_ptr);
    ASSERT_NE(data_ptr, nullptr);
    data_ptr[0] = 1.0f; data_ptr[1] = 2.0f; data_ptr[2] = 3.0f;

    auto start = fbb.StartTable();
    fbb.AddOffset(4, vec);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    auto* v = table->GetPointer<const Vector<float>*>(4);
    ASSERT_NE(v, nullptr);
    EXPECT_FLOAT_EQ(v->Get(2), 3.0f);
  }
}

TEST(FlatBufferBuilderCreateUninitializedVectorOfStructsTest, Works)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateUninitializedVectorOfStructs");
  RecordProperty("Description", "CreateUninitializedVectorOfStructs reserves space for struct filling");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  Point2D* data_ptr = nullptr;
  auto vec = fbb.CreateUninitializedVectorOfStructs(2u, &data_ptr);
  ASSERT_NE(data_ptr, nullptr);
  data_ptr[0] = Point2D(7, 8);
  data_ptr[1] = Point2D(9, 10);

  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<const Point2D*>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->Get(0)->x(), 7);
  EXPECT_EQ(v->Get(1)->y(), 10);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateVectorScalarCastTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateVectorScalarCastTest, Casts)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateVectorScalarCast<T, U>");
  RecordProperty("Description", "CreateVectorScalarCast casts each element from U to T");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  int8_t src[] = {1, -1, 127, -128};
  auto vec = fbb.CreateVectorScalarCast<int32_t>(src, 4);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->Get(0), 1);
  EXPECT_EQ(v->Get(1), -1);
  EXPECT_EQ(v->Get(3), -128);
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderCreateStructTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderCreateStructTest, Standalone)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::CreateStruct<T>");
  RecordProperty("Description", "CreateStruct writes a standalone struct");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  Point2D p(42, 99);
  auto off = fbb.CreateStruct(p);
  EXPECT_FALSE(off.IsNull());
}

// ---------------------------------------------------------------------------
// FlatBufferBuilderAddStructTest
// ---------------------------------------------------------------------------

TEST(FlatBufferBuilderAddStructTest, NullAndNonNull)
{
  RecordProperty("FullyVerifies", "::flatbuffers::FlatBufferBuilder::AddStruct<T>");
  RecordProperty("Description", "AddStruct: null pointer not stored, non-null stored");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  {
    FlatBufferBuilder fbb;
    auto start = fbb.StartTable();
    fbb.AddStruct<Point2D>(4, nullptr);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_FALSE(table->CheckField(4));
  }
  {
    FlatBufferBuilder fbb;
    Point2D pt(3, 7);
    auto start = fbb.StartTable();
    fbb.AddStruct(4, &pt);
    auto root = fbb.EndTable(start);
    fbb.Finish(Offset<Table>(root));

    auto* buf = fbb.GetBufferPointer();
    auto* table = GetRoot<Table>(buf);
    EXPECT_TRUE(table->CheckField(4));
    const Point2D* result = table->GetStruct<const Point2D*>(4);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->x(), 3);
    EXPECT_EQ(result->y(), 7);
  }
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score