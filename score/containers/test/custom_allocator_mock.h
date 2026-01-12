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
#ifndef SCORE_LIB_CONTAINERS_CUSTOM_ALLOCATOR_MOCK_H
#define SCORE_LIB_CONTAINERS_CUSTOM_ALLOCATOR_MOCK_H

#include "score/memory/shared/offset_ptr.h"

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <gmock/gmock.h>

namespace score
{
namespace containers
{

/// \brief Mock for a CustomAllocatorMock needed for specific tests
/// \details Beware: We currently expect from custom allocators, that they deal with pointer types (like our
///          PolymorphicOffsetPtrAllocator does), which provide a get() method, to obtain the raw-pointer (which our
///          OffsetPtr supports). See also the detailed comment in DynamicArray::DynamicArrayDeleter::to_address()!
/// \tparam T element type to be allocated
template <typename T>
class CustomAllocatorMock
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using pointer = memory::shared::OffsetPtr<T>;

    MOCK_METHOD(T*, allocate, (size_type), (noexcept));
    MOCK_METHOD(void, deallocate, (T*, size_type), (noexcept));
    MOCK_METHOD(void, construct, (T*), (noexcept));
    MOCK_METHOD(void, destroy, (T*), (noexcept));
};

/// \brief We need this wrapper as allocators need to be copyable, but a google mock isn't!
/// \tparam T element type to be allocated
template <typename T>
class CustomAllocatorMockWrapper
{
  public:
    using value_type = typename CustomAllocatorMock<T>::value_type;
    using size_type = typename CustomAllocatorMock<T>::size_type;
    using pointer = typename CustomAllocatorMock<T>::pointer;

    explicit CustomAllocatorMockWrapper(CustomAllocatorMock<T>* mock) : mock_{mock} {}

    T* allocate(size_type num_of_elements)
    {
        return mock_->allocate(num_of_elements);
    }

    void deallocate(T* ptr, size_type num_of_elements)
    {
        mock_->deallocate(ptr, num_of_elements);
    }

    void construct(T* ptr)
    {
        mock_->construct(ptr);
    }

    void destroy(T* ptr)
    {
        mock_->destroy(ptr);
    }

  private:
    CustomAllocatorMock<T>* mock_;
};

}  // namespace containers
}  // namespace score

#endif  // SCORE_LIB_CONTAINERS_CUSTOM_ALLOCATOR_MOCK_H
