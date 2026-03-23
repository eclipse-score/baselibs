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
#ifndef SCORE_NOTHROW_MEMORY_RESOURCE_H
#define SCORE_NOTHROW_MEMORY_RESOURCE_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <type_traits>

namespace score::nothrow
{

/// @brief Polymorphic memory resource interface for nothrow allocation.
///
/// Follows the std::pmr::memory_resource model with one critical deviation:
/// all operations are noexcept and allocation failure is signaled by returning
/// nullptr. This is the contract expected by score::nothrow containers, which
/// propagate allocation failures as score::Result errors rather than throwing
/// std::bad_alloc.
class MemoryResource
{
  public:
    virtual ~MemoryResource() = default;

    virtual void* allocate(std::size_t bytes, std::size_t alignment) noexcept = 0;
    virtual void deallocate(void* pointer, std::size_t bytes, std::size_t alignment) noexcept = 0;
    virtual bool is_equal(const MemoryResource& other) const noexcept = 0;
};

/// @brief Returns a process-local new/delete backed memory resource.
MemoryResource* GetNewDeleteResource() noexcept;

/// @brief Returns a memory resource that always fails allocation (returns nullptr).
MemoryResource* GetNullMemoryResource() noexcept;

/// @brief Returns the current default memory resource.
///
/// Initially returns GetNewDeleteResource(). Can be changed via SetDefaultResource().
MemoryResource* GetDefaultResource() noexcept;

/// @brief Sets the default memory resource.
/// @param resource The new default. If nullptr, resets to GetNewDeleteResource().
/// @return The previous default resource.
MemoryResource* SetDefaultResource(MemoryResource* resource) noexcept;

/// @brief A polymorphic allocator in the style of std::pmr::polymorphic_allocator.
///
/// All operations are noexcept. allocate() returns nullptr on failure instead
/// of throwing std::bad_alloc. score::nothrow containers depend on this contract
/// to translate allocation failures into score::Result errors.
template <typename T = std::uint8_t>
class PolymorphicAllocator
{
  public:
    template <typename>
    friend class PolymorphicAllocator;

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    PolymorphicAllocator() noexcept : resource_{GetDefaultResource()} {}

    // NOLINTNEXTLINE(google-explicit-constructor): follows std::pmr style.
    PolymorphicAllocator(MemoryResource* resource) noexcept
        : resource_{resource == nullptr ? GetDefaultResource() : resource}
    {
    }

    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor): follows std::pmr style.
    PolymorphicAllocator(const PolymorphicAllocator<U>& other) noexcept : resource_{other.resource()}
    {
    }

    PolymorphicAllocator(const PolymorphicAllocator&) noexcept = default;
    PolymorphicAllocator(PolymorphicAllocator&&) noexcept = default;
    PolymorphicAllocator& operator=(const PolymorphicAllocator&) noexcept = default;
    PolymorphicAllocator& operator=(PolymorphicAllocator&&) noexcept = default;
    ~PolymorphicAllocator() = default;

    [[nodiscard]] T* allocate(const size_type count) noexcept
    {
        if ((count == 0U) || (count > (std::numeric_limits<size_type>::max() / sizeof(T))))
        {
            return nullptr;
        }

        MemoryResource* const memory_resource{resource_};
        void* const pointer{memory_resource->allocate(count * sizeof(T), alignof(T))};
        return static_cast<T*>(pointer);
    }

    void deallocate(T* const pointer, const size_type count) noexcept
    {
        if ((pointer == nullptr) || (count == 0U))
        {
            return;
        }

        MemoryResource* const memory_resource{resource_};
        memory_resource->deallocate(pointer, count * sizeof(T), alignof(T));
    }

    MemoryResource* resource() noexcept
    {
        return resource_;
    }

    const MemoryResource* resource() const noexcept
    {
        return resource_;
    }

    PolymorphicAllocator select_on_container_copy_construction() const noexcept
    {
        return PolymorphicAllocator{};
    }

  private:
    MemoryResource* resource_;
};

template <typename T1, typename T2>
bool operator==(const PolymorphicAllocator<T1>& lhs, const PolymorphicAllocator<T2>& rhs) noexcept
{
    const MemoryResource* const lhs_resource{lhs.resource()};
    const MemoryResource* const rhs_resource{rhs.resource()};
    return (lhs_resource == rhs_resource) || lhs_resource->is_equal(*rhs_resource);
}

template <typename T1, typename T2>
bool operator!=(const PolymorphicAllocator<T1>& lhs, const PolymorphicAllocator<T2>& rhs) noexcept
{
    return !(lhs == rhs);
}

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_MEMORY_RESOURCE_H
