/********************************************************************************
 * Copyright (c) 2016 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2016 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_CIRCULAR_BUFFER_ITERATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_CIRCULAR_BUFFER_ITERATOR_HPP

#include <score/private/iterator/size.hpp>
#include <score/assert.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

/// \brief Implements a custom random access iterator.
///
/// The iterator has the following requirements on the Container interface:
/// - A member function "size_type size() const;" which returns the number of elements
/// - A member function "reference operator[]" which allows random access into the container
/// - A member function "const_reference operator[] const" which allows random access into the container
///
/// \tparam Container Container for which the iterator is constructed.
/// \tparam is_constant True defines a const iterator, non-const iterator otherwise.
template <typename Container, bool IsConstant = false>
class circular_buffer_iterator
{
    static_assert(std::is_object<Container>::value, "Container is not an object type");
    static_assert(!std::is_const<Container>::value, "Container must not be const-qualified");

public:
    /// \brief The type of the values that can be obtained by dereferencing the iterator. Defined by
    /// std::iterator_traits.
    using value_type = typename Container::value_type;

    /// \brief Defines a reference to the type iterated over. Defined by std::iterator_traits.
    using reference =
        std::conditional_t<IsConstant, typename Container::const_reference, typename Container::reference>;

    /// \brief Defines a pointer to the type iterated over. Defined by std::iterator_traits.
    using pointer = std::conditional_t<IsConstant, typename Container::const_pointer, typename Container::pointer>;

    /// \brief The type that can be used to identify distance between iterators. Defined by std::iterator_traits.
    using difference_type = std::ptrdiff_t;

    /// \brief The category of the iterator. Defined by std::iterator_traits.
    using iterator_category = std::random_access_iterator_tag;

    /// \brief Constructs a new iterator.
    ///
    /// \param container Reference to the container for which the iterator is constructed.
    /// \param offset Initial offset of the iterator with respect to the begin of the container.
    /// \{
    circular_buffer_iterator(Container& container, const bool is_begin)
        : current_(is_begin ? 0 : score::cpp::ssize(container)), container_(&container)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(current_ >= 0);
    }
    circular_buffer_iterator(const Container& container, const bool is_begin)
        : current_(is_begin ? 0 : score::cpp::ssize(container)), container_(&container)
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(current_ >= 0);
    }
    /// \}

    // Forward iterator requirements

    /// \brief Access the pointed-to-element.
    ///
    /// \return Reference to the current element.
    reference operator*() const { return checked_access(current_); }

    /// \brief Access the pointed-to-element.
    ///
    /// \return Pointer to the current element.
    pointer operator->() const { return &checked_access(current_); }

    /// \brief Pre-increments by one.
    ///
    /// \return The incremented iterator.
    circular_buffer_iterator& operator++()
    {
        ++current_;
        return *this;
    }

    /// \brief Post-increments by one.
    ///
    /// \return A copy of the iterator before it is incremented.
    circular_buffer_iterator operator++(int)
    {
        const circular_buffer_iterator orig{*this};
        ++(*this);
        return orig;
    }

    // Bidirectional iterator requirements

    /// \brief Pre-decrements by one.
    ///
    /// \return The decremented iterator.
    circular_buffer_iterator& operator--()
    {
        --current_;
        return *this;
    }

    /// \brief Post-decrements by one.
    ///
    /// \return A copy of the iterator before it is decremented.
    circular_buffer_iterator operator--(int)
    {
        const circular_buffer_iterator orig{*this};
        --(*this);
        return orig;
    }

    // Random access iterator requirements

    /// \brief Access an element by index.
    ///
    /// \param n Position relative to current location
    /// \return A reference to the element at relative location.
    reference operator[](const difference_type n) const { return *(*this + n); }

    /// \brief Advances the iterator.
    ///
    /// \param n The number of positions to increment the iterator.
    /// \return The incremented iterator.
    circular_buffer_iterator& operator+=(const difference_type n)
    {
        current_ += n;
        return *this;
    }

    /// \brief Advances the iterator.
    ///
    /// \param lhs The iterator to be incremented.
    /// \param n The number of positions to increment the iterator.
    /// \return A copy of the iterator which is incremented.
    friend circular_buffer_iterator operator+(const circular_buffer_iterator& lhs, const difference_type n)
    {
        circular_buffer_iterator ret{lhs};
        return ret += n;
    }

    /// \brief Decrements the iterator.
    ///
    /// \param n The number of positions to decrement the iterator.
    /// \return The decremented iterator.
    circular_buffer_iterator& operator-=(const difference_type n)
    {
        current_ -= n;
        return *this;
    }

    /// \brief Decrements the iterator.
    ///
    /// \param lhs The iterator to be incremented.
    /// \param n The number of positions to decrement the iterator.
    /// \return A copy of the iterator which is decremented.
    friend circular_buffer_iterator operator-(const circular_buffer_iterator& lhs, const difference_type n)
    {
        circular_buffer_iterator ret{lhs};
        return ret -= n;
    }

    /// \brief Accesses the underlying pointer.
    ///
    /// \return The underlying pointer.
    const Container* base() const { return container_; }

    /// \brief Returns the current position of the iterator with respect to the begin of the container.
    ///
    /// \return The current position of the iterator.
    difference_type current() const { return current_; }

private:
    /// \brief Access `container_` at `n`.
    reference checked_access(const difference_type n) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n >= 0);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(nullptr != container_);
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(n < score::cpp::ssize(*container_));
        return container_->operator[](static_cast<std::size_t>(n));
    }

    /// \brief The index of the current element.
    difference_type current_;

    /// \brief Pointer to the container for which the iterator was created.
    std::conditional_t<IsConstant, const Container*, Container*> container_;
};

/// \brief Returns the distance between two iterators.
template <typename T, bool IsConstant>
constexpr typename circular_buffer_iterator<T>::difference_type
operator-(const circular_buffer_iterator<T, IsConstant>& lhs, const circular_buffer_iterator<T, IsConstant>& rhs)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(lhs.base() == rhs.base());
    return lhs.current() - rhs.current();
}

/// \brief Checks whether two iterators are equal.
template <typename T, bool IsConstant>
constexpr bool operator==(const circular_buffer_iterator<T, IsConstant>& lhs,
                          const circular_buffer_iterator<T, IsConstant>& rhs)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(lhs.base() == rhs.base());
    return lhs.current() == rhs.current();
}

/// \brief Checks whether two iterators are equal.
template <typename T, bool IsConstant>
constexpr bool operator!=(const circular_buffer_iterator<T, IsConstant>& lhs,
                          const circular_buffer_iterator<T, IsConstant>& rhs)
{
    return !(lhs == rhs);
}

/// \brief Compares whether one iterator is smaller than another iterator.
template <typename T, bool IsConstant>
constexpr bool operator<(const circular_buffer_iterator<T, IsConstant>& lhs,
                         const circular_buffer_iterator<T, IsConstant>& rhs)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(lhs.base() == rhs.base());
    return lhs.current() < rhs.current();
}

/// \brief Compares whether one iterator is smaller or equal than another iterator.
template <typename T, bool IsConstant>
constexpr bool operator<=(const circular_buffer_iterator<T, IsConstant>& lhs,
                          const circular_buffer_iterator<T, IsConstant>& rhs)
{
    return !(rhs < lhs);
}

/// \brief Compares whether one iterator is greater than another iterator.
template <typename T, bool IsConstant>
constexpr bool operator>(const circular_buffer_iterator<T, IsConstant>& lhs,
                         const circular_buffer_iterator<T, IsConstant>& rhs)
{
    return rhs < lhs;
}

/// \brief Compares whether one iterator is greater or equal than another iterator.
template <typename T, bool IsConstant>
constexpr bool operator>=(const circular_buffer_iterator<T, IsConstant>& lhs,
                          const circular_buffer_iterator<T, IsConstant>& rhs)
{
    return !(lhs < rhs);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_CONTAINER_CIRCULAR_BUFFER_ITERATOR_HPP
