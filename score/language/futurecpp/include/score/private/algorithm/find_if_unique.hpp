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
/// \brief Score.Futurecpp.Algorithm component
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_FIND_IF_UNIQUE_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_FIND_IF_UNIQUE_HPP

#include <algorithm>
#include <iterator>

namespace score::cpp
{

/// \brief Similar to std::find_if, but additionally checks that the condition is matched only once in the range,
/// otherwise it returns last
///
/// \param first Iterator to the first element in the range [first,last).
/// \param last  Iterator to the last element in the range [first,last).
/// \param fn    Function taking one arguments of the type the iterators point to. It returns a bool.
/// \return      Iterator to the element satisfying the condition–last if no such element is found or multiple elements
/// satisfying the condition.
template <typename ForwardIt, typename UnaryOperation>
ForwardIt find_if_unique(ForwardIt first, const ForwardIt last, UnaryOperation fn)
{
    first = std::find_if(first, last, fn);
    if ((first != last) && (std::find_if(std::next(first), last, fn) != last))
    {
        first = last;
    }
    return first;
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_FIND_IF_UNIQUE_HPP
