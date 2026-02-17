/********************************************************************************
 * Copyright (c) 2023 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2023 Contributors to the Eclipse Foundation
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_NUMERIC_LERP_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_NUMERIC_LERP_HPP

#include <score/private/math/equals_bitexact.hpp>
#include <type_traits>

namespace score::cpp
{
/// \brief Computes the linear inter-/extrapolation between two floating-point values.
///
/// The returned value is computed via the formula \f$ lerp(a,b,t) = a + t \cdot (b - a) \f$.
/// This consequentially means that for values of \f$t\f$ in the interval [0,1] an interpolation,
/// whereas for values outside that interval an extrapolation is performed.
///
/// \see https://open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0811r3.html
/// \see https://en.cppreference.com/w/cpp/numeric/lerp
///
/// \tparam T   Floating-point type.
/// \param a    First intput value.
/// \param b    Second input value.
/// \param t    Inter-/extrapolation factor.
/// \return     Inter-/extrapolated value.
template <typename T>
constexpr T lerp(T a, T b, T t)
{
    static_assert(std::is_floating_point<T>::value, "Type needs to be floating point.");
    constexpr T zero{static_cast<T>(0)};
    constexpr T one{static_cast<T>(1)};
    if (((a <= zero) && (b >= zero)) || ((a >= zero) && (b <= zero)))
    {
        return (t * b) + ((one - t) * a);
    }

    if (score::cpp::equals_bitexact(t, one))
    {
        return b;
    }

    const T x{a + (t * (b - a))};
    if ((t > one) == (b > a))
    {
        return (b < x) ? x : b;
    }
    return (b > x) ? x : b;
}
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_NUMERIC_LERP_HPP
