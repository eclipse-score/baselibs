/********************************************************************************
 * Copyright (c) 2015 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2015 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_ITERATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_ITERATOR_HPP

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
/// - A member function "T* data();" which returns a pointer to the first element
/// - A member function "const T* data() const;" which returns a pointer to the first element
/// - A member function "size_type size() const;" which returns the number of elements
///
/// \tparam Container Container for which the iterator is constructed.
/// \tparam is_constant True defines a const iterator, non-const iterator otherwise.
template <typename Container, bool IsConstant = false>
class iterator
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

    /// \brief Constructs an iterator which is not associated with a sequence.
    ///
    /// The constructed iterator has singular value that is not associated with any sequence. Most expressions are
    /// undefined except destruction and assignment.
    iterator() : current_{} {}

    /// \brief Constructs a new iterator.
    ///
    /// \param container Reference to the container for which the iterator is constructed.
    /// \param is_begin True iterator pointing to the beginning - otherwise iterator pointing one past the last element.
    /// \{
    iterator(Container& container, const bool is_begin)
        : current_{is_begin ? container.data() : (container.data() + container.size())}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(container.size() >= 0);
    }
    iterator(const Container& container, const bool is_begin)
        : current_{is_begin ? container.data() : (container.data() + container.size())}
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(container.size() >= 0);
    }
    /// \}

    /// @brief Allow iterator to const_iterator conversion.
    ///
    /// @param other Non-const iterator to copy-construct a new const iterator of.
    friend class iterator<Container, true>;
    template <bool LocalIsConstant = IsConstant, std::enable_if_t<LocalIsConstant, bool> = true>
    // NOLINTNEXTLINE(google-explicit-constructor) intentionally allow implicit conversion from non-const iterator
    iterator(const iterator<Container, false>& other) : current_{other.current_}
    {
    }

    // Forward iterator requirements

    /// \brief Access the pointed-to-element.
    ///
    /// \pre current_ != nullptr
    ///
    /// \return Reference to the current element.
    reference operator*() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(nullptr != current_);
        return *current_;
    }

    /// \brief Access the pointed-to-element.
    ///
    /// \pre current_ != nullptr
    ///
    /// \return Pointer to the current element.
    pointer operator->() const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(nullptr != current_);
        return current_;
    }

    /// \brief Pre-increments by one.
    ///
    /// \return The incremented iterator.
    iterator& operator++()
    {
        ++current_;
        return *this;
    }

    /// \brief Post-increments by one.
    ///
    /// \return A copy of the iterator before it is incremented.
    iterator operator++(int)
    {
        const iterator orig{*this};
        ++(*this);
        return orig;
    }

    // Bidirectional iterator requirements

    /// \brief Pre-decrements by one.
    ///
    /// \return The decremented iterator.
    iterator& operator--()
    {
        --current_;
        return *this;
    }

    /// \brief Post-decrements by one.
    ///
    /// \return A copy of the iterator before it is decremented.
    iterator operator--(int)
    {
        const iterator orig{*this};
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
    iterator& operator+=(const difference_type n)
    {
        current_ += n;
        return *this;
    }

    /// \brief Advances the iterator.
    ///
    /// \param lhs The iterator to be incremented.
    /// \param n The number of positions to increment the iterator.
    /// \return A copy of the iterator which is incremented.
    friend iterator operator+(const iterator& lhs, const difference_type n)
    {
        iterator ret{lhs};
        return ret += n;
    }

    /// \brief Decrements the iterator.
    ///
    /// \param n The number of positions to decrement the iterator.
    /// \return The decremented iterator.
    iterator& operator-=(const difference_type n)
    {
        current_ -= n;
        return *this;
    }

    /// \brief Decrements the iterator.
    ///
    /// \param lhs The iterator to be incremented.
    /// \param n The number of positions to decrement the iterator.
    /// \return A copy of the iterator which is decremented.
    friend iterator operator-(const iterator& lhs, const difference_type n)
    {
        iterator ret{lhs};
        return ret -= n;
    }

    /// \brief Accesses the underlying pointer.
    ///
    /// \return The underlying pointer.
    const value_type* current() const { return current_; }

private:
    /// \brief Pointer to the current element
    pointer current_;
};

/// \brief Returns the distance between two iterators.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
typename iterator<T>::difference_type operator-(const iterator<T, LhsIsConstant>& lhs,
                                                const iterator<T, RhsIsConstant>& rhs)
{
    return lhs.current() - rhs.current();
}

/// \brief Checks whether two iterators are equal.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator==(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return lhs.current() == rhs.current();
}

/// \brief Checks whether two iterators are equal.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator!=(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return !(lhs == rhs);
}

/// \brief Compares whether one iterator is smaller than another iterator.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator<(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return lhs.current() < rhs.current();
}

/// \brief Compares whether one iterator is smaller or equal than another iterator.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator<=(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return !(rhs < lhs);
}

/// \brief Compares whether one iterator is greater than another iterator.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator>(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return rhs < lhs;
}

/// \brief Compares whether one iterator is greater or equal than another iterator.
///
/// \pre Both iterators must be created from the same container.
template <typename T, bool LhsIsConstant, bool RhsIsConstant>
bool operator>=(const iterator<T, LhsIsConstant>& lhs, const iterator<T, RhsIsConstant>& rhs) noexcept
{
    return !(lhs < rhs);
}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_ITERATOR_HPP
