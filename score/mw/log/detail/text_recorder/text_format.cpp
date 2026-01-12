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
#include "score/mw/log/detail/text_recorder/text_format.h"

#include <array>
#include <iomanip>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

std::size_t GetSpanSizeCasted(const score::cpp::span<Byte> buffer) noexcept
{
    return GetBufferSizeCasted(buffer.size());
}

std::size_t FormattingFunctionReturnCast(const std::int32_t i) noexcept
{
    if (i > 0)
    {
        return GetBufferSizeCasted(i);
    }
    return 0U;
}

std::size_t TextFormat::PutLogStringViewData(const std::string_view data, score::cpp::span<Byte> buffer) noexcept
{
    const std::size_t length = std::min(data.size(), GetSpanSizeCasted(buffer));
    if (length > 0U)  // LCOV_EXCL_BR_LINE: Cannot be covered in unit tests as 'data.size() > 0' and
                      // buffer size is uncontrollable.
    {
        auto destination_it = std::copy_n(data.begin(), length, buffer.begin());
        auto return_length = length;
        if (destination_it != buffer.end())
        {
            *destination_it = ' ';
            return_length += 1UL;
        }
        else
        {
            std::advance(destination_it, -1L);
            // LCOV_EXCL_BR_START : Defensive programming, else branch could not be reached
            // Preconditions to reach else branch - data.size()==0 or buffer.size()==0
            // These preconditions checked during length calculation
            // In case when length==0 - this part of code is not executed
            if (destination_it >= buffer.begin())
            // LCOV_EXCL_BR_STOP
            {
                *destination_it = ' ';
            }
        }
        return return_length;
    }
    return 0LU;  // LCOV_EXCL_LINE: Cannot be covered in unit tests as if (length > 0) is
                 // uncontrollable.
}

std::size_t TextFormat::PutLogRawBufferData(const LogRawBuffer& data, score::cpp::span<Byte> buffer) noexcept
{
    if (buffer.size() == 0UL)
    {
        return 0UL;
    }
    const auto destination_begin_reference = buffer.begin();
    const auto destination_end_reference = buffer.end();
    auto destination_iterator = destination_begin_reference;
    std::ignore = std::for_each(
        data.begin(), data.end(), [&destination_iterator, destination_end_reference = buffer.end()](const auto input) {
            if (destination_iterator != destination_end_reference)
            {
                std::array<char, 3> temporary_formatting_buffer{};
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) safe to use std::snprintf
                std::ignore = std::snprintf(
                    temporary_formatting_buffer.begin(), temporary_formatting_buffer.size(), "%02hhx", input);
                //  Take first two characters from temporary formatting buffer:
                destination_iterator = std::copy_n(temporary_formatting_buffer.begin(), 2, destination_iterator);
            }
        });
    if (destination_iterator != destination_end_reference)
    {
        constexpr std::array<char, 1UL> kSpace({' '});
        destination_iterator = std::copy(kSpace.begin(), kSpace.end(), destination_iterator);
    }
    return static_cast<std::size_t>(std::distance(destination_begin_reference, destination_iterator));
}

std::size_t TextFormat::PutFormattedTimeData(const std::time_t& time_point, score::cpp::span<Byte> buffer) noexcept
{
    std::size_t total = 0U;
    struct tm time_structure_buffer{};
    const struct tm* const time_structure =
        localtime_r(&time_point, &time_structure_buffer);  // LCOV_EXCL_BR_LINE: there are no branches to be covered.

    if (nullptr != time_structure)  // LCOV_EXCL_BR_LINE: "nullptr" condition can't be controlled via test case.
    {
        const std::size_t buffer_space = GetSpanSizeCasted(buffer);

        std::stringstream ss_time_str;
        ss_time_str << std::put_time(&time_structure_buffer, "%Y/%m/%d %H:%M:%S.");
        const std::string& time_str = ss_time_str.str();

        if (buffer_space > time_str.length())
        {
            std::ignore = std::copy(time_str.begin(), time_str.end(), buffer.begin());
            total = time_str.length();
        }
    }

    return total;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
