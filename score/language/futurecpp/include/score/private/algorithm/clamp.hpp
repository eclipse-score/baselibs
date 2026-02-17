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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_CLSCORE_LANGUAGE_FUTURECPP_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_CLSCORE_LANGUAGE_FUTURECPP_HPP

#include <score/assert.hpp>

#include <functional>

namespace score::cpp
{

/// \brief Clamp value to interval [lo, hi].
///
/// \note
///   The condition comp(lo, hi) is checked with an assertion, but in general the result is undefined if !comp(lo, hi)!
///
/// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4536.html
///
/// \tparam T Data type the function is applied to.
/// \tparam Compare Comparison function object.
/// \param value Value to be clamped.
/// \param lo Minimum value of the interval.
/// \param hi Maximum value of the interval.
/// \param comp Comparison function object.
/// \return Value clamped to interval [lo, hi]
template <typename T, typename Compare>
constexpr const T& clamp(const T& value, const T& lo, const T& hi, Compare comp)
{
    SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(!comp(hi, lo));
    return comp(value, lo) ? lo : comp(hi, value) ? hi : value;
}

/// \brief Clamp value to interval [lo, hi].
///
/// \note
///   The condition comp(lo, hi) is checked with an assertion, but in general the result is undefined if !comp(lo, hi)!
///
/// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4536.html
///
/// \tparam T Data type the function is applied to. The data type must meet the requirements of
/// <a href="https://en.cppreference.com/w/cpp/named_req/LessThanComparable">LessThanComparable</a>.
/// \param value Value to be clamped.
/// \param lo Minimum value of the interval.
/// \param hi Maximum value of the interval.
/// \return Value clamped to interval [lo, hi].
template <typename T>
constexpr const T& clamp(const T& value, const T& lo, const T& hi)
{
    return score::cpp::clamp(value, lo, hi, std::less<T>());
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_ALGORITHM_CLSCORE_LANGUAGE_FUTURECPP_HPP
