/********************************************************************************
 * Copyright (c) 2020 Contributors to the Eclipse Foundation
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
/// \copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP
#define SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP

#include <chrono>
#include <limits>

namespace score::cpp
{
namespace chrono
{

/// @brief Returns the absolute value of the input duration.
template <typename Rep, typename Period, typename = std::enable_if_t<std::numeric_limits<Rep>::is_signed>>
constexpr std::chrono::duration<Rep, Period> abs(const std::chrono::duration<Rep, Period> duration)
{
    return duration >= duration.zero() ? duration : -duration;
}

} // namespace chrono
} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CHRONO_HPP
