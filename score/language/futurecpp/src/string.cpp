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
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace score::cpp
{
namespace pmr
{

namespace
{

template <typename T>
string to_string_integral_impl(const T value, memory_resource* const resource)
{
    static_assert(std::is_integral_v<T>, "Must be an integral type");
    using unsigned_t = std::make_unsigned_t<T>;

    // std::numeric_limits<T>::digits10 yields the number of digits that can be *round-tripped* through T (e.g. 2 for
    // 8-bit int), so we need one extra char for the longest number, and another for the optional sign:
    std::array<string::value_type, std::numeric_limits<unsigned_t>::digits10 + 2> result{};

    const std::to_chars_result r{std::to_chars(result.data(), result.data() + result.size(), value, 10)};
    // only `value_too_large` could happen, but we ensure a sufficiently sized `result` buffer
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD(r.ec == std::errc{});
    return string{result.data(), r.ptr, resource};
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
    return score::cpp::pmr::to_string_integral_impl(value, resource);
}

string to_string(const std::int64_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_integral_impl(value, resource);
}

string to_string(const std::uint32_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_integral_impl(value, resource);
}

string to_string(const std::uint64_t value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_integral_impl(value, resource);
}

string to_string(const double value, memory_resource* const resource)
{
    return score::cpp::pmr::to_string_double_impl(value, resource);
}

} // namespace pmr
} // namespace score::cpp
