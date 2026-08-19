/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#include "score/containers/test/fake_memory_resource.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace score::containers::test
{
namespace
{

TEST(FakeMemoryResourceTest, InitiallyHasZeroAllocatedAndDeallocatedBytes)
{
    FakeMemoryResource resource{};
    EXPECT_EQ(resource.GetUserAllocatedBytes(), 0U);
    EXPECT_EQ(resource.GetUserDeallocatedBytes(), 0U);
}

TEST(FakeMemoryResourceTest, AllocateTracksNumberOfBytesAllocated)
{
    FakeMemoryResource resource{};
    void* memory = resource.allocate(64U, alignof(std::uint64_t));
    EXPECT_NE(memory, nullptr);
    EXPECT_EQ(resource.GetUserAllocatedBytes(), 64U);
    resource.deallocate(memory, 64U, alignof(std::uint64_t));
}

TEST(FakeMemoryResourceTest, AllocateHonorsRequestedAlignment)
{
    constexpr std::size_t alignment{128U};

    FakeMemoryResource resource{};
    void* memory = resource.allocate(64U, alignment);

    ASSERT_NE(memory, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(memory) % alignment, 0U);

    resource.deallocate(memory, 64U, alignment);
}

TEST(FakeMemoryResourceTest, DeallocateTracksNumberOfBytesDeallocated)
{
    FakeMemoryResource resource{};
    void* memory = resource.allocate(32U, alignof(std::uint64_t));
    resource.deallocate(memory, 32U, alignof(std::uint64_t));
    EXPECT_EQ(resource.GetUserDeallocatedBytes(), 32U);
}

TEST(FakeMemoryResourceTest, IsEqualOnlyToItself)
{
    FakeMemoryResource resource_a{};
    FakeMemoryResource resource_b{};
    EXPECT_TRUE(resource_a.is_equal(resource_a));
    EXPECT_FALSE(resource_a.is_equal(resource_b));
}

}  // namespace
}  // namespace score::containers::test
