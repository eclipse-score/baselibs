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
/// \brief Score.Futurecpp.Float Component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_FLOAT_HPP
#define SCORE_LANGUAGE_FUTURECPP_FLOAT_HPP

#include <algorithm>
#include <limits>
#include <type_traits>
#include <score/math.hpp>
#include <score/type_traits.hpp>

namespace score::cpp
{

/// \brief Checks two iterable ranges containing floats for equality, considering the machine precision
///
/// \tparam T The iterator type
/// \param lhs_begin The begin iterator for the first range
/// \param lhs_end The end iterator for the first range
/// \param rhs_begin The begin iterator for the second range
/// \param rhs_end The end iterator for the second range
/// \param max_rel_diff See max_rel_diff of \ref score::cpp::equals(T, T, T)
/// \return True, if the two ranges have the same size and if all elements are almost equals, false otherwise.
template <typename T,
          typename = std::enable_if_t<std::is_floating_point<typename std::iterator_traits<T>::value_type>::value>>
constexpr bool equals(const T lhs_begin,
                      const T lhs_end,
                      const T rhs_begin,
                      const T rhs_end,
                      const typename std::iterator_traits<T>::value_type max_rel_diff =
                          std::numeric_limits<typename std::iterator_traits<T>::value_type>::epsilon())
{
    return std::equal(
        lhs_begin, lhs_end, rhs_begin, rhs_end, [max_rel_diff](const auto lhs_elem, const auto rhs_elem) -> bool {
            return score::cpp::equals(lhs_elem, rhs_elem, max_rel_diff);
        });
}

/// \brief Checks two iterables containing floats for equality, considering the machine precision
///
/// \tparam T The iterable type
/// \param lhs The first iterable that is compared
/// \param rhs The second iterable that is compared
/// \param max_rel_diff See max_rel_diff of \ref score::cpp::equals(T, T, T)
/// \return True, if the two iterables have the same size and if all elements are almost equals, false otherwise.
template <typename T, typename = std::enable_if_t<score::cpp::is_iterable<T>::value>>
constexpr bool
equals(const T& lhs,
       const T& rhs,
       const typename T::value_type max_rel_diff = std::numeric_limits<typename T::value_type>::epsilon())
{
    return score::cpp::equals(std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs), max_rel_diff);
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_FLOAT_HPP
