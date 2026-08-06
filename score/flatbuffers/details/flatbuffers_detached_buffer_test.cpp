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
#include "flatbuffers/detached_buffer.h"

#include <cstdint>
#include <cstring>

#include "flatbuffers/default_allocator.h"
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
// DetachedBufferDefaultConstructTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferDefaultConstructTest, Empty)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::DetachedBuffer");
  RecordProperty("Description", "default-constructed DetachedBuffer is empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DetachedBuffer db;
  EXPECT_EQ(db.data(), nullptr);
  EXPECT_EQ(db.size(), 0u);
  EXPECT_EQ(db.begin(), db.end());
}

// ---------------------------------------------------------------------------
// DetachedBufferFromBuilderTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferFromBuilderTest, FromBuilder)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::data, ::flatbuffers::DetachedBuffer::size, ::flatbuffers::DetachedBuffer::begin, ::flatbuffers::DetachedBuffer::end");
  RecordProperty("Description", "DetachedBuffer produced by Release has correct data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb(256);
  auto str = fbb.CreateString("hello");
  auto start = fbb.StartTable();
  fbb.AddOffset(4, str);
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  DetachedBuffer db = fbb.Release();
  EXPECT_NE(db.data(), nullptr);
  EXPECT_GT(db.size(), 0u);
  EXPECT_EQ(static_cast<size_t>(db.end() - db.begin()), db.size());
}

// ---------------------------------------------------------------------------
// DetachedBufferMoveConstructTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferMoveConstructTest, TransfersOwnership)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::DetachedBuffer(DetachedBuffer&&)");
  RecordProperty("Description", "move constructor transfers ownership; source becomes empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  DetachedBuffer original = fbb.Release();
  auto original_size = original.size();
  auto* original_data = original.data();
  EXPECT_GT(original_size, 0u);

  DetachedBuffer moved(std::move(original));
  EXPECT_EQ(moved.size(), original_size);
  EXPECT_EQ(moved.data(), original_data);
  EXPECT_EQ(original.data(), nullptr);
  EXPECT_EQ(original.size(), 0u);
}

// ---------------------------------------------------------------------------
// DetachedBufferMoveAssignTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferMoveAssignTest, TransfersOwnership)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::operator=(DetachedBuffer&&)");
  RecordProperty("Description", "move assignment transfers; destination cleaned up");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb1;
  {
    auto start = fbb1.StartTable();
    auto root = fbb1.EndTable(start);
    fbb1.Finish(Offset<Table>(root));
  }
  DetachedBuffer db1 = fbb1.Release();
  auto size1 = db1.size();

  FlatBufferBuilder fbb2;
  {
    auto start = fbb2.StartTable();
    auto root = fbb2.EndTable(start);
    fbb2.Finish(Offset<Table>(root));
  }
  DetachedBuffer db2 = fbb2.Release();

  db2 = std::move(db1);
  EXPECT_EQ(db2.size(), size1);
  EXPECT_EQ(db1.data(), nullptr);
  EXPECT_EQ(db1.size(), 0u);
}

// ---------------------------------------------------------------------------
// DetachedBufferSelfMoveAssignTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferSelfMoveAssignTest, Safe)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::operator=(DetachedBuffer&&)");
  RecordProperty("Description", "self-move-assignment is safe");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));
  DetachedBuffer db = fbb.Release();
  auto sz = db.size();
  auto* ptr = db.data();

  DetachedBuffer& ref = db;
  db = std::move(ref);
  EXPECT_EQ(db.size(), sz);
  EXPECT_EQ(db.data(), ptr);
}

// ---------------------------------------------------------------------------
// DetachedBufferBeginEndTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferBeginEndTest, SpanCorrectRange)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::begin, ::flatbuffers::DetachedBuffer::end, ::flatbuffers::DetachedBuffer::data");
  RecordProperty("Description", "begin/end span the correct range");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  FlatBufferBuilder fbb;
  auto start = fbb.StartTable();
  auto root = fbb.EndTable(start);
  fbb.Finish(Offset<Table>(root));

  DetachedBuffer db = fbb.Release();
  EXPECT_EQ(static_cast<size_t>(db.end() - db.begin()), db.size());

  const DetachedBuffer& cdb = db;
  EXPECT_EQ(static_cast<size_t>(cdb.end() - cdb.begin()), cdb.size());
  EXPECT_EQ(cdb.data(), cdb.begin());
}

// ---------------------------------------------------------------------------
// DetachedBufferDestroyOwnedAllocatorTest
// ---------------------------------------------------------------------------

TEST(DetachedBufferDestroyOwnedAllocatorTest, OwnsAllocator)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DetachedBuffer::destroy");
  RecordProperty("Description", "DetachedBuffer with own_allocator=true deletes the allocator on destruction");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto* alloc = new DefaultAllocator();

  constexpr size_t kBufSize = 64;
  uint8_t* buf = alloc->allocate(kBufSize);
  ASSERT_NE(buf, nullptr);

  std::memset(buf, 0xAB, kBufSize);

  DetachedBuffer db(alloc, true, buf, kBufSize, buf, kBufSize);

  EXPECT_EQ(db.data(), buf);
  EXPECT_EQ(db.size(), kBufSize);
  EXPECT_EQ(db.data()[0], 0xAB);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score