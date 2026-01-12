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
#ifndef SCORE_LIB_CONTAINERS_NON_RELOCATABLE_VECTOR_H
#define SCORE_LIB_CONTAINERS_NON_RELOCATABLE_VECTOR_H

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <cstddef>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace score::containers
{

namespace test
{
template <typename ElementType, typename Allocator>
class NonRelocatableVectorAttorney;
}

namespace detail
{

template <class T>
constexpr T* to_address(T* p)
{
// Suppress "AUTOSAR C++14 A16-0-1", The rule states:
// "The pre-processor shall only be used for unconditional and conditional file
// inclusion and include guards, and using the following directives: (1) #ifndef,
// (2) #ifdef, (3) #if, (4) #if defined, (5) #elif, (6) #else, (7) #define, (8) #endif,
// (9) #include."
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#if __cplusplus < 202002L
    // to_address is provided in C++20
    static_assert(!std::is_function_v<T>);
    return p;
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    return std::to_address(p);
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
}

template <class T>
constexpr auto to_address(const T& p)
{
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#if __cplusplus < 202002L
    // to_address is provided in C++20
    return to_address(p.operator->());
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    return std::to_address(p);
// Suppress "AUTOSAR C++14 A16-0-1"
// This check is needed as to_address is available in C++ version 20 and above.
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
}

}  // namespace detail

/// \brief Fixed-capacity vector whose capacity is set on construction and never changed.
///
/// The NonRelocatableVector is a std::vector-like structure which guarantees that no reallocation will ever be made (so
/// pointers or references to elements will never be invalidated. Memory for the capacity specified is allocated on
/// construction and each element is initialised when inserting an element (e.g. via emplace_back).
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
class NonRelocatableVector
{
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // The 'friend' class is employed to encapsulate non-public members. Friend classes provide controlled access to
    // private members, utilized internally, ensuring that end users cannot access implementation specifics. In this
    // case, it is used for testing only in order to simulate corruption of the underlying pointer type to the dynamic
    // storage, which definitely should not be in the public interface.
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class test::NonRelocatableVectorAttorney<ElementType, Allocator>;

    using ptr_index_type =
        typename std::pointer_traits<typename std::allocator_traits<Allocator>::pointer>::difference_type;

  public:
    using value_type = ElementType;
    using reference = ElementType&;
    using const_reference = const ElementType&;
    using iterator = ElementType*;
    using const_iterator = const ElementType*;
    using size_type = std::size_t;

    /// \brief Constructor which reserves memory for the provided number of elements
    /// \param number_of_elements_to_reserve The capacity of the underlying array
    /// \param alloc allocator instance to be used, defaults to default constructed allocator of type Allocator
    explicit NonRelocatableVector(const size_type number_of_elements_to_reserve, Allocator alloc = Allocator())
        : non_relocatable_vector_{allocate_array(number_of_elements_to_reserve, alloc)},
          alloc_{std::move(alloc)},
          size_{0U},
          capacity_{number_of_elements_to_reserve}
    {
    }

    NonRelocatableVector(const NonRelocatableVector& other);
    NonRelocatableVector(NonRelocatableVector&& other) noexcept;

    NonRelocatableVector& operator=(const NonRelocatableVector& other) = delete;
    NonRelocatableVector& operator=(NonRelocatableVector&& other) noexcept;

    ~NonRelocatableVector() noexcept;

    template <typename... Args>
    auto emplace_back(Args&&... args) -> ElementType&;

    ElementType& at(const size_type index);

    const ElementType& at(const size_type index) const;

    ElementType& operator[](const size_type index);

    const ElementType& operator[](const size_type index) const;

    size_type size() const noexcept
    {
        return size_;
    }

    size_type capacity() const noexcept
    {
        return capacity_;
    }

    value_type* data();

    const value_type* data() const;

    /// \brief returns iterator (pointer) to the 1st element of the dynamic array
    /// \attention The returned iterator (pointer) isn't suited to be placed into shared-memory! I.e. even if the
    /// underlying allocator is a shared-memory enabled alloc providing fancy/offset-ptr, the returned iterator is
    /// semantically a raw-pointer.
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    /// \brief returns past the end iterator (pointer) of the dynamic array
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

  private:
    using pointer = typename std::allocator_traits<Allocator>::pointer;

    static pointer allocate_array(size_type number_of_elements, Allocator& allocator);

    iterator GetPastTheEndIterator() const;

    /// \brief Gets an iterator to the first element of the array. Returns nullptr if size_ == 0
    ///
    /// Function is static to ensure that no member variables are modified by another process (in case the
    /// NonRelocatableVector is in shared memory) while getting the iterator
    static auto GetFirstElement(const pointer dynamic_array, const std::size_t non_relocatable_vector_size) -> iterator;

    /// \brief Gets an iterator to the last element of the array. Returns nullptr if size_ == 0
    ///
    /// Function is static to ensure that no member variables are modified by another process (in case the
    /// NonRelocatableVector is in shared memory) while getting the iterator
    static auto GetLastElement(const pointer dynamic_array, const std::size_t non_relocatable_vector_size) -> iterator;

    constexpr void swap(NonRelocatableVector<ElementType, Allocator>& other) noexcept
    {
        // Use std::swap if no other swap is found using ADL
        using std::swap;

        swap(size_, other.size_);
        swap(alloc_, other.alloc_);
        swap(non_relocatable_vector_, other.non_relocatable_vector_);
        swap(capacity_, other.capacity_);
    }

    friend constexpr void swap(NonRelocatableVector<ElementType, Allocator>& lhs,
                               NonRelocatableVector<ElementType, Allocator>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    typename std::allocator_traits<Allocator>::pointer non_relocatable_vector_;
    Allocator alloc_;
    size_type size_;
    size_type capacity_;
};

template <typename ElementType, typename Allocator>
// false-positive: size_t and pointer is trivial, standard allocator move is noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
// coverity[autosar_cpp14_a12_8_1_violation] false-positive
NonRelocatableVector<ElementType, Allocator>::NonRelocatableVector(NonRelocatableVector&& other) noexcept
    // coverity[autosar_cpp14_a12_8_4_violation] false-positive: no copy semantic is used
    : non_relocatable_vector_{std::move(other.non_relocatable_vector_)},
      alloc_{std::move(other.alloc_)},
      size_{std::move(other.size_)},
      capacity_{std::move(other.capacity_)}
{
    other.non_relocatable_vector_ = nullptr;
}

template <typename ElementType, typename Allocator>
NonRelocatableVector<ElementType, Allocator>::NonRelocatableVector(const NonRelocatableVector& other)
    : non_relocatable_vector_{nullptr}, alloc_{other.alloc_}, size_{other.size_}, capacity_{other.capacity_}
{
    using alloc_traits = std::allocator_traits<Allocator>;
    auto storage = allocate_array(capacity_, alloc_);

    // To avoid costly bounds checking when dereferencing cur_pointer (if the allocator is using OffsetPtr), we iterate
    // over raw pointers to the allocated storage.
    auto cur_pointer = detail::to_address(storage);
    auto source_it = other.cbegin();

    for (size_t i = 0U; i < size_; i++)
    {
        // Suppress "AUTOSAR C++14 A5-3-2" rule finding.
        // This rule states: "Null pointers shall not be dereferenced.".
        // Justification: The above check ensures the memory is allocated and not null.
        // Therefore, dereferencing "cur_pointer" is safe.
        // coverity[autosar_cpp14_a5_3_2_violation]
        alloc_traits::construct(alloc_, cur_pointer, *source_it);
        source_it++;
        std::advance(cur_pointer, 1);
    }

    non_relocatable_vector_ = storage;
}

template <typename ElementType, typename Allocator>
// false-positive: size_t and pointer is trivial, standard allocator move is noexcept
// coverity[autosar_cpp14_a15_5_3_violation]
// coverity[autosar_cpp14_a6_2_1_violation] false-positive: no side effects
auto NonRelocatableVector<ElementType, Allocator>::operator=(NonRelocatableVector&& other) noexcept
    -> NonRelocatableVector&
{
    if (this != &other)
    {
        swap(other);
    }
    return *this;
}

template <typename ElementType, typename Allocator>
inline auto NonRelocatableVector<ElementType, Allocator>::allocate_array(size_type number_of_elements,
                                                                         Allocator& allocator) -> pointer
{
    auto storage = std::allocator_traits<Allocator>::allocate(allocator, number_of_elements);
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(nullptr != storage, "no memory allocated");
    return storage;
}

template <typename ElementType, typename Allocator>
NonRelocatableVector<ElementType, Allocator>::~NonRelocatableVector() noexcept
{
    if (non_relocatable_vector_ == nullptr)
    {
        return;
    }
    using alloc_traits = std::allocator_traits<decltype(alloc_)>;
    auto cur_pointer = non_relocatable_vector_;
    for (size_type i = 0U; i < size_; i++)
    {
        alloc_traits::destroy(alloc_, detail::to_address(cur_pointer));
        std::advance(cur_pointer, 1);
    }
    alloc_traits::deallocate(alloc_, non_relocatable_vector_, capacity_);
}

template <typename ElementType, typename Allocator>
template <typename... Args>
auto NonRelocatableVector<ElementType, Allocator>::emplace_back(Args&&... args) -> ElementType&
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(
        size_ < capacity_,
        "Capacity of vector set in constructor has already been reached. Cannot emplace another element.");

    auto* current_storage_pointer = detail::to_address(non_relocatable_vector_);
    std::advance(current_storage_pointer, size_);
    std::allocator_traits<Allocator>::construct(alloc_, current_storage_pointer, std::forward<Args>(args)...);
    size_ += 1U;
    return *current_storage_pointer;
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a15_5_3_violation] only precondition may throw, which is intentional
inline ElementType& NonRelocatableVector<ElementType, Allocator>::at(const size_type index)
{
    // check that this element is in shared memory.
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < size_, "index out of bounds");
    // Suppress "AUTOSAR C++14 A9-3-1" rule finding. This rule states: "An object shall not be accessed outside of
    // its lifetime.". Suppress "AUTOSAR C++14 A9-3-1" rule finding. This rule states:"Member functions shall not
    // return non-const “raw” pointers or references to private or protected data owned by the class.". Suppress
    // "AUTOSAR C++14 M7-5-1" rule finding. This rule states: "A function shall not return a reference or a pointer
    // to an automatic variable (including parameters), defined within the function.". Suppress "AUTOSAR C++14
    // M7-5-2" rule finding. This rule states: "The address of an object with automatic storage shall not be
    // assigned to another object that may persist after the first object has ceased to exist.". Justification: As
    // with a normal container, an underlying item should be accessible as a non-const reference from outside by
    // index. The result reference remains valid as long as the NonRelocatableVector instance is alive.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_a9_3_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    // coverity[autosar_cpp14_m7_5_2_violation]
    return *(std::next(non_relocatable_vector_, static_cast<ptr_index_type>(index)));
}

template <typename ElementType, typename Allocator>
inline const ElementType& NonRelocatableVector<ElementType, Allocator>::at(const size_type index) const
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD_MESSAGE(index < size_, "index out of bounds");
    // Suppress "AUTOSAR C++14 A3-8-1" rule finding.
    // This rule states: "An object shall not be accessed outside of its lifetime.".
    // Suppress "AUTOSAR C++14 M7-5-1" rule finding.
    // This rule states:"A function shall not return a reference or a pointer to an automatic
    // variable (including parameters), defined within the function.".
    // Suppress "AUTOSAR C++14 M7-5-2" rule finding. This rule states: "The address of an object with automatic
    // storage shall not be assigned to another object that may persist after the first object has ceased to
    // exist.". Justification: The index is checked to be within bounds, ensuring the object is within its lifetime;
    // As with a normal container, an underlying item should be accessible by reference from outside by index.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    // coverity[autosar_cpp14_m7_5_2_violation]
    return *(std::next(non_relocatable_vector_, static_cast<ptr_index_type>(index)));
}

template <typename ElementType, typename Allocator>
inline ElementType& NonRelocatableVector<ElementType, Allocator>::operator[](const size_type index)
{
    return this->at(index);
}

template <typename ElementType, typename Allocator>
inline const ElementType& NonRelocatableVector<ElementType, Allocator>::operator[](const size_type index) const
{
    return this->at(index);
}

template <typename ElementType, typename Allocator>
inline auto NonRelocatableVector<ElementType, Allocator>::data() -> value_type*
{
    // If the NonRelocatableVector is in shared memory, then non_relocatable_vector_ may be corrupted by another
    // process while this function is running. Therefore, we make a stack copy of it at the start of the function
    // and use that copy for all checks and for generating the final iterator. Theoretically, the size_ variable
    // could also be corrupted. However, in that case, the bounds checking will not be affected.
    const auto non_relocatable_vector_copy = non_relocatable_vector_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying
    // array without terminating. Since the array is contiguous, then as long as the first and last elements are
    // within the correct bounds, the whole array is.
    score::cpp::ignore = GetLastElement(non_relocatable_vector_copy, size_);
    return GetFirstElement(non_relocatable_vector_copy, size_);
}

template <typename ElementType, typename Allocator>
inline auto NonRelocatableVector<ElementType, Allocator>::data() const -> const value_type*
{
    // If the NonRelocatableVector is in shared memory, then non_relocatable_vector_ may be corrupted by another
    // process while this function is running. Therefore, we make a stack copy of it at the start of the function
    // and use that copy for all checks and for generating the final iterator. Theoretically, the size_ variable
    // could also be corrupted. However, in that case, the bounds checking will not be affected.
    const auto non_relocatable_vector_copy = non_relocatable_vector_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying
    // array without terminating. Since the array is contiguous, then as long as the first and last elements are
    // within the correct bounds, the whole array is.
    score::cpp::ignore = GetLastElement(non_relocatable_vector_copy, size_);
    return GetFirstElement(non_relocatable_vector_copy, size_);
}

template <typename ElementType, typename Allocator>
auto NonRelocatableVector<ElementType, Allocator>::begin() -> iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
auto NonRelocatableVector<ElementType, Allocator>::cbegin() const -> const_iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
auto NonRelocatableVector<ElementType, Allocator>::begin() const -> const_iterator
{
    return this->data();
}

template <typename ElementType, typename Allocator>
inline auto NonRelocatableVector<ElementType, Allocator>::end() -> iterator
{
    return GetPastTheEndIterator();
}

template <typename ElementType, typename Allocator>
auto NonRelocatableVector<ElementType, Allocator>::end() const -> const_iterator
{
    return static_cast<const_iterator>(GetPastTheEndIterator());
}

template <typename ElementType, typename Allocator>
auto NonRelocatableVector<ElementType, Allocator>::cend() const -> const_iterator
{
    return static_cast<const_iterator>(GetPastTheEndIterator());
}

template <typename ElementType, typename Allocator>
inline auto NonRelocatableVector<ElementType, Allocator>::GetPastTheEndIterator() const -> iterator
{
    // If the NonRelocatableVector is in shared memory, then non_relocatable_vector_ may be corrupted by another
    // process while this function is running. Therefore, we make a stack copy of it at the start of the function
    // and use that copy for all checks and for generating the final iterator.
    const auto non_relocatable_vector_copy = non_relocatable_vector_;

    // Certain allocators use pointers which perform bounds checking (e.g. PolymorphicOffsetPtrAllocator and
    // OffsetPtrs). For these, it's important that we can get both the first and last elements of the underlying
    // array without terminating. Since the array is contiguous, then as long as the first and last elements are
    // within the correct bounds, the whole array is.
    score::cpp::ignore = GetFirstElement(non_relocatable_vector_copy, size_);

    auto last_element_raw_iterator = GetLastElement(non_relocatable_vector_copy, size_);
    if (last_element_raw_iterator == nullptr)
    {
        return nullptr;
    }
    auto past_the_end_raw_iterator = std::next(last_element_raw_iterator, 1);

    // Suppress "AUTOSAR C++14 A3-8-1" rule finding.
    // This rule states: "An object shall not be accessed outside of its lifetime.".
    // Suppress "AUTOSAR C++14 M7-5-1" rule finding.
    // This rule states: "A function shall not return a reference or a pointer to an automatic
    // variable (including parameters), defined within the function.".
    // Justification: The local variable "past_the_end_pointer" is a pointer to an internal member.
    // This does not cause the object to outlive its intended scope.
    // coverity[autosar_cpp14_a3_8_1_violation]
    // coverity[autosar_cpp14_m7_5_1_violation]
    return ::score::containers::detail::to_address(past_the_end_raw_iterator);
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a0_1_3_violation] false-positive: used in data(), GetPastTheEndIterator()
inline auto NonRelocatableVector<ElementType, Allocator>::GetFirstElement(const pointer dynamic_array,
                                                                          const std::size_t non_relocatable_vector_size)
    -> iterator
{
    if (non_relocatable_vector_size == 0U)
    {
        return nullptr;
    }
    return ::score::containers::detail::to_address(dynamic_array);
}

template <typename ElementType, typename Allocator>
// coverity[autosar_cpp14_a0_1_3_violation] false-positive: used in data(), GetPastTheEndIterator()
inline auto NonRelocatableVector<ElementType, Allocator>::GetLastElement(const pointer dynamic_array,
                                                                         const std::size_t non_relocatable_vector_size)
    -> iterator
{
    if (non_relocatable_vector_size == 0U)
    {
        return nullptr;
    }
    // cast is safe due to 0 check
    // coverity[autosar_cpp14_m5_0_9_violation]
    auto last_element_fancy_pointer =
        std::next(dynamic_array, static_cast<ptr_index_type>(non_relocatable_vector_size - 1U));
    return ::score::containers::detail::to_address(last_element_fancy_pointer);
}

}  // namespace score::containers

#endif  // SCORE_LIB_CONTAINERS_NON_RELOCATABLE_VECTOR_H
