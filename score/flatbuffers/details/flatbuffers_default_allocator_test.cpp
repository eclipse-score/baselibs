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
#include "flatbuffers/default_allocator.h"

#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// Magic-number sentinels used across tests.
constexpr uint8_t kFillByte = 0xAA;
constexpr uint8_t kBackFillByte = 0x5A;

// ---------------------------------------------------------------------------
// DefaultAllocatorBasicTest
// Directly exercises the DefaultAllocator member functions.
// ---------------------------------------------------------------------------

TEST(DefaultAllocatorBasicTest, AllocateReturnsWritableMemory)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::allocate, ::flatbuffers::DefaultAllocator::deallocate");
  RecordProperty("Description", "allocate returns non-null and the memory is writable");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  constexpr size_t kSize = 128;
  uint8_t* p = alloc.allocate(kSize);
  ASSERT_NE(p, nullptr);
  std::memset(p, kFillByte, kSize);
  EXPECT_EQ(p[0], kFillByte);
  EXPECT_EQ(p[kSize - 1], kFillByte);
  alloc.deallocate(p, kSize);
}

TEST(DefaultAllocatorBasicTest, AllocateSingleByte)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::allocate, ::flatbuffers::DefaultAllocator::deallocate");
  RecordProperty("Description", "allocate a single byte and write to it");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  DefaultAllocator alloc;
  uint8_t* p = alloc.allocate(1);
  ASSERT_NE(p, nullptr);
  *p = 0xFF;
  EXPECT_EQ(*p, 0xFF);
  alloc.deallocate(p, 1);
}

TEST(DefaultAllocatorBasicTest, AllocateLargeRegion)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::allocate, ::flatbuffers::DefaultAllocator::deallocate");
  RecordProperty("Description", "allocate a large (1 MiB) region");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  DefaultAllocator alloc;
  constexpr size_t kSize = 1024 * 1024;
  uint8_t* p = alloc.allocate(kSize);
  ASSERT_NE(p, nullptr);
  std::memset(p, kBackFillByte, kSize);
  EXPECT_EQ(p[0], kBackFillByte);
  EXPECT_EQ(p[kSize - 1], kBackFillByte);
  alloc.deallocate(p, kSize);
}

TEST(DefaultAllocatorBasicTest, DeallocateSizeArgumentIgnored)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::deallocate");
  RecordProperty("Description", "deallocate accepts any size argument without crashing");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  uint8_t* p = alloc.allocate(64);
  ASSERT_NE(p, nullptr);
  alloc.deallocate(p, 999);
}

TEST(DefaultAllocatorBasicTest, StaticDeallocDoesNotCrash)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::dealloc");
  RecordProperty("Description", "static dealloc method frees memory without crashing");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  uint8_t* p = alloc.allocate(32);
  ASSERT_NE(p, nullptr);
  DefaultAllocator::dealloc(static_cast<void*>(p), 32);
}

TEST(DefaultAllocatorBasicTest, UsableThroughBasePointer)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::allocate, ::flatbuffers::DefaultAllocator::deallocate, ~DefaultAllocator, Allocator virtual interface");
  RecordProperty("Description", "DefaultAllocator is usable through base Allocator pointer");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  Allocator* alloc = new DefaultAllocator();
  uint8_t* p = alloc->allocate(16);
  ASSERT_NE(p, nullptr);
  alloc->deallocate(p, 16);
  delete alloc;
}

TEST(DefaultAllocatorBasicTest, MultipleAllocationsDoNotAlias)
{
  RecordProperty("FullyVerifies", "::flatbuffers::DefaultAllocator::allocate, ::flatbuffers::DefaultAllocator::deallocate");
  RecordProperty("Description", "multiple independent allocations do not alias");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  uint8_t* p1 = alloc.allocate(64);
  uint8_t* p2 = alloc.allocate(64);
  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  EXPECT_NE(p1, p2);
  alloc.deallocate(p1, 64);
  alloc.deallocate(p2, 64);
}

// ---------------------------------------------------------------------------
// AllocateFreeHelperTest
// Tests Allocate(Allocator*, size_t).
// ---------------------------------------------------------------------------

TEST(AllocateFreeHelperTest, NullAllocatorUsesDefault)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Allocate");
  RecordProperty("Description", "null allocator falls back to DefaultAllocator");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  uint8_t* p = Allocate(nullptr, 64);
  ASSERT_NE(p, nullptr);
  std::memset(p, 0x11, 64);
  EXPECT_EQ(p[0], 0x11);
  Deallocate(nullptr, p, 64);
}

TEST(AllocateFreeHelperTest, CustomAllocatorIsUsed)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Allocate");
  RecordProperty("Description", "valid allocator is delegated to by Allocate");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  uint8_t* p = Allocate(&alloc, 64);
  ASSERT_NE(p, nullptr);
  std::memset(p, 0x22, 64);
  EXPECT_EQ(p[0], 0x22);
  Deallocate(&alloc, p, 64);
}

// ---------------------------------------------------------------------------
// DeallocateFreeHelperTest
// Tests Deallocate(Allocator*, uint8_t*, size_t).
// ---------------------------------------------------------------------------

TEST(DeallocateFreeHelperTest, NullAllocatorUsesDefault)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Deallocate");
  RecordProperty("Description", "null allocator falls back to DefaultAllocator");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator default_alloc;
  uint8_t* p = default_alloc.allocate(32);
  ASSERT_NE(p, nullptr);
  Deallocate(nullptr, p, 32);
}

TEST(DeallocateFreeHelperTest, CustomAllocatorIsUsed)
{
  RecordProperty("FullyVerifies", "::flatbuffers::Deallocate");
  RecordProperty("Description", "valid allocator is delegated to by Deallocate");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  uint8_t* p = alloc.allocate(32);
  ASSERT_NE(p, nullptr);
  Deallocate(&alloc, p, 32);
}

// ---------------------------------------------------------------------------
// ReallocateDownwardFreeHelperTest
// Tests ReallocateDownward.
// ---------------------------------------------------------------------------

TEST(ReallocateDownwardFreeHelperTest, NullAllocatorUsesDefault)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ReallocateDownward");
  RecordProperty("Description", "null allocator falls back to DefaultAllocator");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator tmp;
  constexpr size_t kOld = 64;
  constexpr size_t kNew = 128;
  uint8_t* old_p = tmp.allocate(kOld);
  ASSERT_NE(old_p, nullptr);

  uint8_t* new_p = ReallocateDownward(nullptr, old_p, kOld, kNew, 0, 0);
  ASSERT_NE(new_p, nullptr);
  Deallocate(nullptr, new_p, kNew);
}

TEST(ReallocateDownwardFreeHelperTest, CustomAllocatorIsUsed)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ReallocateDownward");
  RecordProperty("Description", "valid allocator is delegated to by ReallocateDownward");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "equivalence-classes");

  DefaultAllocator alloc;
  constexpr size_t kOld = 64;
  constexpr size_t kNew = 128;
  uint8_t* old_p = alloc.allocate(kOld);
  ASSERT_NE(old_p, nullptr);

  uint8_t* new_p = ReallocateDownward(&alloc, old_p, kOld, kNew, 0, 0);
  ASSERT_NE(new_p, nullptr);
  alloc.deallocate(new_p, kNew);
}

TEST(ReallocateDownwardFreeHelperTest, NullAllocatorPreservesBackData)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ReallocateDownward");
  RecordProperty("Description", "ReallocateDownward preserves back data via null-allocator path");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  DefaultAllocator tmp;
  constexpr size_t kOld = 64;
  constexpr size_t kNew = 128;
  constexpr size_t kBack = 16;

  uint8_t* old_p = tmp.allocate(kOld);
  ASSERT_NE(old_p, nullptr);
  for (size_t i = 0; i < kBack; ++i)
  {
    old_p[kOld - kBack + i] = static_cast<uint8_t>(i + 10);
  }

  uint8_t* new_p = ReallocateDownward(nullptr, old_p, kOld, kNew, kBack, 0);
  ASSERT_NE(new_p, nullptr);

  for (size_t i = 0; i < kBack; ++i)
  {
    EXPECT_EQ(new_p[kNew - kBack + i], static_cast<uint8_t>(i + 10)) << "mismatch at back index " << i;
  }
  Deallocate(nullptr, new_p, kNew);
}

TEST(ReallocateDownwardFreeHelperTest, CustomAllocatorPreservesFrontData)
{
  RecordProperty("FullyVerifies", "::flatbuffers::ReallocateDownward");
  RecordProperty("Description", "ReallocateDownward preserves front data via custom-allocator path");
  RecordProperty("TestType", "unit-test");
  RecordProperty("DerivationTechnique", "boundary-value-analysis");

  DefaultAllocator alloc;
  constexpr size_t kOld = 64;
  constexpr size_t kNew = 128;
  constexpr size_t kFront = 8;

  uint8_t* old_p = alloc.allocate(kOld);
  ASSERT_NE(old_p, nullptr);
  for (size_t i = 0; i < kFront; ++i)
  {
    old_p[i] = static_cast<uint8_t>(0xC0 + i);
  }

  uint8_t* new_p = ReallocateDownward(&alloc, old_p, kOld, kNew, 0, kFront);
  ASSERT_NE(new_p, nullptr);

  for (size_t i = 0; i < kFront; ++i)
  {
    EXPECT_EQ(new_p[i], static_cast<uint8_t>(0xC0 + i)) << "mismatch at front index " << i;
  }
  alloc.deallocate(new_p, kNew);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score