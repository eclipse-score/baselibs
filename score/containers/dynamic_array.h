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
#ifndef SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H
#define SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H

#include "score/containers/non_relocatable_vector.h"

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <cstddef>
#include <cstring>
#include <memory>

namespace score::containers
{

namespace test
{
template <typename ElementType, typename Allocator>
class DynamicArrayTestAttorney;
}

/// \brief Fixed-size array data structure whose size can be dynamically set at construction.
///
/// The DynamicArray should be used if an array-like data structure is needed whose size is not known at compile time
/// but only needs to be set once (i.e. on construction of DynamicArray) and never resized. All elements are initialised
/// on construction.
///
///
/// \details The implementation also provides some "performance optimizations" in its iterator handling:
/// It makes sure to return "raw-pointers" as its begin/end-iterators. E.g. independent from the pointer-type, the
/// underlying Allocator provides, it doesn't return these pointer-types 1:1 as begin/end iterator, but returns
/// raw-pointers instead. This is a performance benefit, when we use DynamicArray in shared-memory, where we use
/// score::memory::shared::PolymorphicOffsetPtrAllocator as our allocator, which creates fancy OffsetPtr.
/// Dereferencing an OffsetPtr is expensive as it applies bounds-checking for safety-reasons. But when iterating over
/// a DynamicArray residing in shared-memory, doing a bounds-check for every element is unnecessary. Only the 1st/last
/// OffsetPtr needs to be checked. This is exactly the iterator optimization applied here: Only the start/end gets
/// bounds-checked and if this is successful a raw-pointer is handed out as an iterator.
template <typename ElementType, typename Allocator = std::allocator<ElementType>>
class DynamicArray
{
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The 'friend' class is employed to encapsulate non-public members. Friend classes provide controlled access to
    // private members, utilized internally, ensuring that end users cannot access implementation specifics. In this
    // case, it is used for testing only in order to simulate corruption of the underlying pointer type to the dynamic
    // storage, which definitely should not be in the public interface.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class test::DynamicArrayTestAttorney<ElementType, Allocator>;

  public:
    using value_type = ElementType;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;
    using size_type = std::size_t;

    /// \brief Constructor which default initialises the elements underlying array of size array_size
    /// \param number_of_elements The size of the underlying array
    /// \param alloc allocator instance to be used, defaults to default constructed allocator of type Allocator

    // In this case, we can't delegate to the default-value extended ctor and vice versa as the dynamic_array_
    // initialization is specific to each case.
    // coverity[autosar_cpp14_a12_1_5_violation]
    explicit DynamicArray(const size_type number_of_elements, Allocator alloc = Allocator())
        : non_relocatable_vector_(number_of_elements, alloc)
    {
        for (std::size_t i = 0U; i < number_of_elements; ++i)
        {
            non_relocatable_vector_.emplace_back();
        }
    }

    /// \brief Constructor which creates a DynamicArray with number_of_elements copies of elements with value value.
    /// \attention Obviously in this case ElementType needs to be copyable.
    /// \param number_of_elements The size of the underlying array
    /// \param value the value to copy elements from
    /// \param alloc allocator instance to be used, defaults to default constructed allocator of type Allocator
    explicit DynamicArray(const size_type number_of_elements, const ElementType& value, Allocator alloc = Allocator())
        : non_relocatable_vector_(number_of_elements, alloc)
    {
        for (std::size_t i = 0U; i < number_of_elements; ++i)
        {
            non_relocatable_vector_.emplace_back(value);
        }
    }

    ElementType& at(const size_type index)
    {
        return non_relocatable_vector_.at(index);
    }

    const ElementType& at(const size_type index) const
    {
        return non_relocatable_vector_.at(index);
    }

    ElementType& operator[](const size_type index)
    {
        return non_relocatable_vector_[index];
    }

    const ElementType& operator[](const size_type index) const
    {
        return non_relocatable_vector_[index];
    }

    size_type size() const noexcept
    {
        return non_relocatable_vector_.size();
    }

    value_type* data()
    {
        return non_relocatable_vector_.data();
    }

    const value_type* data() const
    {
        return non_relocatable_vector_.data();
    }

    /// \brief returns iterator (pointer) to the 1st element of the dynamic array
    /// \attention The returned iterator (pointer) isn't suited to be placed into shared-memory! I.e. even if the
    /// underlying allocator is a shared-memory enabled alloc providing fancy/offset-ptr, the returned iterator is
    /// semantically a raw-pointer.
    iterator begin()
    {
        return non_relocatable_vector_.begin();
    }
    const_iterator begin() const
    {
        return non_relocatable_vector_.begin();
    }
    const_iterator cbegin() const
    {
        return non_relocatable_vector_.cbegin();
    }

    /// \brief returns past the end iterator (pointer) of the dynamic array
    iterator end()
    {
        return non_relocatable_vector_.end();
    }
    const_iterator end() const
    {
        return non_relocatable_vector_.end();
    }
    const_iterator cend() const
    {
        return non_relocatable_vector_.cend();
    }

  private:
    NonRelocatableVector<ElementType, Allocator> non_relocatable_vector_;
};

}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_DYNAMIC_ARRAY_H
