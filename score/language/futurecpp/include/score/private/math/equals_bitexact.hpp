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

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_EQUALS_BITEXACT_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_EQUALS_BITEXACT_HPP

namespace score::cpp
{

/// \brief Checks floats for bit exact equality.
///
/// Sometimes it is necessary to compare on bit exact equality of floating point values. If the compiler treats warnings
/// as errors and the warning "float-equal" is enabled, then such a comparison triggers a hard compile time error.
/// This function allows to test for bit exact floating point values.
///
/// Note: Mostly comparing floating point values with an epsilon or ULP is the way to go. Use the this function only if
/// you know the consequences of bit exact comparison.
///
/// \tparam T Floating point type.
/// \param lhs First float value that is compared.
/// \param rhs Second float value that is compared.
/// \return True, if the two floats compare bit exact, false otherwise.
template <typename T>
constexpr bool equals_bitexact(const T lhs, const T rhs) noexcept
{
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#else
#error "unknown compiler"
#endif
    // Suppress "Floating point expression `lhs` is compared using operator `==`". The goal of this function is exactly
    // to provide a tooling-agnostic way to compare floating point values on exact equality. So similar to suppress
    // compiler warning also allow static code analysis
    // coverity[misra_cpp_2023_dir_0_3_1_violation : SUPPRESS]
    return lhs == rhs;
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_MATH_EQUALS_BITEXACT_HPP
