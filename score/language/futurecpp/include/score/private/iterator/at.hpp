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

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_AT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_AT_HPP

#include <score/private/iterator/size.hpp>
#include <score/private/type_traits/is_maplike_container.hpp>
#include <score/assert.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace score::cpp
{

///
/// \brief Returns a reference to the element at specified location pos, with assertion as bounds checking.
///
/// Implements at() which is part of the Cpp Core Guidelines bounds profile:
/// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Res-ptr
///
/// \pre pos in [ 0, container.size() )
/// \pre container.size() is less than maximum possible value of pos. (i.e. numeric limit)
///
/// \tparam T Container type.
/// \param container Container to query the element.
/// \param pos Position of the element to return.
/// \return Reference to the requested element.
///
template <typename T,
          typename = std::enable_if_t<
              std::is_base_of<std::random_access_iterator_tag,
                              typename std::iterator_traits<typename T::iterator>::iterator_category>::value &&
              !detail::is_maplike_container<T>::value>>
constexpr auto at(T& container, const std::ptrdiff_t pos) -> decltype(*(std::begin(container)))
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(pos >= 0);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(pos < score::cpp::ssize(container));
    return *(std::begin(container) + pos);
}

///
/// \brief Returns a reference to the element at specified location pos, with assertion as bounds checking.
///
/// Implements at() which is part of the Cpp Core Guidelines bounds profile:
/// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Res-ptr
///
/// \pre pos in [0, N)
///
/// \tparam T C-style array of type T.
/// \tparam N Size of the array.
/// \param array C-style array to query the element.
/// \param pos Position of the element to return.
/// \return Reference to the requested element.
///
template <typename T, std::size_t N>
constexpr T& at(T (&array)[N], const std::ptrdiff_t pos)
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(pos >= 0);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(static_cast<std::size_t>(pos) < N);
    return array[pos];
}

///
/// \brief Returns a reference to the element at specified key, with assertion on its existence.
///
/// Implements at() for associative containers which provide their own find() method (e.g. std::unordered_map,
/// std::multimap). Instead of the usual exception, this implementation uses the SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG analogously to
/// the score::cpp::at() implementations for arrays and vectors.
///
/// \pre key in map
///
/// \tparam T Map type.
/// \param map Container to query the element.
/// \param key Key of the element to return.
/// \return Reference to the requested element.
///
template <typename T, typename = std::enable_if_t<detail::is_maplike_container<T>::value>>
constexpr auto at(T& map, const typename T::key_type& key) -> decltype(std::get<1>(*std::begin(map)))
{
    const auto it = map.find(key);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(it != std::end(map));
    return std::get<1>(*it);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ITERATOR_AT_HPP
