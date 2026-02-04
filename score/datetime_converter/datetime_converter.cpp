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
#include "score/datetime_converter/datetime_converter.h"

namespace score
{
namespace common
{

int32_t leapYearsSince1970(const int16_t year)
{
    int32_t numOfLeapYears = (((year - 1969) / 4) - ((year - 1901) / 100)) + ((year - 1601) / 400);
    if (year < 1970)
    {
        if (year % 4 == 2 || year % 4 == 3)
        {
            numOfLeapYears--;
        }
    }

    return numOfLeapYears;
};

bool yearIsLeap(const int16_t year)
{
    if (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))
    {
        return true;
    }
    else
    {
        return false;
    }
};

bool isValidDateTimeFormat(const std::shared_ptr<DateTimeType> dateTime)
{
    if (dateTime->m_year < 1800 || dateTime->m_year > 9999)
    {
        return false;
    }
    else if (dateTime->m_month < 1 || dateTime->m_month > 12)
    {
        return false;
    }
    else if (dateTime->m_day < 0 || dateTime->m_day > 31)
    {
        return false;
    }
    else if (dateTime->m_hour < 0 || dateTime->m_hour > 23)
    {
        return false;
    }
    else if (dateTime->m_minute < 0 || dateTime->m_minute > 59)
    {
        return false;
    }
    else if (dateTime->m_second < 0 || dateTime->m_second > 59)
    {
        return false;
    }

    if (dateTime->m_month == 2)
    {
        if (yearIsLeap(dateTime->m_year))
        {
            return (dateTime->m_day <= 29);
        }
        else
        {
            return (dateTime->m_day <= 28);
        }
    }

    if (dateTime->m_month == 4 || dateTime->m_month == 6 || dateTime->m_month == 9 || dateTime->m_month == 11)
    {
        return (dateTime->m_day <= 30);
    }

    return true;
};

bool dateTimeToEpoch(const std::shared_ptr<DateTimeType> dateTime, time_t* epoch)
{
    if (!isValidDateTimeFormat(dateTime))
    {
        return false;
    }

    *epoch = 0;
    *epoch = dateTime->m_year - MEDIAN_YEAR;
    *epoch = *epoch * DAYS_PER_YEAR;

    *epoch = *epoch + leapYearsSince1970(dateTime->m_year);
    *epoch = *epoch + DAYS_UNTIL_MONTHS[(dateTime->m_month - 1) % 12];
    *epoch = *epoch + dateTime->m_day - 1;

    if (yearIsLeap(dateTime->m_year) && (dateTime->m_month > 2))
    {
        *epoch = *epoch + 1;
    }

    *epoch = *epoch * HOURS_PER_DAY * MINUTES_PER_HOUR;
    *epoch = *epoch + dateTime->m_hour * MINUTES_PER_HOUR;
    *epoch = *epoch + dateTime->m_minute;
    *epoch = *epoch * SECONDS_PER_MINUTE;
    *epoch = *epoch + dateTime->m_second;

    if (dateTime->m_year > 1900 && dateTime->m_year < 1969 && yearIsLeap(dateTime->m_year))
    {
        *epoch = *epoch - SECONDS_PER_DAY;
    }

    return true;
};

std::shared_ptr<DateTimeType> epochToDateTime(time_t epoch)
{
    bool before1970{false};
    if (epoch < 0)
    {
        before1970 = true;
    }

    std::shared_ptr<DateTimeType> dateTime = std::make_shared<DateTimeType>(0, 1, 1, 0, 0, 0);

    time_t yearsSum = epoch / SECONDS_PER_YEAR;
    dateTime->m_year = static_cast<std::int16_t>(yearsSum + MEDIAN_YEAR);
    if (before1970)
    {
        dateTime->m_year = dateTime->m_year - 1;
    }
    epoch =
        epoch - (yearsSum * DAYS_PER_YEAR * SECONDS_PER_DAY) - (leapYearsSince1970(dateTime->m_year) * SECONDS_PER_DAY);

    if (dateTime->m_year > 1900 && dateTime->m_year < 1969 && yearIsLeap(dateTime->m_year))
    {
        epoch = epoch + SECONDS_PER_DAY;
    }

    time_t daysSum = (epoch / SECONDS_PER_DAY) + 1;
    if (before1970)
    {
        if (yearIsLeap(dateTime->m_year) && ((daysSum * -1) < DAYS_UNTIL_MONTHS[1]))
        {
            --daysSum;
        }
        daysSum = DAYS_PER_YEAR + daysSum - 1;
        if (yearIsLeap(dateTime->m_year))
        {
            daysSum = daysSum - 1;
        }
    }
    else
    {
        if (yearIsLeap(dateTime->m_year))
        {
            --daysSum;
        }
    }

    if (before1970)
    {
        if (yearIsLeap(dateTime->m_year - 1) && ((daysSum * -1) < DAYS_UNTIL_MONTHS[1]))
        {
            if ((daysSum * -1) >= DAYS_PER_LEAP_YEAR)
            {
                dateTime->m_year = dateTime->m_year + 1;
                daysSum = daysSum + DAYS_PER_LEAP_YEAR;
            }
        }
        else
        {
            if ((daysSum * -1) >= DAYS_PER_YEAR)
            {
                dateTime->m_year = dateTime->m_year + 1;
                daysSum = daysSum + DAYS_PER_YEAR;
            }
        }
    }
    else
    {
        if (yearIsLeap(dateTime->m_year + 1) && (daysSum > DAYS_UNTIL_MONTHS[1]))
        {
            if (daysSum >= DAYS_PER_LEAP_YEAR)
            {
                dateTime->m_year = dateTime->m_year + 1;
                daysSum = daysSum - DAYS_PER_LEAP_YEAR;
            }
        }
        else
        {
            if (daysSum >= DAYS_PER_YEAR)
            {
                dateTime->m_year = dateTime->m_year + 1;
                daysSum = daysSum - DAYS_PER_YEAR;
            }
        }
    }

    for (std::int16_t i = 1; i < 13; i++)
    {
        if (yearIsLeap(dateTime->m_year))
        {
            if (daysSum < DAYS_UNTIL_MONTHS_LEAP_YEAR[i])
            {
                dateTime->m_month = static_cast<std::int8_t>(i);
                dateTime->m_day = static_cast<std::int8_t>(daysSum - DAYS_UNTIL_MONTHS_LEAP_YEAR[i - 1] + 1);
                break;
            }
        }
        else
        {
            if (daysSum <= DAYS_UNTIL_MONTHS[i])
            {
                dateTime->m_month = static_cast<std::int8_t>(i);
                dateTime->m_day = static_cast<std::int8_t>(daysSum - DAYS_UNTIL_MONTHS[i - 1]);
                break;
            }
        }
    }

    dateTime->m_second = static_cast<std::int8_t>(epoch % SECONDS_PER_MINUTE);
    epoch = epoch - dateTime->m_second;
    epoch = epoch / SECONDS_PER_MINUTE;
    dateTime->m_minute = static_cast<std::int8_t>(epoch % MINUTES_PER_HOUR);
    epoch = epoch - dateTime->m_minute;
    epoch = epoch / MINUTES_PER_HOUR;
    dateTime->m_hour = static_cast<std::int8_t>(epoch % HOURS_PER_DAY);
    epoch = epoch - dateTime->m_hour * HOURS_PER_DAY;

    if (before1970)
    {
        dateTime->m_second = static_cast<std::int8_t>(dateTime->m_second + SECONDS_PER_MINUTE);
        dateTime->m_minute = static_cast<std::int8_t>(dateTime->m_minute + MINUTES_PER_HOUR - 1);
        dateTime->m_hour = static_cast<std::int8_t>(dateTime->m_hour + HOURS_PER_DAY - 1);

        if (dateTime->m_second >= SECONDS_PER_MINUTE)
        {
            dateTime->m_second = dateTime->m_second - SECONDS_PER_MINUTE;
            dateTime->m_minute++;
        }
        if (dateTime->m_minute >= MINUTES_PER_HOUR)
        {
            dateTime->m_minute = dateTime->m_minute - MINUTES_PER_HOUR;
            dateTime->m_hour++;
        }
        if (dateTime->m_hour >= HOURS_PER_DAY)
        {
            dateTime->m_hour = dateTime->m_hour - HOURS_PER_DAY;
            dateTime->m_day++;
        }
    }

    if (isValidDateTimeFormat(dateTime))
        return dateTime;
    else
        return nullptr;
};

}  // namespace common
}  // namespace score
