/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_DATETIME_CONVERTER__DATETIME_CONVERTER_H
#define SCORE_LIB_DATETIME_CONVERTER__DATETIME_CONVERTER_H

#include <cstdint>
#include <memory>

namespace score
{
namespace common
{

static constexpr std::int32_t SECONDS_PER_DAY = 86400;
static constexpr std::int32_t SECONDS_PER_YEAR = 31557600;
static constexpr std::int32_t SECONDS_PER_MINUTE = 60;
static constexpr std::int32_t MINUTES_PER_HOUR = 60;
static constexpr std::int32_t HOURS_PER_DAY = 24;
static constexpr std::int32_t DAYS_PER_YEAR = 365;
static constexpr std::int32_t DAYS_PER_LEAP_YEAR = 366;
static constexpr std::int32_t MEDIAN_YEAR = 1970;

const std::int16_t DAYS_UNTIL_MONTHS[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
const std::int16_t DAYS_UNTIL_MONTHS_LEAP_YEAR[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

struct DateTimeType
{
    DateTimeType(std::int16_t year,
                 std::int8_t month,
                 std::int8_t day,
                 std::int8_t hour,
                 std::int8_t minute,
                 std::int8_t second)
        : m_year{year}, m_month{month}, m_day{day}, m_hour{hour}, m_minute{minute}, m_second{second}
    {
    }
    DateTimeType()
    {
        m_year = 1970;
        m_month = 1;
        m_day = 1;
        m_hour = 0;
        m_minute = 0;
        m_second = 0;
    }

    std::int16_t m_year{0};
    std::int8_t m_month{0};
    std::int8_t m_day{0};
    std::int8_t m_hour{0};
    std::int8_t m_minute{0};
    std::int8_t m_second{0};
};

int16_t leapYearsSince1970(const int16_t year);
bool yearIsLeap(const int16_t year);
bool isValidDateTimeFormat(const std::shared_ptr<DateTimeType> dateTime);

bool dateTimeToEpoch(const std::shared_ptr<DateTimeType> dateTime, time_t* epoch);
std::shared_ptr<DateTimeType> epochToDateTime(time_t epoch);

}  // namespace common
}  // namespace score

#endif  // SCORE_LIB_DATETIME_CONVERTER__DATETIME_CONVERTER_H
