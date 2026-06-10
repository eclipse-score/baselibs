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
#ifndef SCORE_LIB_CONTAINERS_TEST_MOCKABLE_ALLOCATOR_H
#define SCORE_LIB_CONTAINERS_TEST_MOCKABLE_ALLOCATOR_H

#include "score/containers/test/mockable_pointer.h"

#include <cstddef>
#include <memory>

namespace score::containers::test
{

/// \brief An allocator wrapper around std::allocator that returns MockablePointer<T> as its pointer type.
///
/// This allocator behaves exactly like std::allocator but uses MockablePointer<T> instead of raw T*.
/// This allows tests of allocator aware containers (like NonRelocatableVector) with real allocation/deallocation
/// behavior during normal operations (f.i. construction, emplace_back, and destruction), while still being able to
/// register a PointerSpyMock on MockablePointer<T> to verify pointer operations during specific method calls
/// (e.g. data()).
///
/// Usage:
///   // Construction and fill work without any mock setup:
///   NonRelocatableVector<uint32_t, MockableAllocator<uint32_t>> vec{10};
///   vec.emplace_back(42);
///
///   // Only set the spy when you want to verify pointer interactions:
///   PointerSpyMock<uint32_t> spy;
///   MockablePointerMockGuard<uint32_t> guard{&spy};
///   EXPECT_CALL(spy, PlusAssign(9));
///   vec.data();
template <typename T>
class MockableAllocator
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = MockablePointer<T>;

    MockableAllocator() noexcept = default;

    template <typename U>
    // Rationale : Non-explicit constructor is needed for implicit conversions in rebind cases.
    // NOLINTNEXTLINE(google-explicit-constructor) - standard allocator rebind pattern
    MockableAllocator(const MockableAllocator<U>& /*unused*/) noexcept
    {
    }

    pointer allocate(size_type n)
    {
        T* raw = std_alloc_.allocate(n);
        return MockablePointer<T>(raw);
    }

    void deallocate(pointer ptr, size_type n)
    {
        std_alloc_.deallocate(ptr.get(), n);
    }

    bool operator==(const MockableAllocator& /*unused*/) const noexcept
    {
        return true;
    }

    bool operator!=(const MockableAllocator& /*unused*/) const noexcept
    {
        return false;
    }

  private:
    std::allocator<T> std_alloc_{};
};

}  // namespace score::containers::test

#endif  // SCORE_LIB_CONTAINERS_TEST_MOCKABLE_ALLOCATOR_H

