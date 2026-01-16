/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_SAFE_MATH_DETAILS_FLOATING_POINT_ENVIRONMENT_H
#define SCORE_LIB_SAFE_MATH_DETAILS_FLOATING_POINT_ENVIRONMENT_H

#include "score/result/result.h"

#include <type_traits>

namespace score::safe_math::details
{

/// Testing for errors of floating-point operations is difficult to do when not programming for a specific hardware.
/// This class is intended to wrap this in a simple and safe interface. One necessary simplification is that we do not
/// differ between errors since the hardware is not forced to provide support all and may map cases to different errors
/// which would make this library hardware-specific if we differ between errors.
class FloatingPointEnvironment
{
  public:
    FloatingPointEnvironment();
    ~FloatingPointEnvironment();

    FloatingPointEnvironment(const FloatingPointEnvironment&) = delete;
    FloatingPointEnvironment& operator=(const FloatingPointEnvironment&) = delete;
    FloatingPointEnvironment(FloatingPointEnvironment&&) = delete;
    FloatingPointEnvironment& operator=(FloatingPointEnvironment&&) = delete;

    /// Performs the calculation and returns its result if no error occurred, or the error.
    /// \tparam T Return type of the calculation
    /// \tparam F Type of calculation
    /// \param calculation The calculation to perform
    /// \return The result or error
    template <class F, class T = std::invoke_result_t<F()>>
    static score::Result<T> CalculateAndVerify(const F& calculation) noexcept
    {
        FloatingPointEnvironment floating_point_environment{};
        const T result = calculation();

        score::ResultBlank potential_error = floating_point_environment.Test();
        if (!potential_error.has_value())
        {
            return score::MakeUnexpected<T>(potential_error.error());
        }
        return result;
    }

    score::ResultBlank Test() const noexcept;

    void Clear() noexcept;
};

}  // namespace score::safe_math::details

#endif  // SCORE_LIB_SAFE_MATH_DETAILS_FLOATING_POINT_ENVIRONMENT_H
