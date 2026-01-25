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
/// @file
/// @copyright Copyright (c) 2020 Contributors to the Eclipse Foundation
///

#include <score/string.hpp>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace pmr
{

namespace
{

/// @brief Computes the absolute value of the specified value
/// @param value signed or unsigned integer
/// @returns an unsigned integer
///
/// std::abs is "undefined if the result cannot be represented by the return type." abs_to_unsigned avoids this
/// precondition by returning an unsigned type, where the result can always be represented.
template <typename T>
constexpr std::make_unsigned_t<T> abs_to_unsigned(const T value)
{
    if constexpr (std::is_signed_v<T>)
    {
        using unsigned_t = std::make_unsigned_t<T>;
        return (value == std::numeric_limits<T>::lowest()) ? static_cast<unsigned_t>(std::numeric_limits<T>::max()) + 1
                                                           : static_cast<unsigned_t>(std::abs(value));
    }
    else
    {
        return value;
    }
}

template <typename T>
string to_string_impl(const T value, memory_resource* const resource)
{
    static_assert(std::is_integral_v<T>, "Must be an integral type");
    using unsigned_t = std::make_unsigned_t<T>;

    // std::numeric_limits<T>::digits10 yields the number of digits that can be *round-tripped* through T (e.g. 2 for
    // 8-bit int), so we need one extra char for the longest number, and another for the optional sign:
    std::array<string::value_type, std::numeric_limits<unsigned_t>::digits10 + 2> result{};

    // Build the representation starting with last digit
    auto current_place = result.end();
    unsigned_t remaining_value{abs_to_unsigned(value)};

    do
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(current_place != result.begin());
        --current_place;
        const auto remainder = remaining_value % unsigned_t{10};
        *current_place = static_cast<string::value_type>(static_cast<unsigned_t>('0') + remainder);
        remaining_value /= unsigned_t{10};
    } while (remaining_value != 0);

    if (value < 0)
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT_DBG(current_place != result.begin());
        --current_place;
        *current_place = '-';
    }

    return string{current_place, result.end(), resource};
}

string to_string_double_impl(const double value, memory_resource* const resource)
{
    score::cpp::pmr::string buffer{resource};
    const int n{std::snprintf(nullptr, 0U, "%lf", value)};
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(n >= 0);

    buffer.resize(static_cast<score::cpp::pmr::string::size_type>(n) + 1U); // +1 for null-termination
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(std::snprintf(&buffer[0], buffer.size(), "%lf", value) == n);
    buffer.pop_back();
    return buffer;
}

} // namespace

string to_string(const std::int32_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const std::int64_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const std::uint32_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const std::uint64_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_impl(value, resource);
}

string to_string(const double value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_double_impl(value, resource);
}

} // namespace pmr
} // namespace score::cpp
