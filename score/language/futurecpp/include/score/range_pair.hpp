/********************************************************************************
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2017 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.RangePair Component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_RANGE_PAIR_HPP
#define SCORE_LANGUAGE_FUTURECPP_RANGE_PAIR_HPP

#include <iterator>
#include <utility>

namespace score::cpp
{

///
/// \brief Class to hold a pair of iterators which is compatible to a \c std::pair.
///
/// \tparam Iterator Iterator type.
///
/// This class can be used when a pair of iterators must be returned from a function.
///
template <typename Iterator>
struct range_pair : std::pair<Iterator, Iterator>
{
    /// Iterator typedef.
    using iterator = Iterator;

    /// Const iterator typedef.
    using const_iterator = Iterator;

    /// Size type typedef.
    using size_type = std::size_t;

    /// Difference type of \c Iterator.
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;

    /// Value type of \c Iterator.
    using value_type = typename std::iterator_traits<Iterator>::value_type;

    /// Pointer type of \c Iterator.
    using pointer = typename std::iterator_traits<Iterator>::pointer;

    /// Reference type of \c Iterator.
    using reference = typename std::iterator_traits<Iterator>::reference;

    /// Iterator category of \c Iterator.
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

    /// Construct a \c range_pair specified with \a begin and \a end.
    /// \param begin First iterator, i.e. begin of range.
    /// \param end Last iterator, i.e. end of range.
    range_pair(iterator begin, iterator end) : std::pair<Iterator, Iterator>(begin, end) {}

    /// Construct a \c range_pair from \a other where \a other is a \c std::pair.
    /// \param other \c std::pair to convert into a \ref range_pair.
    template <typename OtherIterator>
    range_pair(const std::pair<OtherIterator, OtherIterator>& other) : std::pair<Iterator, Iterator>(other)
    {
    }

    template <typename OtherIterator>
    range_pair& operator=(std::pair<OtherIterator, OtherIterator> other)
    {
        std::swap(other, *this);
        return *this;
    }

    /// \returns Begin of range.
    iterator begin() const { return this->first; }

    /// \return End of range.
    iterator end() const { return this->second; }

    /// \return Size of range.
    size_type size() const { return static_cast<size_type>(std::distance(this->first, this->second)); }
};

///
/// \brief Factory function to create an iterator \ref range_pair from \a begin and \a end.
///
/// \tparam Iterator Iterator type (can be auto deduced).
/// \param begin First iterator, i.e. begin of range.
/// \param end Last iterator, i.e. end of range.
/// \returns Constructed type of \ref range_pair.
///
template <typename Iterator>
constexpr range_pair<Iterator> make_range_pair(Iterator begin, Iterator end)
{
    return range_pair<Iterator>(begin, end);
}

///
/// \brief Factory function to create an iterator \ref range_pair from \a begin and \a size.
///
/// \tparam Iterator Iterator type (can be auto deduced).
/// \param begin First iterator, i.e., begin of range.
/// \param size The size of the range. End of the range is computed as \a begin + \a size.
/// \returns Constructed type of \ref range_pair.
///
template <typename Iterator>
constexpr range_pair<Iterator> make_range_pair(Iterator begin, std::size_t size)
{
    return range_pair<Iterator>(begin, begin + size);
}

///
/// \brief Test if a range pair is empty.
///
/// \tparam Iterator Iterator type (can be auto deduced).
/// \param rp Range to test for emptiness.
/// \returns True if empty, otherwise false.
///
template <typename Iterator>
constexpr bool empty(const range_pair<Iterator>& rp)
{
    return rp.begin() == rp.end();
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_RANGE_PAIR_HPP
