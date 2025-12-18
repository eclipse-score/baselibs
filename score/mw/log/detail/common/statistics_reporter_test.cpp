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
#include "score/mw/log/detail/common/statistics_reporter.h"

#include "score/mw/log/recorder_mock.h"

#include "gtest/gtest.h"

using testing::_;
using testing::InSequence;
using testing::Return;

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{
namespace
{

const SlotHandle kSlot{};
const std::chrono::seconds kReportInterval{1};
const std::size_t kNumberOfSlots{7};
const std::size_t kSlotSizeBytes{1024};
const std::chrono::steady_clock::time_point kInitialTime{};
const std::chrono::steady_clock::time_point kOverdueTime = kInitialTime + kReportInterval;
const LogLevel kZeroErrorsLogLevel{LogLevel::kInfo};
const LogLevel kNonZeroErrorsLogLevel{LogLevel::kWarn};

struct StatisticsReporterFixture : public ::testing::Test
{
    void ExpectNoReport()
    {
        EXPECT_CALL(recorder_mock_, StartRecord(_, _)).Times(0);
    }

    void ExpectReport(const std::size_t expected_no_slot_available_counter,
                      const std::size_t expected_message_too_long_counter,
                      const LogLevel expected_log_level)
    {
        InSequence in_sequence{};
        EXPECT_CALL(recorder_mock_, StartRecord(_, expected_log_level)).Times(1).WillOnce(Return(kSlot));
        EXPECT_CALL(recorder_mock_, LogUint64(kSlot, kNumberOfSlots));
        EXPECT_CALL(recorder_mock_, LogUint64(kSlot, kSlotSizeBytes));
        EXPECT_CALL(recorder_mock_, LogUint64(kSlot, expected_no_slot_available_counter));
        EXPECT_CALL(recorder_mock_, LogUint64(kSlot, expected_message_too_long_counter));
        EXPECT_CALL(recorder_mock_, StopRecord(kSlot)).Times(1);
    }

    RecorderMock recorder_mock_{};
    StatisticsReporter unit_{recorder_mock_, kReportInterval, kNumberOfSlots, kSlotSizeBytes};
};

TEST_F(StatisticsReporterFixture, UpdateShallReportIfOverDue)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Update function will report statistics if report interval elapsed.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::size_t expected_no_slot_available_counter = 0;
    const std::size_t expected_message_too_long_counter = 0;
    ExpectReport(expected_no_slot_available_counter, expected_message_too_long_counter, kZeroErrorsLogLevel);
    unit_.Update(kOverdueTime);
}

TEST_F(StatisticsReporterFixture, UpdateShallReportWarningIfOverDueAndErrors)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Update function will report warning in case of overdue error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::size_t expected_no_slot_available_counter = 2;
    const std::size_t expected_message_too_long_counter = 1;
    ExpectReport(expected_no_slot_available_counter, expected_message_too_long_counter, kNonZeroErrorsLogLevel);

    unit_.IncrementNoSlotAvailable();
    unit_.IncrementNoSlotAvailable();
    unit_.IncrementMessageTooLong();

    unit_.Update(kOverdueTime);
}

TEST_F(StatisticsReporterFixture, UpdateShallReportWarningMessageIsTooooLong)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Update function will report warning in case of error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(recorder_mock_, StopRecord(kSlot)).Times(0);

    unit_.IncrementMessageTooLong();
    unit_.Update(kOverdueTime);
}

TEST_F(StatisticsReporterFixture, UpdateShallGiveUpIfNotYetTimeToReport)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "Update function will not report if the time has not passed yet. In that case no output is expected");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    InSequence in_sequence{};
    EXPECT_CALL(recorder_mock_, StartRecord(_, _)).Times(0);

    unit_.Update(kInitialTime);
}

TEST_F(StatisticsReporterFixture, UpdateShallGiveUpIfAlreadyReporting)
{
    RecordProperty("ParentRequirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Update function will not report if it is already reporting.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    InSequence in_sequence{};
    EXPECT_CALL(recorder_mock_, StartRecord(_, _)).Times(1).WillOnce([this](auto, auto) {
        EXPECT_CALL(recorder_mock_, StartRecord(_, _)).Times(0);
        unit_.Update(kOverdueTime);
        return kSlot;
    });

    unit_.Update(kOverdueTime);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
