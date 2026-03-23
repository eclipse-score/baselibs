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

#include "score/nothrow/memory_resource.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

namespace score::nothrow
{
namespace
{

class CountingMemoryResource final : public MemoryResource
{
  public:
    void* allocate(std::size_t bytes, std::size_t) noexcept override
    {
        ++allocate_calls_;
        last_allocation_size_ = bytes;
        return (bytes == 0U) ? nullptr : static_cast<void*>(&storage_);
    }

    void deallocate(void*, std::size_t bytes, std::size_t) noexcept override
    {
        ++deallocate_calls_;
        last_deallocation_size_ = bytes;
    }

    bool is_equal(const MemoryResource& other) const noexcept override
    {
        return this == &other;
    }

    std::size_t allocate_calls_{0U};
    std::size_t deallocate_calls_{0U};
    std::size_t last_allocation_size_{0U};
    std::size_t last_deallocation_size_{0U};

  private:
    alignas(std::max_align_t) std::uint8_t storage_[128U]{};
};

TEST(MemoryResource, GetDefaultResourceReturnsNewDeleteResource)
{
    EXPECT_EQ(GetDefaultResource(), GetNewDeleteResource());
}

TEST(MemoryResource, NewDeleteResourceCanAllocateAndDeallocate)
{
    MemoryResource* const resource = GetNewDeleteResource();
    ASSERT_NE(resource, nullptr);

    void* const block = resource->allocate(64U, alignof(std::max_align_t));
    ASSERT_NE(block, nullptr);

    resource->deallocate(block, 64U, alignof(std::max_align_t));
}

TEST(PolymorphicAllocator, DefaultAllocatorUsesDefaultResource)
{
    PolymorphicAllocator<std::uint32_t> allocator{};

    std::uint32_t* const ptr = allocator.allocate(4U);
    ASSERT_NE(ptr, nullptr);

    allocator.deallocate(ptr, 4U);
}

TEST(PolymorphicAllocator, UsesProvidedResource)
{
    CountingMemoryResource resource{};
    PolymorphicAllocator<std::uint32_t> allocator{&resource};

    std::uint32_t* const ptr = allocator.allocate(3U);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(resource.allocate_calls_, 1U);
    EXPECT_EQ(resource.last_allocation_size_, sizeof(std::uint32_t) * 3U);

    allocator.deallocate(ptr, 3U);
    EXPECT_EQ(resource.deallocate_calls_, 1U);
    EXPECT_EQ(resource.last_deallocation_size_, sizeof(std::uint32_t) * 3U);
}

TEST(PolymorphicAllocator, NullResourceFallsBackToDefaultResource)
{
    PolymorphicAllocator<std::uint8_t> allocator{nullptr};

    std::uint8_t* const ptr = allocator.allocate(8U);
    ASSERT_NE(ptr, nullptr);

    allocator.deallocate(ptr, 8U);
}

TEST(MemoryResource, NullMemoryResourceAlwaysReturnsNullptr)
{
    MemoryResource* const resource = GetNullMemoryResource();
    ASSERT_NE(resource, nullptr);
    EXPECT_EQ(resource->allocate(64U, alignof(std::max_align_t)), nullptr);
    EXPECT_EQ(resource->allocate(0U, 1U), nullptr);
}

TEST(MemoryResource, SetDefaultResourceChangesDefault)
{
    CountingMemoryResource custom{};
    MemoryResource* const previous = SetDefaultResource(&custom);
    EXPECT_EQ(previous, GetNewDeleteResource());
    EXPECT_EQ(GetDefaultResource(), &custom);

    SetDefaultResource(nullptr);
    EXPECT_EQ(GetDefaultResource(), GetNewDeleteResource());
}

}  // namespace
}  // namespace score::nothrow
