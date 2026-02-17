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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_ITERATOR_CATEGORY_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_ITERATOR_CATEGORY_HPP

#include <iterator>
#include <type_traits>

namespace score::cpp
{
namespace detail
{

/// \brief Checks whether the iterator category is convertible to a given category
///
/// \tparam I iterator type
/// \{
template <typename I>
using is_input_iterator =
    std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::input_iterator_tag>;

template <typename I>
using is_forward_iterator =
    std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::forward_iterator_tag>;

template <typename I>
using is_bidirectional_iterator =
    std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::bidirectional_iterator_tag>;

template <typename I>
using is_random_access_iterator =
    std::is_convertible<typename std::iterator_traits<I>::iterator_category, std::random_access_iterator_tag>;
/// \}

} // namespace detail
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_TYPE_TRAITS_ITERATOR_CATEGORY_HPP
