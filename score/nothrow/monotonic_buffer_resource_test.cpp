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

#include "score/nothrow/monotonic_buffer_resource.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

namespace score::nothrow
{
namespace
{

TEST(MonotonicBufferResource, AllocatesFromBuffer)
{
    alignas(std::max_align_t) std::byte buffer[256U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    void* const block = resource.allocate(64U, 1U);
    ASSERT_NE(block, nullptr);
    EXPECT_GE(static_cast<std::byte*>(block), buffer);
    EXPECT_LT(static_cast<std::byte*>(block), buffer + sizeof(buffer));
}

TEST(MonotonicBufferResource, RespectsAlignment)
{
    alignas(64U) std::byte buffer[256U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    resource.allocate(1U, 1U);

    void* const aligned = resource.allocate(32U, 16U);
    ASSERT_NE(aligned, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(aligned) % 16U, 0U);
}

TEST(MonotonicBufferResource, SequentialAllocationsDoNotOverlap)
{
    alignas(std::max_align_t) std::byte buffer[256U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    auto* const first = static_cast<std::byte*>(resource.allocate(32U, 1U));
    auto* const second = static_cast<std::byte*>(resource.allocate(32U, 1U));
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_GE(second, first + 32U);
}

TEST(MonotonicBufferResource, RemainingDecreasesOnAllocation)
{
    alignas(std::max_align_t) std::byte buffer[256U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    EXPECT_EQ(resource.remaining(), 256U);
    resource.allocate(64U, 1U);
    EXPECT_LE(resource.remaining(), 192U);
}

TEST(MonotonicBufferResource, ZeroBytesReturnsNullptr)
{
    alignas(std::max_align_t) std::byte buffer[64U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    EXPECT_EQ(resource.allocate(0U, 1U), nullptr);
    EXPECT_EQ(resource.remaining(), 64U);
}

TEST(MonotonicBufferResource, OverflowDelegatesToUpstream)
{
    alignas(std::max_align_t) std::byte buffer[32U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    void* const block = resource.allocate(64U, 1U);
    ASSERT_NE(block, nullptr);

    auto* const byte_ptr = static_cast<std::byte*>(block);
    EXPECT_FALSE(byte_ptr >= buffer && byte_ptr < buffer + sizeof(buffer));

    GetNewDeleteResource()->deallocate(block, 64U, 1U);
}

TEST(MonotonicBufferResource, OverflowToNullResourceReturnsNullptr)
{
    alignas(std::max_align_t) std::byte buffer[32U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer), GetNullMemoryResource()};

    EXPECT_NE(resource.allocate(16U, 1U), nullptr);
    EXPECT_EQ(resource.allocate(64U, 1U), nullptr);
}

TEST(MonotonicBufferResource, ReleaseResetsBuffer)
{
    alignas(std::max_align_t) std::byte buffer[128U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    resource.allocate(64U, 1U);
    EXPECT_LE(resource.remaining(), 64U);

    resource.release();
    EXPECT_EQ(resource.remaining(), 128U);

    void* const block = resource.allocate(128U, 1U);
    ASSERT_NE(block, nullptr);
    EXPECT_GE(static_cast<std::byte*>(block), buffer);
}

TEST(MonotonicBufferResource, DeallocateIsNoop)
{
    alignas(std::max_align_t) std::byte buffer[128U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};

    void* const block = resource.allocate(64U, 1U);
    const auto remaining_before = resource.remaining();

    resource.deallocate(block, 64U, 1U);
    EXPECT_EQ(resource.remaining(), remaining_before);
}

TEST(MonotonicBufferResource, IsEqualOnlyWithSelf)
{
    alignas(std::max_align_t) std::byte buffer_a[64U]{};
    alignas(std::max_align_t) std::byte buffer_b[64U]{};
    MonotonicBufferResource a{buffer_a, sizeof(buffer_a)};
    MonotonicBufferResource b{buffer_b, sizeof(buffer_b)};

    EXPECT_TRUE(a.is_equal(a));
    EXPECT_FALSE(a.is_equal(b));
}

TEST(MonotonicBufferResource, WorksWithPolymorphicAllocator)
{
    alignas(std::max_align_t) std::byte buffer[256U]{};
    MonotonicBufferResource resource{buffer, sizeof(buffer)};
    PolymorphicAllocator<std::uint32_t> allocator{&resource};

    std::uint32_t* const ptr = allocator.allocate(4U);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(ptr) % alignof(std::uint32_t), 0U);

    allocator.deallocate(ptr, 4U);
}

}  // namespace
}  // namespace score::nothrow
