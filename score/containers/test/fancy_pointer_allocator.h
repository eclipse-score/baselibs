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
#ifndef SCORE_LIB_CONTAINERS_TEST_FANCY_POINTER_ALLOCATOR_H
#define SCORE_LIB_CONTAINERS_TEST_FANCY_POINTER_ALLOCATOR_H

#include "score/containers/test/mockable_pointer.h"

#include <score/memory_resource.hpp>
#include <score/assert.hpp>

#include <cstddef>
#include <limits>

namespace score::containers::test
{

/// \brief A generic allocator whose pointer type is MockablePointer<T> (a fancy pointer, not a raw T*), backed by a
/// score::cpp::pmr::memory_resource.
///
/// Used in allocator-aware container tests to verify that containers correctly support fancy pointers (i.e. do not
/// assume the allocator's pointer type is a raw pointer). This mirrors the role that
/// score::memory::shared::PolymorphicOffsetPtrAllocator plays in production code, without requiring any
/// shared-memory-specific machinery.
template <typename T>
class FancyPointerAllocator
{
  public:
    using value_type = T;
    using pointer = MockablePointer<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
        using other = FancyPointerAllocator<U>;
    };

    // Non-explicit constructor is required so that FancyPointerAllocator can be constructed implicitly from a
    // memory resource reference, mirroring PolymorphicOffsetPtrAllocator's constructor from ManagedMemoryResource&.
    // NOLINTNEXTLINE(google-explicit-constructor): intentional implicit conversion.
    FancyPointerAllocator(score::cpp::pmr::memory_resource& resource) noexcept : resource_{&resource} {}

    template <typename U>
    // Non-explicit constructor is required to support allocator rebind.
    // NOLINTNEXTLINE(google-explicit-constructor): intentional implicit conversion.
    FancyPointerAllocator(const FancyPointerAllocator<U>& other) noexcept : resource_{other.GetMemoryResource()}
    {
    }

    FancyPointerAllocator(const FancyPointerAllocator&) noexcept = default;
    FancyPointerAllocator(FancyPointerAllocator&&) noexcept = default;
    FancyPointerAllocator& operator=(const FancyPointerAllocator&) noexcept = default;
    FancyPointerAllocator& operator=(FancyPointerAllocator&&) noexcept = default;
    ~FancyPointerAllocator() = default;

    pointer allocate(size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(n <= (std::numeric_limits<size_type>::max() / sizeof(T)));
        return pointer{static_cast<T*>(resource_->allocate(n * sizeof(T), alignof(T)))};
    }

    void deallocate(pointer p, size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(n <= (std::numeric_limits<size_type>::max() / sizeof(T)));
        resource_->deallocate(static_cast<void*>(p.get()), n * sizeof(T), alignof(T));
    }

    score::cpp::pmr::memory_resource* GetMemoryResource() const noexcept
    {
        return resource_;
    }

  private:
    score::cpp::pmr::memory_resource* resource_;
};

template <typename T>
bool operator==(const FancyPointerAllocator<T>& lhs, const FancyPointerAllocator<T>& rhs) noexcept
{
    return *lhs.GetMemoryResource() == *rhs.GetMemoryResource();
}

template <typename T>
bool operator!=(const FancyPointerAllocator<T>& lhs, const FancyPointerAllocator<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_FANCY_POINTER_ALLOCATOR_H
