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

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_DIV_ROUND_CLOSEST_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_DIV_ROUND_CLOSEST_HPP

#include <score/assert.hpp>

#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace math
{

/// \brief Integer division rounded to the closest integer
/// Inspired by the Linux Kernel macro \c DIV_ROUND_CLOSEST()
///
/// Formula: \f$round(\frac{num}{den})\f$
/// Rounds up at the midpoint, e.g. \f$round(\frac{3}{2}) = 2\f$
///
/// \tparam T integer type
/// \param num nominator of the division
/// \param den non-zero denominator of the division
/// \return result of the division rounded to the closest integer
template <typename T>
constexpr T div_round_closest(const T num, const T den)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(den != 0);
    if /* constexpr */ (std::is_signed<T>::value)
    {
        const bool quotient_out_of_range = (num == std::numeric_limits<T>::min()) && (den == static_cast<T>(-1));
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_MESSAGE(!quotient_out_of_range, "Quotient out of range: T_min / -1 > T_max");
    }
    static_assert(std::is_integral<T>::value, "div_round_closest makes only sense with integers");

    const T quotient = static_cast<T>(num / den);
    const T rem = static_cast<T>(num % den);

    if (std::is_unsigned<T>::value || ((num >= 0) && (den > 0)))
    {
        // Result is >= 0. Round up if necessary.
        // den > rem
        return (rem < (den - rem)) ? quotient : static_cast<T>(quotient + T{1});
    }

    if ((num < 0) && (den < 0))
    {
        // Result is >= 0. Round up if necessary.
        // den < rem
        return (rem > (den - rem)) ? quotient : static_cast<T>(quotient + T{1});
    }

    if (den < 0)
    {
        // Result is <= 0. Round down if necessary.
        // rem >= 0
        // -den > rem
        return (-rem >= (den + rem)) ? quotient : static_cast<T>(quotient - T{1});
    }

    // Result is <= 0. Round down if necessary.
    // den > 0 && rem < 0
    // den > -rem
    return (-rem <= (den + rem)) ? quotient : static_cast<T>(quotient - T{1});
}

} // namespace math
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_DIV_ROUND_CLOSEST_HPP
