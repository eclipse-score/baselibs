/********************************************************************************
 * Copyright (c) 2024 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2024 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Iterator Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_DATA_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_DATA_HPP

#include <cstddef>
#include <initializer_list>

namespace score::cpp
{

/// \brief Returns a pointer to the block of memory containing the elements of the range.
///
/// \param c a container or view with a `data()` member function
///
/// \return Return `c.data()`
template <typename C>
constexpr auto data(C& c) -> decltype(c.data())
{
    return c.data();
}

/// \brief Returns a pointer to the block of memory containing the elements of the range.
///
/// \param c a container or view with a `data()` member function
///
/// \return Return `c.data()`
template <typename C>
constexpr auto data(const C& c) -> decltype(c.data())
{
    return c.data();
}

/// \brief Returns a pointer to the block of memory containing the elements of the range.
///
/// \param array an array of arbitrary type
///
/// \return Return `array`
template <typename T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept
{
    return array; // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
}

/// \brief Returns a pointer to the block of memory containing the elements of the range.
///
/// \param il an `std::initializer_list`
///
/// \return Return `il.begin()`
template <typename E>
constexpr const E* data(const std::initializer_list<E> il) noexcept
{
    return il.begin();
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_DATA_HPP
