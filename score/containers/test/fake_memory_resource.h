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
#ifndef SCORE_LIB_CONTAINERS_TEST_FAKE_MEMORY_RESOURCE_H
#define SCORE_LIB_CONTAINERS_TEST_FAKE_MEMORY_RESOURCE_H

#include <score/memory_resource.hpp>

#include <cstddef>

namespace score::containers::test
{

/// \brief A generic, unbounded, allocation-backed score::cpp::pmr::memory_resource that tracks the number of bytes
/// allocated and deallocated by its users.
///
/// Used in allocator-aware container tests to verify that containers request/release exactly the expected
/// number of bytes, without requiring any shared-memory-specific behavior.
class FakeMemoryResource : public score::cpp::pmr::memory_resource
{
  public:
    FakeMemoryResource() noexcept = default;
    ~FakeMemoryResource() override = default;

    FakeMemoryResource(const FakeMemoryResource&) = delete;
    FakeMemoryResource& operator=(const FakeMemoryResource&) = delete;
    FakeMemoryResource(FakeMemoryResource&&) = delete;
    FakeMemoryResource& operator=(FakeMemoryResource&&) = delete;

    std::size_t GetUserAllocatedBytes() const noexcept
    {
        return allocated_bytes_;
    }

    std::size_t GetUserDeallocatedBytes() const noexcept
    {
        return deallocated_bytes_;
    }

  private:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* memory, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const score::cpp::pmr::memory_resource& other) const noexcept override;

    std::size_t allocated_bytes_{0U};
    std::size_t deallocated_bytes_{0U};
};

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_FAKE_MEMORY_RESOURCE_H
