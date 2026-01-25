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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MEMORY_RESOURCE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MEMORY_RESOURCE_HPP

#include <score/assert.hpp>

#include <cstddef>

namespace score::cpp
{
namespace pmr
{

/// A polymorphic memory resource in the style of C++17 std::pmr::memory_resource.
/// See https://en.cppreference.com/w/cpp/memory/memory_resource for details.
///
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
class memory_resource
{
    static constexpr std::size_t max_align = alignof(std::max_align_t);

public:
    memory_resource() = default;
    memory_resource(const memory_resource&) = default;
    memory_resource& operator=(const memory_resource&) = default;
    virtual ~memory_resource() = default;

    ///
    ///\pre alignment must be a power of two.
    ///
    void* allocate(std::size_t bytes, std::size_t alignment = max_align)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(((alignment - 1) & alignment) == 0);
        void* const p{do_allocate(bytes, alignment)};
        SCORE_LANGUAGE_FUTURECPP_ASSERT(p != nullptr);
        return p;
    }

    ///
    ///\pre p must have been returned from a prior call to allocate(bytes, alignment) on a
    ///     memory resource equal to *this, and the must not have been deallocated since.
    ///
    void deallocate(void* p, std::size_t bytes, std::size_t alignment = max_align)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(p != nullptr); // standard says `p` must come from `.allocate()` which never returns `nullptr`
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(((alignment - 1) & alignment) == 0);
        return do_deallocate(p, bytes, alignment);
    }

    bool is_equal(const memory_resource& other) const noexcept { return do_is_equal(other); }

private:
    virtual void* do_allocate(std::size_t bytes, std::size_t alignment) = 0;
    virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) = 0;
    virtual bool do_is_equal(const memory_resource& other) const noexcept = 0;
};

inline bool operator==(const memory_resource& lhs, const memory_resource& rhs)
{
    return (&lhs == &rhs) || lhs.is_equal(rhs);
}

inline bool operator!=(const memory_resource& lhs, const memory_resource& rhs) { return !(lhs == rhs); }

/// Returns a pointer to a resource that forwards each request to the global new/delete operators.
/// The new_delete_resource is a Singleton object.
memory_resource* new_delete_resource() noexcept;

/// Returns a pointer to a resource that throws std::bad_alloc on each allocation.
/// The null_memory_resource is a Singleton object.
memory_resource* null_memory_resource() noexcept;

/// \brief Gets the default memory resource pointer.
///
/// The default memory resource pointer is used by certain facilities when an explicit memory resource is not supplied.
/// The initial default memory resource pointer is the return value of score::cpp::pmr::new_delete_resource.
///
/// This function is thread-safe. Previous call to score::cpp::pmr::set_default_resource synchronizes with (see
/// std::memory_order) the subsequent score::cpp::pmr::get_default_resource calls.
///
memory_resource* get_default_resource() noexcept;

/// \brief Sets the default memory resource pointer.
///
/// \returns the previous value of the default memory resource pointer.
///
/// If `new_resource` is not null, sets the default memory resource pointer to `new_resource`; otherwise, sets the
/// default memory resource pointer to score::cpp::pmr::new_delete_resource().
///
/// The default memory resource pointer is used by certain facilities when an explicit memory resource is not supplied.
/// The initial default memory resource pointer is the return value of score::cpp::pmr::new_delete_resource.
///
/// This function is thread-safe. Every call to score::cpp::pmr::set_default_resource synchronizes with (see std::memory_order)
/// the subsequent score::cpp::pmr::set_default_resource and score::cpp::pmr::get_default_resource calls.
///
memory_resource* set_default_resource(memory_resource* new_resource) noexcept;

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_MEMORY_RESOURCE_HPP
