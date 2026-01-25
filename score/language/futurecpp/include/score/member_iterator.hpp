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
/// \brief Score.Futurecpp.MemberIterator component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_MEMBER_ITERATOR_HPP
#define SCORE_LANGUAGE_FUTURECPP_MEMBER_ITERATOR_HPP

#include <score/private/type_traits/iterator_category.hpp>

#include <iterator>
#include <type_traits>
#include <score/type_traits.hpp>

namespace score::cpp
{

namespace detail
{

} // namespace detail

///
/// \brief Wrapper of an iterator of structures to access a public member by its pointer
///
/// \tparam Iterator The iterator which is wrapped
/// \tparam MemberType The type of the member which should be bound
/// \tparam MemberPtr Non-type template parameter of the pointer to data member
///
template <typename Iterator, typename MemberType, MemberType std::iterator_traits<Iterator>::value_type::*MemberPtr>
class member_iterator
{
    /// extracted class that the member refers to
    using class_t = typename std::iterator_traits<Iterator>::value_type;

    /// if the iterator is not mutable - then the member type must be const for references and pointers
    using mutable_dependent_value_type =
        std::conditional_t<detail::is_mutable_iterator<Iterator>::value, MemberType, std::add_const_t<MemberType>>;

public:
    /// \c Iterator value_type
    using value_type = MemberType;
    /// \c Iterator reference type (const for immutable)
    using reference = std::add_lvalue_reference_t<mutable_dependent_value_type>;
    /// pointer type of \c Iterator
    using pointer = std::add_pointer_t<mutable_dependent_value_type>;
    /// difference_type of \c Iterator
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    /// \c Iterator category
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

    /// construction from \a it
    member_iterator(const Iterator& it) : wrapped_iterator_{it} {}
    /// default constructor - forward iterator concept
    member_iterator()
    {
        static_assert(std::is_default_constructible<Iterator>::value,
                      "default constructor is only available starting with forward iterators !)");
        wrapped_iterator_ = {};
    }

    /// pre-increment operator - general iterator concept
    member_iterator& operator++()
    {
        ++wrapped_iterator_;
        return *this;
    }
    /// post-increment operator - input & output iterator concept (based on mutability)
    member_iterator operator++(int) { return {wrapped_iterator_++}; }
    /// dereference operator - input & output iterator concept (based on mutability)
    reference operator*() const { return *wrapped_iterator_.*MemberPtr; }
    /// equality-operator - input iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_input_iterator<I>::value>>
    friend bool operator==(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ == rhs.wrapped_iterator_;
    }
    /// inequality-operator - input iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_input_iterator<I>::value>>
    friend bool operator!=(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ != rhs.wrapped_iterator_;
    }
    /// pre-decrement operator - bidirectional iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_bidirectional_iterator<I>::value>>
    member_iterator& operator--()
    {
        --wrapped_iterator_;
        return *this;
    }
    /// post-decrement operator - bidirectional iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_bidirectional_iterator<I>::value>>
    member_iterator operator--(int)
    {
        return {wrapped_iterator_--};
    }

    /// less operator - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator<(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ < rhs.wrapped_iterator_;
    }

    /// less-equal operator - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator<=(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ <= rhs.wrapped_iterator_;
    }

    /// greater operator - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator>(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ > rhs.wrapped_iterator_;
    }

    /// greater-equal operator - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend bool operator>=(const member_iterator& lhs, const member_iterator& rhs)
    {
        return lhs.wrapped_iterator_ >= rhs.wrapped_iterator_;
    }

    /// add operator for offset \a n as second operand - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend member_iterator operator+(const member_iterator& self, difference_type n)
    {
        return {self.wrapped_iterator_ + n};
    }

    /// add operator for offset \a n as first operand - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend member_iterator operator+(difference_type n, const member_iterator& self)
    {
        return {n + self.wrapped_iterator_};
    }

    /// subtraction operator with offset \a n as second operand- random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend member_iterator operator-(const member_iterator& self, difference_type n)
    {
        return {self.wrapped_iterator_ - n};
    }

    /// subtraction operator to calculate distance between \a lhs and \a rhs - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    friend difference_type operator-(const member_iterator& lhs, const member_iterator& rhs)
    {
        return {lhs.wrapped_iterator_ - rhs.wrapped_iterator_};
    }

    /// add and assign operator by offset \a n - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    member_iterator& operator+=(difference_type n)
    {
        wrapped_iterator_ += n;
        return *this;
    }

    /// subtract and assign operator by offset \a n - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    member_iterator& operator-=(difference_type n)
    {
        wrapped_iterator_ -= n;
        return *this;
    }

    /// subscript operator for direct access by offset \a n - random access iterator concept
    template <typename I = Iterator, typename = std::enable_if_t<detail::is_random_access_iterator<I>::value>>
    reference operator[](difference_type n)
    {
        return wrapped_iterator_[n].*MemberPtr;
    }

    /// getter to obtain the original structure
    auto base() -> std::add_lvalue_reference_t<
        std::conditional_t<detail::is_mutable_iterator<Iterator>::value, class_t, std::add_const_t<class_t>>>
    {
        return *wrapped_iterator_;
    }

    /// getter to obtain the original structure
    auto base() const -> std::add_lvalue_reference_t<std::add_const_t<class_t>> { return *wrapped_iterator_; }

private:
    Iterator wrapped_iterator_;
};

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_MEMBER_ITERATOR_HPP
