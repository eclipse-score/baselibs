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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_HELPER_FUNCTIONS_H
#define SCORE_MW_LOG_DETAIL_COMMON_HELPER_FUNCTIONS_H

#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <typeinfo>

namespace helper
{

template <typename T>
std::size_t Sum(T t)
{
    return static_cast<std::size_t>(t);
}

template <typename T, typename... Rest>
std::size_t Sum(T t, Rest... rest)
{
    std::size_t partial_sum = Sum(rest...);

    // Check for overflow
    if (static_cast<std::size_t>(t) > (std::numeric_limits<std::size_t>::max() - partial_sum))
    {
        // Handle overflow: clamp to max value
        return std::numeric_limits<std::size_t>::max();
    }

    return static_cast<std::size_t>(t) + partial_sum;
}

template <typename Target, typename Source>
inline constexpr Target ClampTo(Source value) noexcept
{
    static_assert(std::is_integral<Target>::value, "ClampTo requires an integral target type.");

    // LCOV_EXCL_START : tooling issue. all branches covered
    // If the Source is bigger than what Target can represent, clamp to max of Target.
    if (static_cast<std::uint64_t>(value) > static_cast<std::uint64_t>(std::numeric_limits<Target>::max()))
    {
        // line is covered by test ClampTest::ClampSourceOverflow
        return std::numeric_limits<Target>::max();
    }
    // LCOV_EXCL_STOP

    return static_cast<Target>(value);
}

inline std::uint16_t ClampAddNullTerminator(std::uint16_t length) noexcept
{
    constexpr auto max_u16 = std::numeric_limits<std::uint16_t>::max();
    // If there's room for a null terminator, add one; otherwise, stick to the max.
    return (length < max_u16) ? static_cast<std::uint16_t>(length + 1U) : length;
}

}  // namespace helper

#endif  // SCORE_MW_LOG_DETAIL_COMMON_HELPER_FUNCTIONS_H
