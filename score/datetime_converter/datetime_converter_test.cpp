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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace score
{
namespace platform
{
namespace testing
{

using DateTimeType = score::common::DateTimeType;

class DateTimeConverterTest : public ::testing::Test
{
  public:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(DateTimeConverterTest, date_to_epoch_regular_years)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(2025, 1, 2, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(1735787045, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1978, 3, 6, 15, 40, 50);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(258046850, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_regular_years)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(1735787045);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(2025, dtt1->m_year);
    ASSERT_EQ(1, dtt1->m_month);
    ASSERT_EQ(2, dtt1->m_day);
    ASSERT_EQ(3, dtt1->m_hour);
    ASSERT_EQ(4, dtt1->m_minute);
    ASSERT_EQ(5, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(258046850);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(1978, dtt2->m_year);
    ASSERT_EQ(3, dtt2->m_month);
    ASSERT_EQ(6, dtt2->m_day);
    ASSERT_EQ(15, dtt2->m_hour);
    ASSERT_EQ(40, dtt2->m_minute);
    ASSERT_EQ(50, dtt2->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_leap_years)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(2020, 2, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(1582859045, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(2020, 2, 29, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(1582945445, epoch);

    std::shared_ptr<DateTimeType> dtt3 = std::make_shared<DateTimeType>(2020, 3, 1, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt3, &epoch));
    ASSERT_EQ(1583031845, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_leap_years)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(1582859045);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(2020, dtt1->m_year);
    ASSERT_EQ(2, dtt1->m_month);
    ASSERT_EQ(28, dtt1->m_day);
    ASSERT_EQ(3, dtt1->m_hour);
    ASSERT_EQ(4, dtt1->m_minute);
    ASSERT_EQ(5, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(1582945445);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(2020, dtt2->m_year);
    ASSERT_EQ(2, dtt2->m_month);
    ASSERT_EQ(29, dtt2->m_day);
    ASSERT_EQ(3, dtt2->m_hour);
    ASSERT_EQ(4, dtt2->m_minute);
    ASSERT_EQ(5, dtt2->m_second);

    std::shared_ptr<DateTimeType> dtt3 = score::common::epochToDateTime(1583031845);
    ASSERT_NE(nullptr, dtt3);
    ASSERT_EQ(2020, dtt3->m_year);
    ASSERT_EQ(3, dtt3->m_month);
    ASSERT_EQ(1, dtt3->m_day);
    ASSERT_EQ(3, dtt3->m_hour);
    ASSERT_EQ(4, dtt3->m_minute);
    ASSERT_EQ(5, dtt3->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_regular_years_before_1970)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(1953, 1, 2, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(-536360155, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1833, 3, 6, 15, 40, 50);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(-4317783550, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_regular_years_before_1970)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(-536360155);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(1953, dtt1->m_year);
    ASSERT_EQ(1, dtt1->m_month);
    ASSERT_EQ(2, dtt1->m_day);
    ASSERT_EQ(3, dtt1->m_hour);
    ASSERT_EQ(4, dtt1->m_minute);
    ASSERT_EQ(5, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(-4317783550);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(1833, dtt2->m_year);
    ASSERT_EQ(3, dtt2->m_month);
    ASSERT_EQ(6, dtt2->m_day);
    ASSERT_EQ(15, dtt2->m_hour);
    ASSERT_EQ(40, dtt2->m_minute);
    ASSERT_EQ(50, dtt2->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_1970)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(1969, 12, 31, 23, 59, 59);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(-1, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1970, 1, 1, 0, 0, 0);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(0, epoch);

    std::shared_ptr<DateTimeType> dtt3 = std::make_shared<DateTimeType>(1970, 1, 1, 0, 0, 1);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt3, &epoch));
    ASSERT_EQ(1, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_1970)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(-1);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(1969, dtt1->m_year);
    ASSERT_EQ(12, dtt1->m_month);
    ASSERT_EQ(31, dtt1->m_day);
    ASSERT_EQ(23, dtt1->m_hour);
    ASSERT_EQ(59, dtt1->m_minute);
    ASSERT_EQ(59, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(0);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(1970, dtt2->m_year);
    ASSERT_EQ(1, dtt2->m_month);
    ASSERT_EQ(1, dtt2->m_day);
    ASSERT_EQ(0, dtt2->m_hour);
    ASSERT_EQ(0, dtt2->m_minute);
    ASSERT_EQ(0, dtt2->m_second);

    std::shared_ptr<DateTimeType> dtt3 = score::common::epochToDateTime(1);
    ASSERT_NE(nullptr, dtt3);
    ASSERT_EQ(1970, dtt3->m_year);
    ASSERT_EQ(1, dtt3->m_month);
    ASSERT_EQ(1, dtt3->m_day);
    ASSERT_EQ(0, dtt3->m_hour);
    ASSERT_EQ(0, dtt3->m_minute);
    ASSERT_EQ(1, dtt3->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_leap_years_before_1800_1900)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(1820, 2, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(-4728574555, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1820, 2, 29, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(-4728488155, epoch);

    std::shared_ptr<DateTimeType> dtt3 = std::make_shared<DateTimeType>(1820, 3, 1, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt3, &epoch));
    ASSERT_EQ(-4728401755, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_leap_years_before_1800_1900)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(-4728574555);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(1820, dtt1->m_year);
    ASSERT_EQ(2, dtt1->m_month);
    ASSERT_EQ(28, dtt1->m_day);
    ASSERT_EQ(3, dtt1->m_hour);
    ASSERT_EQ(4, dtt1->m_minute);
    ASSERT_EQ(5, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(-4728488155);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(1820, dtt2->m_year);
    ASSERT_EQ(2, dtt2->m_month);
    ASSERT_EQ(29, dtt2->m_day);
    ASSERT_EQ(3, dtt2->m_hour);
    ASSERT_EQ(4, dtt2->m_minute);
    ASSERT_EQ(5, dtt2->m_second);

    std::shared_ptr<DateTimeType> dtt3 = score::common::epochToDateTime(-4728401755);
    ASSERT_NE(nullptr, dtt3);
    ASSERT_EQ(1820, dtt3->m_year);
    ASSERT_EQ(3, dtt3->m_month);
    ASSERT_EQ(1, dtt3->m_day);
    ASSERT_EQ(3, dtt3->m_hour);
    ASSERT_EQ(4, dtt3->m_minute);
    ASSERT_EQ(5, dtt3->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_leap_years_before_1904_1970)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(1904, 6, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(-2067368155, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1904, 1, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(-2080500955, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_leap_years_before_1904_1970)
{
    std::shared_ptr<DateTimeType> dtt5 = score::common::epochToDateTime(-2067368155);
    ASSERT_NE(nullptr, dtt5);
    ASSERT_EQ(1904, dtt5->m_year);
    ASSERT_EQ(6, dtt5->m_month);
    ASSERT_EQ(28, dtt5->m_day);
    ASSERT_EQ(3, dtt5->m_hour);
    ASSERT_EQ(4, dtt5->m_minute);
    ASSERT_EQ(5, dtt5->m_second);

    std::shared_ptr<DateTimeType> dtt6 = score::common::epochToDateTime(-2080500955);
    ASSERT_NE(nullptr, dtt6);
    ASSERT_EQ(1904, dtt6->m_year);
    ASSERT_EQ(1, dtt6->m_month);
    ASSERT_EQ(28, dtt6->m_day);
    ASSERT_EQ(3, dtt6->m_hour);
    ASSERT_EQ(4, dtt6->m_minute);
    ASSERT_EQ(5, dtt6->m_second);
}

TEST_F(DateTimeConverterTest, date_to_epoch_random_cases)
{
    int64_t epoch{0};
    std::shared_ptr<DateTimeType> dtt1 = std::make_shared<DateTimeType>(1905, 1, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt1, &epoch));
    ASSERT_EQ(-2048878555, epoch);

    std::shared_ptr<DateTimeType> dtt2 = std::make_shared<DateTimeType>(1905, 3, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt2, &epoch));
    ASSERT_EQ(-2043780955, epoch);

    std::shared_ptr<DateTimeType> dtt3 = std::make_shared<DateTimeType>(1906, 2, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt3, &epoch));
    ASSERT_EQ(-2014664155, epoch);

    std::shared_ptr<DateTimeType> dtt4 = std::make_shared<DateTimeType>(1912, 6, 23, 16, 42, 26);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt4, &epoch));
    ASSERT_EQ(-1815290254, epoch);

    std::shared_ptr<DateTimeType> dtt5 = std::make_shared<DateTimeType>(2000, 1, 1, 0, 0, 0);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt5, &epoch));
    ASSERT_EQ(946684800, epoch);

    std::shared_ptr<DateTimeType> dtt6 = std::make_shared<DateTimeType>(1900, 1, 1, 0, 0, 0);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt6, &epoch));
    ASSERT_EQ(-2208988800, epoch);

    std::shared_ptr<DateTimeType> dtt7 = std::make_shared<DateTimeType>(1901, 1, 1, 0, 0, 0);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt7, &epoch));
    ASSERT_EQ(-2177452800, epoch);

    std::shared_ptr<DateTimeType> dtt8 = std::make_shared<DateTimeType>(2020, 1, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt8, &epoch));
    ASSERT_EQ(1580180645, epoch);

    std::shared_ptr<DateTimeType> dtt9 = std::make_shared<DateTimeType>(2020, 3, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::dateTimeToEpoch(dtt9, &epoch));
    ASSERT_EQ(1585364645, epoch);
}

TEST_F(DateTimeConverterTest, epoch_to_date_random_cases)
{
    std::shared_ptr<DateTimeType> dtt1 = score::common::epochToDateTime(-2048878555);
    ASSERT_NE(nullptr, dtt1);
    ASSERT_EQ(1905, dtt1->m_year);
    ASSERT_EQ(1, dtt1->m_month);
    ASSERT_EQ(28, dtt1->m_day);
    ASSERT_EQ(3, dtt1->m_hour);
    ASSERT_EQ(4, dtt1->m_minute);
    ASSERT_EQ(5, dtt1->m_second);

    std::shared_ptr<DateTimeType> dtt2 = score::common::epochToDateTime(-2043780955);
    ASSERT_NE(nullptr, dtt2);
    ASSERT_EQ(1905, dtt2->m_year);
    ASSERT_EQ(3, dtt2->m_month);
    ASSERT_EQ(28, dtt2->m_day);
    ASSERT_EQ(3, dtt2->m_hour);
    ASSERT_EQ(4, dtt2->m_minute);
    ASSERT_EQ(5, dtt2->m_second);

    std::shared_ptr<DateTimeType> dtt3 = score::common::epochToDateTime(-2014664155);
    ASSERT_NE(nullptr, dtt3);
    ASSERT_EQ(1906, dtt3->m_year);
    ASSERT_EQ(2, dtt3->m_month);
    ASSERT_EQ(28, dtt3->m_day);
    ASSERT_EQ(3, dtt3->m_hour);
    ASSERT_EQ(4, dtt3->m_minute);
    ASSERT_EQ(5, dtt3->m_second);

    std::shared_ptr<DateTimeType> dtt4 = score::common::epochToDateTime(-1815290254);
    ASSERT_NE(nullptr, dtt4);
    ASSERT_EQ(1912, dtt4->m_year);
    ASSERT_EQ(6, dtt4->m_month);
    ASSERT_EQ(23, dtt4->m_day);
    ASSERT_EQ(16, dtt4->m_hour);
    ASSERT_EQ(42, dtt4->m_minute);
    ASSERT_EQ(26, dtt4->m_second);

    std::shared_ptr<DateTimeType> dtt5 = score::common::epochToDateTime(946684800);
    ASSERT_NE(nullptr, dtt5);
    ASSERT_EQ(2000, dtt5->m_year);
    ASSERT_EQ(1, dtt5->m_month);
    ASSERT_EQ(1, dtt5->m_day);
    ASSERT_EQ(0, dtt5->m_hour);
    ASSERT_EQ(0, dtt5->m_minute);
    ASSERT_EQ(0, dtt5->m_second);

    std::shared_ptr<DateTimeType> dtt6 = score::common::epochToDateTime(-2208988800);
    ASSERT_NE(nullptr, dtt6);
    ASSERT_EQ(1900, dtt6->m_year);
    ASSERT_EQ(1, dtt6->m_month);
    ASSERT_EQ(1, dtt6->m_day);
    ASSERT_EQ(0, dtt6->m_hour);
    ASSERT_EQ(0, dtt6->m_minute);
    ASSERT_EQ(0, dtt6->m_second);

    std::shared_ptr<DateTimeType> dtt7 = score::common::epochToDateTime(-2177452800);
    ASSERT_NE(nullptr, dtt7);
    ASSERT_EQ(1901, dtt7->m_year);
    ASSERT_EQ(1, dtt7->m_month);
    ASSERT_EQ(1, dtt7->m_day);
    ASSERT_EQ(0, dtt7->m_hour);
    ASSERT_EQ(0, dtt7->m_minute);
    ASSERT_EQ(0, dtt7->m_second);

    std::shared_ptr<DateTimeType> dtt8 = score::common::epochToDateTime(1580180645);
    ASSERT_NE(nullptr, dtt8);
    ASSERT_EQ(2020, dtt8->m_year);
    ASSERT_EQ(1, dtt8->m_month);
    ASSERT_EQ(28, dtt8->m_day);
    ASSERT_EQ(3, dtt8->m_hour);
    ASSERT_EQ(4, dtt8->m_minute);
    ASSERT_EQ(5, dtt8->m_second);

    std::shared_ptr<DateTimeType> dtt9 = score::common::epochToDateTime(1585364645);
    ASSERT_NE(nullptr, dtt9);
    ASSERT_EQ(2020, dtt9->m_year);
    ASSERT_EQ(3, dtt9->m_month);
    ASSERT_EQ(28, dtt9->m_day);
    ASSERT_EQ(3, dtt9->m_hour);
    ASSERT_EQ(4, dtt9->m_minute);
    ASSERT_EQ(5, dtt9->m_second);
}

TEST_F(DateTimeConverterTest, test_if_datetime_format_is_correct)
{
    std::shared_ptr<DateTimeType> dtt = std::make_shared<DateTimeType>(1820, 2, 28, 3, 4, 5);
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_year = 2;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));

    // test leap year
    dtt->m_year = 2020;
    dtt->m_month = 2;
    dtt->m_day = 29;
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_day = 30;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));

    // test regular year
    dtt->m_year = 2021;
    dtt->m_day = 28;
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_day = 29;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));

    // test hours
    dtt->m_day = 20;
    dtt->m_hour = -1;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_hour = 24;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_hour = 22;
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));

    // test minutes
    dtt->m_hour = 20;
    dtt->m_minute = -1;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_minute = 60;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_minute = 22;
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));

    // test seconds
    dtt->m_minute = 20;
    dtt->m_second = -1;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_second = 60;
    ASSERT_FALSE(score::common::isValidDateTimeFormat(dtt));
    dtt->m_second = 22;
    ASSERT_TRUE(score::common::isValidDateTimeFormat(dtt));
}

TEST_F(DateTimeConverterTest, test_yearIsLeap)
{
    ASSERT_FALSE(score::common::yearIsLeap(1999));
    ASSERT_TRUE(score::common::yearIsLeap(2000));
    ASSERT_TRUE(score::common::yearIsLeap(2004));

    ASSERT_FALSE(score::common::yearIsLeap(1900));
    ASSERT_FALSE(score::common::yearIsLeap(2100));
}

}  // namespace testing
}  // namespace platform
}  // namespace score
