/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_TRANSFORM_IF_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_TRANSFORM_IF_HPP

namespace score::cpp
{

/// \brief Conditionally transforms elements in [`first`, `last`) and adds them to `out_begin`
///
/// \note The order is identical to a call to std::copy_if followed by std::transform without the intermediate data
///
/// \tparam ForwardIter Iterator type satisfying the `LegacyForwardIterator` named requirement
/// \tparam OutputIter Iterator type satisfying the `LegacyOutputIterator` named requirement
/// \tparam UnaryFunction Unary functional type
/// \tparam Predicate Predicate functional type
/// \param first begin of input range
/// \param last one behind last of input range
/// \param out_begin begin of output range
/// \param unary_function function that is applied to transform the input type to the output type
/// \param predicate unary predicate which returns `true` for the required elements
/// \return Instance of OutputIter type pointing to the new end of output range (one behind last)
template <typename ForwardIter, typename OutputIter, typename UnaryFunction, typename Predicate>
OutputIter transform_if(
    ForwardIter first, const ForwardIter last, OutputIter out_begin, UnaryFunction unary_function, Predicate predicate)
{
    while (first != last)
    {
        if (predicate(*first))
        {
            *out_begin = unary_function(*first);
            ++out_begin;
        }
        ++first;
    }
    return out_begin;
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_TRANSFORM_IF_HPP
