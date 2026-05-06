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
#include "score/mw/log/logging.h"

#include "score/mw/log/recorder_mock.h"
#include "score/mw/log/runtime.h"

#include <string_view>

#include "gtest/gtest.h"

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

const SlotHandle kHandle{42};
const auto kContext = std::string_view{"MYCT"};

TEST(Logging, CanSetAndRetrieveDefaultRecorder)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of retrieving the default logger.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::SetLogRecorder");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given a Recorder
    RecorderMock recorder_mock{};

    // When setting it as global default recorder
    score::mw::log::SetLogRecorder(&recorder_mock);

    // Then retrieving it will return the correct one
    EXPECT_EQ(&recorder_mock, &GetDefaultLogRecorder());
}

class LoggingFixture : public ::testing::Test
{
  public:
    LoggingFixture()
    {
        score::mw::log::SetLogRecorder(&recorder_mock);
        EXPECT_CALL(recorder_mock, StopRecord(kHandle)).Times(1);
    }

    RecorderMock recorder_mock{};
};

TEST_F(LoggingFixture, CanLogVerboseWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging verbose message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogVerbose");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Given nothing
    // Expecting a log record of level verbose
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kVerbose)).WillOnce(Return(kHandle));

    // When logging at level verbose
    score::mw::log::LogVerbose() << 42;
}

TEST_F(LoggingFixture, CanLogDebugWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging debug message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogDebug");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level debug
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kDebug)).WillOnce(Return(kHandle));

    // When logging at level debug
    score::mw::log::LogDebug() << 42;
}

TEST_F(LoggingFixture, CanLogInfoWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging info message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogInfo");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level info
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kInfo)).WillOnce(Return(kHandle));

    // When logging at level info
    score::mw::log::LogInfo() << 42;
}

TEST_F(LoggingFixture, CanLogWarnWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging warning message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogWarn");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level warn
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kWarn)).WillOnce(Return(kHandle));

    // When logging at level warn
    score::mw::log::LogWarn() << 42;
}

TEST_F(LoggingFixture, CanLogErrorWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging error message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogError");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level error
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kError)).WillOnce(Return(kHandle));

    // When logging at level error
    score::mw::log::LogError() << 42;
}

TEST_F(LoggingFixture, CanLogFatalWithoutContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging fatal message without context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogFatal");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level fatal
    EXPECT_CALL(recorder_mock, StartRecord(_, LogLevel::kFatal)).WillOnce(Return(kHandle));

    // When logging at level fatal
    score::mw::log::LogFatal() << 42;
}

TEST_F(LoggingFixture, CanLogVerboseWitContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging verbose message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogVerbose");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level verbose
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kVerbose)).WillOnce(Return(kHandle));

    // When logging at level verbose
    score::mw::log::LogVerbose(kContext) << 42;
}

TEST_F(LoggingFixture, CanLogDebugWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging debug message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogDebug");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level debug
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kDebug)).WillOnce(Return(kHandle));

    // When logging at level debug
    score::mw::log::LogDebug(kContext) << 42;
}

TEST_F(LoggingFixture, CanLogInfoWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging info message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogInfo");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level info
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kInfo)).WillOnce(Return(kHandle));

    // When logging at level info
    score::mw::log::LogInfo(kContext) << 42;
}

TEST_F(LoggingFixture, CanLogWarnWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging warning message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogWarn");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level warn
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kWarn)).WillOnce(Return(kHandle));

    // When logging at level warn
    score::mw::log::LogWarn(kContext) << 42;
}

TEST_F(LoggingFixture, CanLogErrorWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging error message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogError");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level error
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kError)).WillOnce(Return(kHandle));

    // When logging at level error
    score::mw::log::LogError(kContext) << 42;
}

TEST_F(LoggingFixture, CanLogFatalWithContext)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verify the ability of logging fatal message with context provided.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("Verifies", "::score::mw::log::LogFatal");
    RecordProperty("DerivationTechnique", "equivalence-classes"); // equivalence classes

    // Given nothing
    // Expecting a log record of level fatal
    EXPECT_CALL(recorder_mock, StartRecord(kContext, LogLevel::kFatal)).WillOnce(Return(kHandle));

    // When logging at level fatal
    score::mw::log::LogFatal(kContext) << 42;
}

}  // namespace
}  // namespace log
}  // namespace mw
}  // namespace score
