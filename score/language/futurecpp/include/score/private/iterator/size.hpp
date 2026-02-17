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
/// \brief Score.Futurecpp.Size Component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_SIZE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_SIZE_HPP

#include <score/type_traits.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace score::cpp
{

///
/// \brief Get size of a container.
///
/// \tparam C Container type.
/// \param c a container with a size method
/// \return Size of \a container.
///
template <typename C>
constexpr auto size(const C& c) -> decltype(c.size())
{
    return c.size();
}
///
/// \brief Get size of a container as a signed type.
///
/// Implements: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1227r0.html
///
/// \tparam C Container type.
/// \param c A container with a size method.
/// \return Size of \a container as a signed type.
///
template <typename C, typename std::enable_if_t<has_size_v<C>, int> = 0>
constexpr std::ptrdiff_t ssize(const C& c)
{
    return static_cast<std::ptrdiff_t>(score::cpp::size(c));
}
///
/// \brief Get size of a range pair.
///
/// \tparam C Container type.
/// \return Size of \a container as a signed type.
///
template <typename C, typename std::enable_if_t<!has_size_v<C>, int> = 0>
constexpr std::ptrdiff_t ssize(const C& c)
{
    return std::distance(std::begin(c), std::end(c));
}
///
/// \brief Get size of an array.
///
/// \tparam T Array type.
/// \tparam N Compile time computed size of the array.
/// \return Size of \a array as an unsigned type.
///
template <typename T, std::size_t N>
constexpr std::size_t size(const T (&)[N])
{
    return N;
}
///
/// \brief Get size of an array.
///
/// Implements: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1227r0.html
///
/// \tparam T Array type.
/// \tparam N Compile time computed size of the array.
/// \return Size of \a array as a signed type.
///
template <typename T, std::size_t N>
constexpr std::ptrdiff_t ssize(const T (&)[N])
{
    return static_cast<std::ptrdiff_t>(N);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_SIZE_HPP
