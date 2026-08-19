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
#include "flatbuffers/allocator.h"

#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>
#include <new>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

// Magic-number sentinels used across tests.
constexpr uint8_t kFrontByte = 0x0F;
constexpr uint8_t kFillByte = 0x55;
constexpr uint8_t kWriteByte = 0xAB;
constexpr uint8_t kBackByte = 0x80;

// -------------------------------------------------------
// Minimal Allocator implementation used across all tests.
// -------------------------------------------------------
class SimpleAllocator : public ::flatbuffers::Allocator
{
  public:
    SimpleAllocator() : alloc_throws_{false} {};

    void AllocWillThrow()
    {
        alloc_throws_ = true;
    }

    uint8_t* allocate(size_t size) override
    {
        if (alloc_throws_)
        {
            throw std::bad_alloc{};
        }
        return new uint8_t[size];
    }

    void deallocate(uint8_t* ptr, size_t /*size*/) override
    {
        delete[] ptr;
    }

  private:
    bool alloc_throws_;
};

// ---------------------------------------------------------------------------
// AllocatorTest
// Tests that a concrete Allocator implementation that satisfies the contract.
// ---------------------------------------------------------------------------

TEST(AllocatorTest, AllocateReturnsNonNull)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::allocate, ::flatbuffers::Allocator::deallocate");
    RecordProperty("Description", "allocate returns a non-null pointer; deallocate does not crash");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    EXPECT_NO_THROW({
        SimpleAllocator alloc;
        constexpr size_t kSize = 64U;
        uint8_t* ptr = alloc.allocate(kSize);
        ASSERT_NE(ptr, nullptr);
        alloc.deallocate(ptr, kSize);
    });
}

TEST(AllocatorTest, AllocateSingleByte)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::allocate, ::flatbuffers::Allocator::deallocate");
    RecordProperty("Description", "allocate and deallocate a single byte");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    SimpleAllocator alloc;
    uint8_t* ptr = alloc.allocate(1U);
    ASSERT_NE(ptr, nullptr);
    *ptr = kWriteByte;
    EXPECT_EQ(*ptr, kWriteByte);
    alloc.deallocate(ptr, 1U);
}

TEST(AllocatorTest, AllocateLargeRegion)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::allocate, ::flatbuffers::Allocator::deallocate");
    RecordProperty("Description", "allocate and deallocate a large (1 MiB) region");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    SimpleAllocator alloc;
    constexpr size_t kSize = static_cast<size_t>(1024U) * 1024U;  // 1 MiB
    uint8_t* ptr = alloc.allocate(kSize);
    ASSERT_NE(ptr, nullptr);
    std::memset(ptr, kFillByte, kSize);
    EXPECT_EQ(ptr[0], kFillByte);
    EXPECT_EQ(ptr[kSize - 1U], kFillByte);
    alloc.deallocate(ptr, kSize);
}

TEST(AllocatorTest, UsableThroughBasePointer)
{
    RecordProperty("FullyVerifies",
                   "::flatbuffers::Allocator::allocate, ::flatbuffers::Allocator::deallocate, "
                   "::flatbuffers::Allocator::~Allocator");
    RecordProperty("Description", "allocator is usable through a base-class pointer; virtual destructor is exercised");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    constexpr size_t kSize = 32U;
    auto alloc = std::make_unique<SimpleAllocator>();
    ::flatbuffers::Allocator* base_ptr = alloc.get();
    uint8_t* ptr = base_ptr->allocate(kSize);
    ASSERT_NE(ptr, nullptr);
    base_ptr->deallocate(ptr, kSize);
    // unique_ptr exercises virtual destructor on scope exit
}

// ---------------------------------------------------------------------------
// AllocatorReallocateDownwardTest
// Tests the default reallocate_downward implementation provided by Allocator.
// ---------------------------------------------------------------------------

struct GrowsBufferParams
{
    size_t old_size;
    size_t new_size;
};

class AllocatorGrowsBufferTest : public ::testing::TestWithParam<GrowsBufferParams>
{
};

TEST_P(AllocatorGrowsBufferTest, GrowsBuffer)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward grows the buffer and returns a non-null pointer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto [old_size, new_size] = GetParam();
    SimpleAllocator alloc;

    uint8_t* old_ptr = alloc.allocate(old_size);
    ASSERT_NE(old_ptr, nullptr);

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, old_size, new_size, 0U, 0U);
    ASSERT_NE(new_ptr, nullptr);
    EXPECT_NE(new_ptr, old_ptr);

    alloc.deallocate(new_ptr, new_size);
}

INSTANTIATE_TEST_SUITE_P(SizeRanges,
                         AllocatorGrowsBufferTest,
                         ::testing::Values(GrowsBufferParams{1U, 2U},
                                           GrowsBufferParams{8U, 16U},
                                           GrowsBufferParams{64U, 128U},
                                           GrowsBufferParams{256U, 1024U}));

struct BackDataParams
{
    size_t old_size;
    size_t new_size;
    size_t back_size;
};

class AllocatorPreservesBackDataTest : public ::testing::TestWithParam<BackDataParams>
{
};

TEST_P(AllocatorPreservesBackDataTest, PreservesBackData)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward copies in_use_back bytes to the tail of the new buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto [old_size, new_size, back_size] = GetParam();
    SimpleAllocator alloc;

    uint8_t* old_ptr = alloc.allocate(old_size);
    ASSERT_NE(old_ptr, nullptr);

    for (size_t i = 0U; i < back_size; ++i)
    {
        old_ptr[old_size - back_size + i] = static_cast<uint8_t>(i + 1U);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, old_size, new_size, back_size, 0U);
    ASSERT_NE(new_ptr, nullptr);

    for (size_t i = 0U; i < back_size; ++i)
    {
        EXPECT_EQ(new_ptr[new_size - back_size + i], static_cast<uint8_t>(i + 1U)) << "mismatch at back index " << i;
    }

    alloc.deallocate(new_ptr, new_size);
}

INSTANTIATE_TEST_SUITE_P(SizeRanges,
                         AllocatorPreservesBackDataTest,
                         ::testing::Values(BackDataParams{4U, 8U, 1U},
                                           BackDataParams{64U, 128U, 16U},
                                           BackDataParams{256U, 1024U, 128U}));

struct FrontDataParams
{
    size_t old_size;
    size_t new_size;
    size_t front_size;
};

class AllocatorPreservesFrontDataTest : public ::testing::TestWithParam<FrontDataParams>
{
};

TEST_P(AllocatorPreservesFrontDataTest, PreservesFrontData)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward copies in_use_front bytes to the head of the new buffer");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto [old_size, new_size, front_size] = GetParam();
    SimpleAllocator alloc;

    uint8_t* old_ptr = alloc.allocate(old_size);
    ASSERT_NE(old_ptr, nullptr);

    for (size_t i = 0U; i < front_size; ++i)
    {
        old_ptr[i] = static_cast<uint8_t>(kFrontByte + i);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, old_size, new_size, 0U, front_size);
    ASSERT_NE(new_ptr, nullptr);

    for (size_t i = 0U; i < front_size; ++i)
    {
        EXPECT_EQ(new_ptr[i], static_cast<uint8_t>(kFrontByte + i)) << "mismatch at front index " << i;
    }

    alloc.deallocate(new_ptr, new_size);
}

INSTANTIATE_TEST_SUITE_P(SizeRanges,
                         AllocatorPreservesFrontDataTest,
                         ::testing::Values(FrontDataParams{4U, 8U, 1U},
                                           FrontDataParams{64U, 128U, 8U},
                                           FrontDataParams{256U, 1024U, 64U}));

struct BothDataParams
{
    size_t old_size;
    size_t new_size;
    size_t back_size;
    size_t front_size;
};

class AllocatorPreservesBothDataTest : public ::testing::TestWithParam<BothDataParams>
{
};

TEST_P(AllocatorPreservesBothDataTest, PreservesBothFrontAndBackData)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward preserves both front and back in-use regions simultaneously");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    const auto [old_size, new_size, back_size, front_size] = GetParam();
    SimpleAllocator alloc;

    uint8_t* old_ptr = alloc.allocate(old_size);
    ASSERT_NE(old_ptr, nullptr);

    for (size_t i = 0U; i < front_size; ++i)
    {
        old_ptr[i] = static_cast<uint8_t>(kFrontByte + i);
    }
    for (size_t i = 0U; i < back_size; ++i)
    {
        old_ptr[old_size - back_size + i] = static_cast<uint8_t>(kBackByte + i);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, old_size, new_size, back_size, front_size);
    ASSERT_NE(new_ptr, nullptr);

    for (size_t i = 0U; i < front_size; ++i)
    {
        EXPECT_EQ(new_ptr[i], static_cast<uint8_t>(kFrontByte + i)) << "front mismatch at index " << i;
    }
    for (size_t i = 0U; i < back_size; ++i)
    {
        EXPECT_EQ(new_ptr[new_size - back_size + i], static_cast<uint8_t>(kBackByte + i))
            << "back mismatch at index " << i;
    }

    alloc.deallocate(new_ptr, new_size);
}

INSTANTIATE_TEST_SUITE_P(SizeRanges,
                         AllocatorPreservesBothDataTest,
                         ::testing::Values(BothDataParams{16U, 32U, 4U, 4U},
                                           BothDataParams{64U, 256U, 20U, 12U},
                                           BothDataParams{256U, 1024U, 64U, 32U}));

TEST(AllocatorReallocateDownwardTest, PreserveFrontAndBackIfAllocatingNewSizeFails)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward preserves front and back if new size cannot be allocated");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    SimpleAllocator alloc;
    constexpr size_t kOldSize = 64U;
    constexpr size_t kNewSize = 12345U;
    constexpr size_t kBack = 20U;
    constexpr size_t kFront = 12U;

    uint8_t* old_ptr = alloc.allocate(kOldSize);
    ASSERT_NE(old_ptr, nullptr);

    for (size_t i = 0U; i < kFront; ++i)
    {
        old_ptr[i] = static_cast<uint8_t>(kFrontByte + i);
    }
    for (size_t i = 0U; i < kBack; ++i)
    {
        old_ptr[kOldSize - kBack + i] = static_cast<uint8_t>(kBackByte + i);
    }

    uint8_t* new_ptr = nullptr;

    EXPECT_THROW(
        {
            alloc.AllocWillThrow();
            new_ptr = alloc.reallocate_downward(old_ptr, kOldSize, kNewSize, kBack, kFront);
        },
        std::bad_alloc);

    ASSERT_EQ(new_ptr, nullptr);

    for (size_t i = 0U; i < kFront; ++i)
    {
        EXPECT_EQ(old_ptr[i], static_cast<uint8_t>(kFrontByte + i)) << "front mismatch at index " << i;
    }
    for (size_t i = 0U; i < kBack; ++i)
    {
        EXPECT_EQ(old_ptr[kOldSize - kBack + i], static_cast<uint8_t>(kBackByte + i)) << "back mismatch at index " << i;
    }

    alloc.deallocate(old_ptr, kOldSize);
}

TEST(AllocatorReallocateDownwardTest, AssertsWhenNewSizeEqualsOldSize)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward asserts when new_size equals old_size");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    EXPECT_DEATH(
        {
            SimpleAllocator alloc;
            constexpr size_t kSize = 64U;
            uint8_t* ptr = alloc.allocate(kSize);
            alloc.reallocate_downward(ptr, kSize, kSize, 0U, 0U);
        },
        "");
}

TEST(AllocatorReallocateDownwardTest, AssertsWhenNewSizeSmallerThanOldSize)
{
    RecordProperty("FullyVerifies", "::flatbuffers::Allocator::reallocate_downward");
    RecordProperty("Description", "reallocate_downward asserts when new_size is smaller than old_size");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-value-analysis");

    EXPECT_DEATH(
        {
            SimpleAllocator alloc;
            constexpr size_t kOldSize = 64U;
            constexpr size_t kNewSize = 32U;
            uint8_t* ptr = alloc.allocate(kOldSize);
            alloc.reallocate_downward(ptr, kOldSize, kNewSize, 0U, 0U);
        },
        "");
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
