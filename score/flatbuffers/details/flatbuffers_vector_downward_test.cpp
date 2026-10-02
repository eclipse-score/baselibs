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
#include "flatbuffers/vector_downward.h"

#include <cstdint>
#include <cstring>

#include "flatbuffers/default_allocator.h"
#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// ---------------------------------------------------------------------------
// VectorDownwardDefaultConstructTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardDefaultConstructTest, Empty)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::vector_downward");
  RecordProperty("Description", "freshly constructed vector_downward is empty");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(256, nullptr, false, 1);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
}

// ---------------------------------------------------------------------------
// VectorDownwardPushTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardPushTest, Bytes)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::push");
  RecordProperty("Description", "pushing bytes increases size");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  vector_downward<> vd(64, nullptr, false, 1);

  uint8_t dummy = 0x42;
  vd.push(&dummy, 0);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));

  vd.push(&dummy, 1);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(1));
  EXPECT_EQ(vd.data()[0], 0x42);

  uint8_t data4[] = {0x01, 0x02, 0x03, 0x04};
  vd.push(data4, 4);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(5));
  EXPECT_EQ(vd.data()[0], 0x01);
  EXPECT_EQ(vd.data()[4], 0x42);
}

// ---------------------------------------------------------------------------
// VectorDownwardPushSmallTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardPushSmallTest, Scalar)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::push_small");
  RecordProperty("Description", "push_small for scalar types");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint32_t val = EndianScalar(static_cast<uint32_t>(0xDEADBEEF));
  vd.push_small(val);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(4));
  EXPECT_EQ(ReadScalar<uint32_t>(vd.data()), static_cast<uint32_t>(0xDEADBEEF));
}

// ---------------------------------------------------------------------------
// VectorDownwardFillTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardFillTest, Zeros)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::fill");
  RecordProperty("Description", "fill writes zero bytes");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);

  vd.fill(0);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));

  vd.fill(1);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(1));
  EXPECT_EQ(vd.data()[0], 0);

  vd.fill(4);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(5));
  for (int i = 0; i < 4; ++i)
  {
    EXPECT_EQ(vd.data()[i], 0);
  }
}

// ---------------------------------------------------------------------------
// VectorDownwardPopTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardPopTest, RemoveBytes)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::pop");
  RecordProperty("Description", "pop removes bytes from the front");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {1, 2, 3, 4};
  vd.push(data, 4);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(4));

  vd.pop(2);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(2));

  vd.pop(0);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(2));

  vd.pop(2);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
}

// ---------------------------------------------------------------------------
// VectorDownwardClearTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardClearTest, PreservesCapacity)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::clear");
  RecordProperty("Description", "clear resets size but preserves capacity");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {1, 2, 3};
  vd.push(data, 3);
  EXPECT_GT(vd.size(), static_cast<uoffset_t>(0));
  auto cap_before = vd.capacity();

  vd.clear();
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
  EXPECT_EQ(vd.capacity(), cap_before);
}

// ---------------------------------------------------------------------------
// VectorDownwardResetTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardResetTest, Deallocates)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::reset");
  RecordProperty("Description", "reset clears buffer AND deallocates");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {1, 2, 3};
  vd.push(data, 3);

  vd.reset();
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
  EXPECT_EQ(vd.capacity(), 0u);
}

// ---------------------------------------------------------------------------
// VectorDownwardReleaseTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardReleaseTest, DetachedBuffer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::release");
  RecordProperty("Description", "release returns a DetachedBuffer with the data");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {0xAA, 0xBB};
  vd.push(data, 2);

  auto db = vd.release();
  EXPECT_EQ(db.size(), 2u);
  EXPECT_EQ(db.data()[0], 0xAA);
  EXPECT_EQ(db.data()[1], 0xBB);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
}

// ---------------------------------------------------------------------------
// VectorDownwardReleaseRawTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardReleaseRawTest, RawPointer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::release_raw, ::flatbuffers::Deallocate");
  RecordProperty("Description", "release_raw returns raw pointer and clears");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {1, 2, 3, 4, 5};
  vd.push(data, 5);

  size_t allocated, offset;
  uint8_t* raw = vd.release_raw(allocated, offset);
  EXPECT_NE(raw, nullptr);
  EXPECT_GT(allocated, 0u);
  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(0));
  EXPECT_EQ(raw[offset + 0], 1);
  EXPECT_EQ(raw[offset + 1], 2);

  Deallocate(nullptr, raw, allocated);
}

// ---------------------------------------------------------------------------
// VectorDownwardMoveConstructTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardMoveConstructTest, TransfersOwnership)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::vector_downward(vector_downward&&)");
  RecordProperty("Description", "move constructor transfers ownership");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {10, 20};
  vd.push(data, 2);

  vector_downward<> moved(std::move(vd));
  EXPECT_EQ(moved.size(), static_cast<uoffset_t>(2));
  EXPECT_EQ(moved.data()[0], 10);
  EXPECT_EQ(moved.data()[1], 20);
}

// ---------------------------------------------------------------------------
// VectorDownwardMoveAssignTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardMoveAssignTest, TransfersOwnership)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::operator=(vector_downward&&)");
  RecordProperty("Description", "move assignment transfers ownership");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {10, 20};
  vd.push(data, 2);

  vector_downward<> other(64, nullptr, false, 1);
  other = std::move(vd);
  EXPECT_EQ(other.size(), static_cast<uoffset_t>(2));
}

// ---------------------------------------------------------------------------
// VectorDownwardSwapTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardSwapTest, Exchanges)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::swap");
  RecordProperty("Description", "swap exchanges contents of two vectors");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> a(64, nullptr, false, 1);
  vector_downward<> b(64, nullptr, false, 1);

  uint8_t da[] = {1, 2, 3};
  a.push(da, 3);
  uint8_t db[] = {4, 5};
  b.push(db, 2);

  a.swap(b);
  EXPECT_EQ(a.size(), static_cast<uoffset_t>(2));
  EXPECT_EQ(b.size(), static_cast<uoffset_t>(3));
  EXPECT_EQ(a.data()[0], 4);
  EXPECT_EQ(b.data()[0], 1);
}

// ---------------------------------------------------------------------------
// VectorDownwardScratchTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardScratchTest, PushPopSize)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::scratch_push_small, scratch_size, scratch_pop");
  RecordProperty("Description", "scratch memory push/pop/size work correctly");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  vector_downward<> vd(64, nullptr, false, 1);
  uint8_t data[] = {0xFF};
  vd.push(data, 1);

  EXPECT_EQ(vd.scratch_size(), static_cast<uoffset_t>(0));

  vd.scratch_push_small(static_cast<uint32_t>(42));
  EXPECT_EQ(vd.scratch_size(), static_cast<uoffset_t>(4));

  vd.scratch_pop(4);
  EXPECT_EQ(vd.scratch_size(), static_cast<uoffset_t>(0));
}

// ---------------------------------------------------------------------------
// VectorDownwardGrowthTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardGrowthTest, Reallocates)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::push");
  RecordProperty("Description", "pushing more than initial_size forces reallocation");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  vector_downward<> vd(8, nullptr, false, 1);
  uint8_t data[16];
  memset(data, 0xAB, sizeof(data));
  vd.push(data, sizeof(data));

  EXPECT_EQ(vd.size(), static_cast<uoffset_t>(16));
  EXPECT_GE(vd.capacity(), 16u);
  for (int i = 0; i < 16; ++i)
  {
    EXPECT_EQ(vd.data()[i], 0xAB);
  }
}

// ---------------------------------------------------------------------------
// VectorDownwardClearAllocatorOwnedTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardClearAllocatorOwnedTest, DeletesAllocator)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::clear_allocator");
  RecordProperty("Description", "when own_allocator_ is true, destruction path deletes allocator");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto* alloc = new DefaultAllocator();
  {
    vector_downward<> vd(64, alloc, true, 1);
    uint8_t data[] = {1, 2, 3};
    vd.push(data, 3);
    // ~vector_downward() -> reset() -> clear_allocator() -> delete alloc
  }
}

// ---------------------------------------------------------------------------
// VectorDownwardReleaseOwnedAllocatorTest
// ---------------------------------------------------------------------------

TEST(VectorDownwardReleaseOwnedAllocatorTest, TransfersOwnership)
{
  RecordProperty("FullyVerifies", "::flatbuffers::vector_downward::release");
  RecordProperty("Description", "release transfers owned allocator to returned DetachedBuffer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  auto* alloc = new DefaultAllocator();
  vector_downward<> vd(64, alloc, true, 1);
  uint8_t data[] = {0xAA, 0xBB};
  vd.push(data, 2);

  auto db = vd.release();
  EXPECT_EQ(db.size(), 2u);
  EXPECT_EQ(db.data()[0], 0xAA);
  EXPECT_EQ(db.data()[1], 0xBB);
  // ~DetachedBuffer() will delete alloc via the owned-allocator path.
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score