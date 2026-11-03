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

#ifndef SCORE_SERIALIZATION_TYPES_LENGTH_PREFIXED_VECTOR_H
#define SCORE_SERIALIZATION_TYPES_LENGTH_PREFIXED_VECTOR_H

#include <score/assert.hpp>
#include <score/memory_resource.hpp>

#include <limits>
#include <vector>

namespace score
{
namespace serialization
{

template <typename InputIterator>
using RequireInputIterator =
    std::enable_if_t<std::is_convertible<typename std::iterator_traits<InputIterator>::iterator_category,
                                         std::input_iterator_tag>::value>;

// Special type for supporting n-byte length-prefixed vector of data
template <typename T, typename LengthT, typename Allocator = score::cpp::pmr::polymorphic_allocator<T>>
class LengthPrefixedVector
{
  public:
    using size_type = LengthT;
    using allocator_type = typename std::vector<T, Allocator>::allocator_type;
    using iterator = typename std::vector<T, Allocator>::iterator;
    using const_iterator = typename std::vector<T, Allocator>::const_iterator;
    using pointer = typename std::vector<T, Allocator>::pointer;
    using const_pointer = typename std::vector<T, Allocator>::const_pointer;
    using reverse_iterator = typename std::vector<T, Allocator>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T, Allocator>::const_reverse_iterator;
    using reference = typename std::vector<T, Allocator>::reference;
    using const_reference = typename std::vector<T, Allocator>::const_reference;
    using value_type = typename std::vector<T, Allocator>::value_type;
    using difference_type = typename std::vector<T, Allocator>::difference_type;

    LengthPrefixedVector() = default;
    ~LengthPrefixedVector() = default;
    explicit LengthPrefixedVector(const allocator_type& allocator) : vec(allocator) {}

    explicit LengthPrefixedVector(size_type size, const allocator_type& allocator = allocator_type{})
        : vec(static_cast<typename std::vector<T, Allocator>::size_type>(size), allocator)
    {
    }

    LengthPrefixedVector(size_type size,
                         const value_type& initial_value,
                         const allocator_type& allocator = allocator_type{})
        : vec(static_cast<typename std::vector<T, Allocator>::size_type>(size), initial_value, allocator)
    {
    }

    LengthPrefixedVector(std::initializer_list<T> list, const allocator_type& allocator = allocator_type{})
        : vec(list, allocator)
    {
    }

    template <class InputIterator, typename = RequireInputIterator<InputIterator>>
    LengthPrefixedVector(InputIterator first, InputIterator last, const allocator_type& allocator = allocator_type{})
        : vec(first, last, allocator)
    {
    }

    // Suppress AUTOSAR C++14 A12-8-1" rule finding. The rule states "Move and copy constructors shall move and
    // respectively copy base classes and data members of a class, without any side effects". std::vector involves
    // allocating memory and copying elements, which is expected behavior.
    // coverity[autosar_cpp14_a12_8_1_violation]
    LengthPrefixedVector(const LengthPrefixedVector& x) = default;
    LengthPrefixedVector(const LengthPrefixedVector& x, const allocator_type& allocator) : vec{x.vec, allocator} {}

    LengthPrefixedVector(LengthPrefixedVector&& x) noexcept = default;
    LengthPrefixedVector(LengthPrefixedVector&& x, const allocator_type& allocator) : vec{std::move(x.vec), allocator}
    {
    }

    // NOLINTNEXTLINE(google-explicit-constructor): This class is a wrapper around a std::vector, thus desired.
    LengthPrefixedVector(const std::vector<T, Allocator>& x) : vec{x} {}
    LengthPrefixedVector(const std::vector<T, Allocator>& x, const allocator_type& allocator) : vec{x, allocator} {}

    // NOLINTNEXTLINE(google-explicit-constructor): This class is a wrapper around a std::vector, thus desired.
    LengthPrefixedVector(std::vector<T, Allocator>&& x) : vec{std::move(x)} {}
    LengthPrefixedVector(std::vector<T, Allocator>&& x, const allocator_type& allocator) : vec{std::move(x), allocator}
    {
    }

    LengthPrefixedVector& operator=(const LengthPrefixedVector&) = default;
    LengthPrefixedVector& operator=(LengthPrefixedVector&&) noexcept = default;

    LengthPrefixedVector& operator=(const std::vector<T, Allocator>& x) &
    {
        vec = x;
        return *this;
    }

    LengthPrefixedVector& operator=(std::vector<T, Allocator>&& x) &
    {
        vec = std::move(x);
        return *this;
    }

    LengthPrefixedVector& operator=(std::initializer_list<value_type> list) &
    {
        vec = list;
        return *this;
    }

    size_type size() const noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE((vec.size() <= std::numeric_limits<size_type>::max()),
                                                    "size() exceeds values for chosen LengthT");
        // Suppress "AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
        // loss." In this context, there is no data loss because the assert above ensures the value is in range.
        // coverity[autosar_cpp14_a4_7_1_violation]
        return static_cast<size_type>(vec.size());
    }

    allocator_type get_allocator() const noexcept
    {
        return vec.get_allocator();
    }

    iterator begin() noexcept
    {
        return vec.begin();
    }
    const_iterator begin() const noexcept
    {
        return vec.begin();
    }
    iterator end() noexcept
    {
        return vec.end();
    }
    const_iterator end() const noexcept
    {
        return vec.end();
    }

    reverse_iterator rbegin() noexcept
    {
        return vec.rbegin();
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return vec.rbegin();
    }
    reverse_iterator rend() noexcept
    {
        return vec.rend();
    }
    const_reverse_iterator rend() const noexcept
    {
        return vec.rend();
    }
    const_iterator cbegin() const noexcept
    {
        return vec.cbegin();
    }
    const_iterator cend() const noexcept
    {
        return vec.cend();
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return vec.crbegin();
    }
    const_reverse_iterator crend() const noexcept
    {
        return vec.crend();
    }

    template <class InputIterator, typename = RequireInputIterator<InputIterator>>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
        return vec.insert(position, first, last);
    }

    void push_back(const value_type& value)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE((vec.size() < std::numeric_limits<size_type>::max()),
                                                    "push_back(): too many values for given length type");
        vec.push_back(value);
    }
    void push_back(value_type&& value)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE((vec.size() < std::numeric_limits<size_type>::max()),
                                                    "push_back(): too many values for given length type");
        vec.push_back(std::move(value));
    }
    void clear() noexcept
    {
        vec.clear();
    }
    void resize(size_type new_size)
    {
        vec.resize(static_cast<typename std::vector<T, Allocator>::size_type>(new_size));
    }
    void resize(size_type new_size, const_reference initial_value)
    {
        vec.resize(static_cast<typename std::vector<T, Allocator>::size_type>(new_size), initial_value);
    }

    reference operator[](size_type n)
    {
        return vec[n];
    }
    const_reference operator[](size_type n) const
    {
        return vec[n];
    }
    reference at(size_type n)
    {
        return vec.at(n);
    }
    const_reference at(size_type n) const
    {
        return vec.at(n);
    }

    value_type* data() noexcept
    {
        return vec.data();
    }
    const value_type* data() const noexcept
    {
        return vec.data();
    }

  private:
    std::vector<T, Allocator> vec;
};

template <typename T, typename LengthT>
inline bool operator==(const LengthPrefixedVector<T, LengthT>& lhs,
                       const LengthPrefixedVector<T, LengthT>& rhs) noexcept
{
    return ((lhs.size() == rhs.size()) && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_TYPES_LENGTH_PREFIXED_VECTOR_H
