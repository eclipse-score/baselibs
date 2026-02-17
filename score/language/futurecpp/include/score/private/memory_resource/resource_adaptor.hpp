/********************************************************************************
 * Copyright (c) 2019 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2019 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Pmr.MemoryResource component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_RESOURCE_ADAPTOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_RESOURCE_ADAPTOR_HPP

#include <score/private/memory_resource/memory_resource.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/assert.hpp>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::cpp
{
namespace pmr
{

namespace detail
{
// Extended alignment support
//
// This is implemented by extending the requested size to accommodate a single pointer value
// for bookkeeping and a padding area of size (alignment-1) that will be partitioned into two
// variable-sized padding areas, one before the object and one after the object, such that the
// alignment requirement is met.
// The bookkeeping value orig_ptr is required because deallocation requires recovering the original
// pointer value returned by the underlying allocator. As the size of the padding area before the
// object is no longer knowable at this point, the original pointer value must be saved explicitly.
//
// +--- bytes + sizeof(void*) + alignment - 1 -----------+
// |                                                     |
// |           | orig_ptr |xxxxxxx T xxxxxxxx|           |
// |--padding--|---void*--|----- bytes ------|--padding--|
//                        ^
//                        |- ret
//
/// Adjusts an allocation at orig_ptr of size total_size to meet the extended alignment
/// requirement of an allocation allocate(bytes, alignment). The behavior is undefined unless
/// total_size is at least bytes + sizeof(void*) + alignment - 1.
/// \return A pointer to a region of memory of at least size bytes aligned with alignment.
inline void*
establish_extended_alignment(void* orig_ptr, std::size_t bytes, std::size_t total_size, std::size_t alignment)
{
    // we always need sizeof(void*) bytes before the object to store orig_ptr
    void* ptr = static_cast<char*>(orig_ptr) + sizeof(void*);
    // leave additional space in front of the memory region to satisfy the alignment requirement
    std::size_t remaining_size = total_size - sizeof(void*);
    void* ret = std::align(alignment, bytes, ptr, remaining_size);
    SCORE_LANGUAGE_FUTURECPP_ASSERT(ret != nullptr);
    // store orig_ptr in the memory immediately preceding the object
    score::cpp::ignore = std::memcpy(static_cast<char*>(ret) - sizeof(void*), &orig_ptr, sizeof(void*));
    return ret;
}

/// Retrieves the original pointer from a pointer returned by establish_extended_alignment().
/// \pre aligned_ptr must have been returned by a previous call to establish_extended_alignment(orig_ptr, ...)
/// \return The value of orig_ptr passed into the corresponding call to establish_extended_alignment.
inline void* retrieve_unaligned_pointer(void* aligned_ptr)
{
    // the original pointer value returned by the underlying allocator is stored in the memory
    // immediately preceding the object
    void* orig_ptr;
    score::cpp::ignore = std::memcpy(&orig_ptr, static_cast<char*>(aligned_ptr) - sizeof(void*), sizeof(void*));
    return orig_ptr;
}

/// The type that an allocator wrapped by the resource adaptor will be rebound to.
/// Each allocation served by the wrapped allocator will have at least size and alignment required by this type.
/// Choosing a smaller type here does not necessarily lead to tighter memory bounds, as serving allocations with
/// alignment requirements stricter than this type incur additional memory overhead for respecting the alignment.
///
using resource_adaptor_base_type = std::aligned_storage_t<alignof(std::max_align_t), alignof(std::max_align_t)>;

/// Wraps a C++11 Allocator type \tparam Alloc with a pmr::memory_resource interface.
///
/// cppcoreguidelines-special-member-functions: Follows literally the C++ standard see
/// https://en.cppreference.com/w/cpp/experimental/resource_adaptor.html for details.
template <typename Alloc>
class resource_adaptor_impl : public memory_resource // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
    using allocator_type = Alloc;
    static_assert(alignof(typename Alloc::value_type) >= alignof(void*),
                  "Underlying allocator must be able to serve allocations suitably aligned for storing void*");

    resource_adaptor_impl() = default;
    resource_adaptor_impl(const resource_adaptor_impl&) = default;
    resource_adaptor_impl(resource_adaptor_impl&&) noexcept = default;

    explicit resource_adaptor_impl(const Alloc& a) : memory_resource{}, allocator_(a) {}

    explicit resource_adaptor_impl(Alloc&& a) : memory_resource{}, allocator_(std::move(a)) {}

    resource_adaptor_impl& operator=(const resource_adaptor_impl&) = default;

    allocator_type get_allocator() const { return allocator_; }

private:
    using value_type = typename std::allocator_traits<Alloc>::value_type;

    /// Returns the minimum alignment of allocations served by the underlying allocator.
    ///
    constexpr std::size_t allocator_natural_alignment() { return alignof(value_type); }

    /// Convert a size in bytes to a size in multiple of sizeof(T),
    /// such that the converted size is the smallest multiple with size >= bytesize.
    ///
    constexpr std::size_t value_type_size_from_bytesize(std::size_t bytesize)
    {
        return ((std::max(bytesize, std::size_t{1}) - std::size_t{1}) / sizeof(value_type)) + std::size_t{1};
    }

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        if (alignment <= allocator_natural_alignment())
        {
            return allocator_.allocate(value_type_size_from_bytesize(bytes));
        }
        else
        {
            // Extended alignment support
            //
            // we require the alignment to be at least the alignment of void*, so that orig_ptr actually
            // lives at a properly aligned address for its type
            alignment = std::max(alignof(void*), alignment);
            // request a block of extended size from the underlying allocator
            std::size_t const extended_bytes = bytes + sizeof(void*) + alignment - 1;
            void* const orig_ptr = allocator_.allocate(value_type_size_from_bytesize(extended_bytes));
            if (orig_ptr == nullptr)
            {
                return nullptr;
            }
            void* ret = score::cpp::pmr::detail::establish_extended_alignment(orig_ptr, bytes, extended_bytes, alignment);
            return ret;
        }
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
        if (alignment <= allocator_natural_alignment())
        {
            allocator_.deallocate(static_cast<typename allocator_type::value_type*>(p),
                                  value_type_size_from_bytesize(bytes));
        }
        else
        {
            // Extended alignment support
            //
            // recompute the extended size analogous to do_allocate()
            alignment = std::max(alignof(void*), alignment);
            std::size_t const extended_bytes = bytes + sizeof(void*) + alignment - 1;
            // the original pointer value returned by the underlying allocator is stored in the memory
            // immediately preceding the object
            void* orig_ptr = retrieve_unaligned_pointer(p);
            // free the original memory block
            allocator_.deallocate(static_cast<typename allocator_type::value_type*>(orig_ptr),
                                  value_type_size_from_bytesize(extended_bytes));
        }
    }

    bool do_is_equal(const memory_resource& other) const noexcept override
    {
        resource_adaptor_impl const* p = dynamic_cast<resource_adaptor_impl const*>(&other);
        return (p != nullptr) && (allocator_ == p->allocator_);
    }

private:
    Alloc allocator_;
};

} // namespace detail

/// A resource adaptor for wrapping C++11 Allocators in a polymorphic memory resource.
/// This is inspired by std::pmr::resource_adaptor from the ISO C++ Library Fundamentals TS2.
///
/// This can be used to allow polymorphic use of the Intel Safe C++ allocators as follows:
///\code{.c}
/// sstd::monotonic_allocation_strategy strategy{&storage, storage_size};
/// sstd::arena_allocator<int, decltype(strategy)> alloc{&strategy};
/// bmw::pmr::resource_adaptor<decltype(alloc)> pmr{alloc};
/// bmw::pmr::vector<int> v{&pmr};
///\endcode
///
///* Non-conforming: The base type for the underlying allocator is std::max_align_t instead of char. This is because
///                the memory overhead for small allocations is significant if the underlying allocator does have a
///                weak minimal alignment.
///
template <typename Allocator>
using resource_adaptor = detail::resource_adaptor_impl<
    typename std::allocator_traits<Allocator>::template rebind_alloc<detail::resource_adaptor_base_type>>;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_RESOURCE_ADAPTOR_HPP
