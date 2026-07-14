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
#include <cstdlib>
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

    // Default constructor is required for parity with PolymorphicOffsetPtrAllocator's default constructor, which is
    // relied upon by GetAllocator()'s SFINAE dispatch in allocator_test_type_helpers.h whenever the requested
    // Allocator type doesn't literally match FancyPointerAllocator<ElementType> (i.e. the allocator is rebound to a
    // different element type than the one it was originally instantiated with). The resulting allocator is never
    // used to allocate directly; it is only ever rebound (via the converting constructor below) before use.
    FancyPointerAllocator() noexcept = default;

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
        // Falls back to malloc when unconnected to a memory resource (default-constructed), mirroring
        // PolymorphicOffsetPtrAllocator::allocate()'s behavior for a null proxy_.
        if (resource_ == nullptr)
        {
            return pointer{static_cast<T*>(std::malloc(n * sizeof(T)))};
        }
        return pointer{static_cast<T*>(resource_->allocate(n * sizeof(T), alignof(T)))};
    }

    void deallocate(pointer p, size_type n)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(n <= (std::numeric_limits<size_type>::max() / sizeof(T)));
        if (resource_ == nullptr)
        {
            std::free(static_cast<void*>(p.get()));
            return;
        }
        resource_->deallocate(static_cast<void*>(p.get()), n * sizeof(T), alignof(T));
    }

    score::cpp::pmr::memory_resource* GetMemoryResource() const noexcept
    {
        return resource_;
    }

  private:
    score::cpp::pmr::memory_resource* resource_{nullptr};
};

template <typename T>
bool operator==(const FancyPointerAllocator<T>& lhs, const FancyPointerAllocator<T>& rhs) noexcept
{
    if ((lhs.GetMemoryResource() == nullptr) || (rhs.GetMemoryResource() == nullptr))
    {
        return lhs.GetMemoryResource() == rhs.GetMemoryResource();
    }
    return *lhs.GetMemoryResource() == *rhs.GetMemoryResource();
}

template <typename T>
bool operator!=(const FancyPointerAllocator<T>& lhs, const FancyPointerAllocator<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_FANCY_POINTER_ALLOCATOR_H
