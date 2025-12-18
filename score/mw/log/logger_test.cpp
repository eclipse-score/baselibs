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
#include "score/mw/log/logger.h"

#include "score/mw/log/logging.h"
#include "score/mw/log/recorder_mock.h"

#include <string_view>

#include <gtest/gtest.h>

namespace score
{
namespace mw
{
namespace log
{
namespace
{

using ::testing::_;
using ::testing::Return;

const SlotHandle HANDLE{42};
const auto CONTEXT = std::string_view{"MYCT"};
const auto CONTEXT_DESCRIPTION = std::string_view{"Test context description"};
const auto kDefaultContext = std::string_view{"DFLT"};

class BasicLoggerFixture : public ::testing::Test
{
  public:
    BasicLoggerFixture()
    {
        score::mw::log::SetLogRecorder(&recorder_mock_);
    }

    Logger unit_{CONTEXT};
    RecorderMock recorder_mock_{};
};

class LoggerFixture : public BasicLoggerFixture
{
  public:
    LoggerFixture()
    {
        EXPECT_CALL(recorder_mock_, StopRecord(HANDLE)).Times(1);
    }
};

TEST_F(LoggerFixture, CanLogVerboseWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging verbose message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level verbose
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kVerbose)).WillOnce(Return(HANDLE));

    // When logging at level verbose
    unit_.LogVerbose() << 42;
}

TEST_F(LoggerFixture, CanLogDebugWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging debug message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level debug
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kDebug)).WillOnce(Return(HANDLE));

    // When logging at level debug
    unit_.LogDebug() << 42;
}

TEST_F(LoggerFixture, CanLogInfoWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging info message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level info
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kInfo)).WillOnce(Return(HANDLE));

    // When logging at level info
    unit_.LogInfo() << 42;
}

TEST_F(LoggerFixture, CanLogWarnWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging warning message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level warn
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kWarn)).WillOnce(Return(HANDLE));

    // When logging at level warn
    unit_.LogWarn() << 42;
}

TEST_F(LoggerFixture, CanLogErrorWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging error message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level error
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kError)).WillOnce(Return(HANDLE));

    // When logging at level error
    unit_.LogError() << 42;
}

TEST_F(LoggerFixture, CanLogFatalWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging fatal message.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level fatal
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kFatal)).WillOnce(Return(HANDLE));

    // When logging at level fatal
    unit_.LogFatal() << 42;
}

TEST_F(LoggerFixture, CheckThatWithLevelSetsCorrectLogLevel)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging warn message using WithLevel API.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given nothing
    // Expecting a log record of level warn
    EXPECT_CALL(recorder_mock_, StartRecord(CONTEXT, LogLevel::kWarn)).WillOnce(Return(HANDLE));

    // When logging at level warn
    unit_.WithLevel(score::mw::log::LogLevel::kWarn) << 42;
}

TEST_F(BasicLoggerFixture, CheckThatIsLogEnabledReturnsCorrectValue)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify that IsLogEnabled API is returning the correct log level.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::Logger");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    // Given IsLogEnabled is called with the expected context
    EXPECT_CALL(recorder_mock_, IsLogEnabled(LogLevel::kWarn, CONTEXT)).WillOnce(Return(true));

    // Expect that the correct value is returned.
    EXPECT_TRUE(unit_.IsLogEnabled(LogLevel::kWarn));
}

TEST(CreateLoggerGetContext, CreateLoggerWithNeededContext)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that GetContext shall return the right context.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    Logger unit = CreateLogger(CONTEXT);
    EXPECT_EQ(unit.GetContext(), CONTEXT);
}

TEST(CreateLoggerGetContext, WhenCreateLoggerWIthEmptyContextShallReturnDefaultLogger)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that creating a logger with empty context it shall return the default logger.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    Logger unit = CreateLogger(std::string_view{});
    EXPECT_EQ(unit.GetContext(), kDefaultContext);
}

TEST(CreateLoggerGetContext, CreateLoggerPassingTwoArgs)
{
    RecordProperty("ParentRequirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that GetContext shall return the right context when instantiating the CreateLogger with "
                   "two arguments.");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    Logger unit = CreateLogger(CONTEXT, CONTEXT_DESCRIPTION);
    EXPECT_EQ(unit.GetContext(), CONTEXT);
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
