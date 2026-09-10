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
#include "flatbuffers/vector.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <numeric>
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

// ---------------------------------------------------------------------------
// VectorGetTest
// ---------------------------------------------------------------------------

TEST(VectorGetTest, Scalars)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::Get, operator[], size, empty");
  RecordProperty("Description", "Get, operator[] access elements; size/empty report correct state");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {10, 20, 30, 40, 50};
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
  EXPECT_FALSE(v->empty());
  EXPECT_EQ(v->Get(0), 10);
  EXPECT_EQ(v->Get(4), 50);
  EXPECT_EQ(v->Get(2), 30);
  EXPECT_EQ((*v)[0], 10);
  EXPECT_EQ((*v)[4], 50);
}

// ---------------------------------------------------------------------------
// VectorEmptyTest
// ---------------------------------------------------------------------------

TEST(VectorEmptyTest, ZeroElements)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::size, empty");
  RecordProperty("Description", "empty vector has size 0 and empty() is true");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
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
  EXPECT_TRUE(v->empty());
}

// ---------------------------------------------------------------------------
// VectorLengthTest
// ---------------------------------------------------------------------------

TEST(VectorLengthTest, NullAndNotNull)
{
  RecordProperty("FullyVerifies", "::flatbuffers::VectorLength<T>");
  RecordProperty("Description", "null vector returns 0, non-null returns actual size");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  EXPECT_EQ(VectorLength<int32_t>(nullptr), 0u);

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {1, 2, 3};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  EXPECT_EQ(VectorLength(v), 3u);
}

// ---------------------------------------------------------------------------
// VectorMutateTest
// ---------------------------------------------------------------------------

TEST(VectorMutateTest, InPlace)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::Mutate");
  RecordProperty("Description", "in-place mutation of vector elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {100, 200, 300};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetMutableRoot<Table>(buf);
  auto* v = table->GetPointer<Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);

  v->Mutate(0, 999);
  EXPECT_EQ(v->Get(0), 999);
  v->Mutate(2, 0);
  EXPECT_EQ(v->Get(2), 0);
  v->Mutate(1, std::numeric_limits<int32_t>::max());
  EXPECT_EQ(v->Get(1), std::numeric_limits<int32_t>::max());
}

// ---------------------------------------------------------------------------
// VectorForwardIteratorTest
// ---------------------------------------------------------------------------

TEST(VectorForwardIteratorTest, RangeBased)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::begin, ::flatbuffers::Vector<T>::end");
  RecordProperty("Description", "forward iteration collects elements in order");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {5, 10, 15, 20};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);

  std::vector<int32_t> collected;
  for (auto it = v->begin(); it != v->end(); ++it)
  {
    collected.push_back(*it);
  }
  ASSERT_EQ(collected.size(), 4u);
  EXPECT_EQ(collected[0], 5);
  EXPECT_EQ(collected[3], 20);
}

// ---------------------------------------------------------------------------
// VectorReverseIteratorTest
// ---------------------------------------------------------------------------

TEST(VectorReverseIteratorTest, RangeBased)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::rbegin, ::flatbuffers::Vector<T>::rend");
  RecordProperty("Description", "reverse iteration collects elements in reverse order");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {1, 2, 3};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);

  std::vector<int32_t> rev;
  for (auto it = v->rbegin(); it != v->rend(); ++it)
  {
    rev.push_back(*it);
  }
  ASSERT_EQ(rev.size(), 3u);
  EXPECT_EQ(rev[0], 3);
  EXPECT_EQ(rev[2], 1);
}

// ---------------------------------------------------------------------------
// VectorIteratorArithmeticTest
// ---------------------------------------------------------------------------

TEST(VectorIteratorArithmeticTest, Operators)
{
  RecordProperty("FullyVerifies", "VectorIterator arithmetic");
  RecordProperty("Description", "iterator arithmetic for subtraction, addition, comparison");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {10, 20, 30, 40};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);

  auto b = v->begin();
  auto e = v->end();

  EXPECT_EQ(e - b, 4);
  auto it = b + 2;
  EXPECT_EQ(*it, 30);
  EXPECT_LT(b, e);
  EXPECT_LE(b, b);
  EXPECT_GE(e, b);
  EXPECT_FALSE(b > e);
}

// ---------------------------------------------------------------------------
// VectorMakeSpanTest
// ---------------------------------------------------------------------------

TEST(VectorMakeSpanTest, SpanOverData)
{
  RecordProperty("FullyVerifies", "::flatbuffers::make_span(const Vector<T>&)");
  RecordProperty("Description", "make_span creates a span over vector data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  std::vector<uint8_t> data = {0xAA, 0xBB, 0xCC};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<uint8_t>*>(4);
  ASSERT_NE(v, nullptr);

  auto s = make_span(*v);
  EXPECT_EQ(s.size(), 3u);
  EXPECT_EQ(s[0], 0xAA);
  EXPECT_EQ(s[2], 0xCC);
}

// ---------------------------------------------------------------------------
// VectorOfStringsTest
// ---------------------------------------------------------------------------

TEST(VectorOfStringsTest, Offsets)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<Offset<String>>::Get");
  RecordProperty("Description", "vector of Offset<String> elements");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto s1 = fbb.CreateString("alpha");
  auto s2 = fbb.CreateString("beta");
  auto s3 = fbb.CreateString("gamma");
  std::vector<Offset<String>> str_offsets = {s1, s2, s3};
  auto vec = fbb.CreateVector(str_offsets);
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

// ---------------------------------------------------------------------------
// VectorSingleElementTest
// ---------------------------------------------------------------------------

TEST(VectorSingleElementTest, OneElement)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Vector<T>::size, Get");
  RecordProperty("Description", "vector with exactly 1 element");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb(256);
  std::vector<int32_t> data = {42};
  auto vec = fbb.CreateVector(data);
  auto start = fbb.StartTable();
  fbb.AddOffset(4, vec);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  auto* buf = fbb.GetBufferPointer();
  auto* table = GetRoot<Table>(buf);
  auto* v = table->GetPointer<const Vector<int32_t>*>(4);
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(v->size(), 1u);
  EXPECT_EQ(v->Get(0), 42);
  EXPECT_FALSE(v->empty());
  EXPECT_EQ(v->end() - v->begin(), 1);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score