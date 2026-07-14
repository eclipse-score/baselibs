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
#include "score/containers/test/fancy_pointer_allocator.h"

#include "score/containers/test/fake_memory_resource.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <type_traits>

namespace score::containers::test
{
namespace
{

static_assert(!std::is_same<FancyPointerAllocator<std::uint32_t>::pointer, std::uint32_t*>::value,
              "FancyPointerAllocator's pointer type must be a fancy pointer, not a raw pointer.");

TEST(FancyPointerAllocatorTest, AllocateForwardsToMemoryResourceAndTracksBytes)
{
    FakeMemoryResource resource{};
    FancyPointerAllocator<std::uint32_t> allocator{resource};

    auto pointer = allocator.allocate(4U);

    EXPECT_NE(pointer.get(), nullptr);
    EXPECT_EQ(resource.GetUserAllocatedBytes(), 4U * sizeof(std::uint32_t));

    allocator.deallocate(pointer, 4U);
    EXPECT_EQ(resource.GetUserDeallocatedBytes(), 4U * sizeof(std::uint32_t));
}

TEST(FancyPointerAllocatorTest, AllocatorsSharingTheSameResourceCompareEqual)
{
    FakeMemoryResource resource{};
    FancyPointerAllocator<std::uint32_t> allocator_a{resource};
    FancyPointerAllocator<std::uint32_t> allocator_b{resource};

    EXPECT_TRUE(allocator_a == allocator_b);
    EXPECT_FALSE(allocator_a != allocator_b);
}

TEST(FancyPointerAllocatorTest, AllocatorsWithDifferentResourcesCompareNotEqual)
{
    FakeMemoryResource resource_a{};
    FakeMemoryResource resource_b{};
    FancyPointerAllocator<std::uint32_t> allocator_a{resource_a};
    FancyPointerAllocator<std::uint32_t> allocator_b{resource_b};

    EXPECT_TRUE(allocator_a != allocator_b);
    EXPECT_FALSE(allocator_a == allocator_b);
}

TEST(FancyPointerAllocatorTest, RebindConstructsFromDifferentElementType)
{
    FakeMemoryResource resource{};
    FancyPointerAllocator<std::uint32_t> allocator{resource};

    using ReboundAllocator = std::allocator_traits<FancyPointerAllocator<std::uint32_t>>::rebind_alloc<std::uint64_t>;
    ReboundAllocator rebound{allocator};

    EXPECT_EQ(rebound.GetMemoryResource(), &resource);
}

}  // namespace
}  // namespace score::containers::test
