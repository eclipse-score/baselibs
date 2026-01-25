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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POLYMORPHIC_ALLOCATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POLYMORPHIC_ALLOCATOR_HPP

#include <score/private/memory/uninitialized_construct_using_allocator.hpp>
#include <score/private/memory_resource/memory_resource.hpp>
#include <score/private/utility/ignore.hpp>
#include <score/assert.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>

namespace score::cpp
{
namespace pmr
{

/// A polymorphic allocator in the style of C++17 std::pmr::polymorphic_allocator.
/// See https://en.cppreference.com/w/cpp/memory/polymorphic_allocator for details.
template <typename Tp = std::uint8_t>
class polymorphic_allocator // NOLINT(cppcoreguidelines-special-member-functions) Follows literally the C++ standard
{
public:
    using value_type = Tp;

    /// \brief Initialize the internal memory resource to score::cpp::pmr::get_default_resource().
    ///
    polymorphic_allocator() noexcept : resource_{score::cpp::pmr::get_default_resource()} {}

    /// Constructor.
    /// Initializes the internal memory resource to r.
    ///\pre r is non-null.
    ///\note This constructor provides an implicit conversion from memory_resource*.
    ///
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    polymorphic_allocator(memory_resource* r) : resource_(r) { SCORE_LANGUAGE_FUTURECPP_PRECONDITION(r != nullptr); }

    /// Converting constructor.
    /// Initializes the internal memory resource to the same internal memory resource used by rhs.
    ///
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor) follows C++ Standard
    polymorphic_allocator(const polymorphic_allocator<U>& rhs) noexcept : resource_(rhs.resource())
    {
    }

    polymorphic_allocator(const polymorphic_allocator&) = default;
    polymorphic_allocator& operator=(const polymorphic_allocator&) = delete;

    Tp* allocate(std::size_t n) { return static_cast<Tp*>(resource_->allocate(sizeof(Tp) * n, alignof(Tp))); }

    ///
    ///\pre ptr must have been returned by a previous call to allocate(n) on a polymorphic allocator equal to *this
    ///     and must not have been deallocated since.
    ///
    void deallocate(Tp* ptr, std::size_t n) { resource_->deallocate(ptr, sizeof(Tp) * n, alignof(Tp)); }

    /// Constructs an object of the given type U in allocated, but not initialized storage pointed to by p with the
    /// provided arguments.
    ///
    /// It does so by means of "uses-allocator construction", i.e., if the object is of a type that itself uses
    /// allocators, it passes this->resource() down to the constructed object.
    ///
    /// # Uses-allocator construction
    /// There are three conventions of passing an allocator alloc to a constructor of some type T:
    /// * if T does not use a compatible allocator (std::uses_allocator<T, Alloc>::value is false), then alloc is
    ///   ignored.
    /// * otherwise, std::uses_allocator<T, Alloc>::value is true, and
    ///   * if T uses the leading-allocator convention (is invocable as T(std::allocator_arg, alloc, args...)), then
    ///     uses-allocator construction uses this form.
    ///   * if T uses the trailing-allocator convention (is invocable as T(args..., alloc)), then uses-allocator
    ///     construction uses this form.
    ///   * otherwise, the program is ill-formed. (This means std::uses_allocator<T, Alloc>::value is true, but the type
    ///   does not follow either of the two allowed conventions.)
    /// * As a special case, std::pair is treated as a uses-allocator type even though std::uses_allocator is false for
    ///   pairs.
    ///
    /// # std::uses_allocator<T, Alloc>
    /// If T has a member typedef allocator_type which is convertible from Alloc, the member constant `value` is true.
    /// Otherwise `value` is false.
    ///
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        score::cpp::ignore = score::cpp::detail::uninitialized_construct_using_allocator(p, *this, std::forward<Args>(args)...);
    }

    memory_resource* resource() const { return resource_; }

    /// \brief Return a default-constructed `polymorphic_allocator` object.
    ///
    /// Called indirectly via `std::allocator_traits` by the copy constructors of all (pmr) containers.
    polymorphic_allocator select_on_container_copy_construction() const { return polymorphic_allocator{}; }

private:
    memory_resource* resource_;
};

template <typename Tp1, typename Tp2>
inline bool operator==(const polymorphic_allocator<Tp1>& lhs, const polymorphic_allocator<Tp2>& rhs)
{
    return *(lhs.resource()) == *(rhs.resource());
}

template <typename Tp1, typename Tp2>
inline bool operator!=(const polymorphic_allocator<Tp1>& lhs, const polymorphic_allocator<Tp2>& rhs)
{
    return !(lhs == rhs);
}

} // namespace pmr
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MEMORY_RESOURCE_POLYMORPHIC_ALLOCATOR_HPP
