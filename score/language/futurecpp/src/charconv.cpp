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
/// @file
/// @copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///

#include <score/charconv.hpp>

#include <score/assert.hpp>

#include <charconv>

namespace score::cpp
{

namespace
{

template <typename T>
to_chars_result to_chars_impl(char* first, char* last, const T value, const int base)
{
    // the implementation started with our own base 16 to chars.
    // now just delegate to `std::to_chars`.
    // keep this restriction. user should switch to `std::to_chars` instead
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_PRD(base == 16);
    const std::to_chars_result r{std::to_chars(first, last, value, base)};
    return {r.ptr, r.ec};
}

} // namespace

to_chars_result to_chars(char* first, char* last, std::int8_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint8_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int16_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint16_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int32_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint32_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::int64_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

to_chars_result to_chars(char* first, char* last, std::uint64_t value, int base)
{
    return score::cpp::to_chars_impl(first, last, value, base);
}

} // namespace score::cpp
