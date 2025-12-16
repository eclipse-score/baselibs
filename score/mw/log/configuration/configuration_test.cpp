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
#include "score/mw/log/configuration/configuration.h"

#include "gtest/gtest.h"

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

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnTrueIfLogLevelIsBelowThreshold)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log message shall be enabled if the log level is below the threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{kCtx}, LogLevel::kError}};
    config.SetContextLogLevel(context_log_level_map);
    EXPECT_TRUE(config.IsLogLevelEnabled(LogLevel::kFatal, kCtx));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnTrueIfLogLevelIsEqualThreshold)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log message shall be enabled if the log level is equal to the threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{kCtx}, LogLevel::kError}};
    config.SetContextLogLevel(context_log_level_map);
    EXPECT_TRUE(config.IsLogLevelEnabled(LogLevel::kError, kCtx));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnFalseIfLogLevelIsAboveThreshold)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log message shall be disabled if the log level is above to the threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{kCtx}, LogLevel::kError}};
    config.SetContextLogLevel(context_log_level_map);
    EXPECT_FALSE(config.IsLogLevelEnabled(LogLevel::kInfo, kCtx));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnTrueIfLogLevelIsAboveOrEqualDefaultThreshold)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The log message shall be enabled if the log level is equal to the default log level threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    config.SetDefaultLogLevel(LogLevel::kInfo);
    EXPECT_TRUE(config.IsLogLevelEnabled(LogLevel::kInfo, kCtx));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnFalseIfLogLevelIsBelowDefaultThreshold)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The log message shall be disabled if the log level is below the default log level threshold.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    config.SetDefaultLogLevel(LogLevel::kInfo);
    EXPECT_FALSE(config.IsLogLevelEnabled(LogLevel::kVerbose, kCtx));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnTrueIfLogLevelIsAboveOrEqualDefaultThresholdForConsole)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "The log message for the console shall be enabled if the log level is equal to the default log level "
        "threshold for the console.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    config.SetDefaultConsoleLogLevel(LogLevel::kInfo);
    EXPECT_TRUE(config.IsLogLevelEnabled(LogLevel::kInfo, kCtx, true));
}

TEST(ConfigurationTestSuite, IsLogEnabledShallReturnFalseIfLogLevelIsBelowDefaultThresholdForConsole)
{
    RecordProperty("Requirement", "SCR-1633254");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "The log message shall be disabled for the console if the log level is below the default log level "
                   "threshold for the console.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    const std::string_view kCtx{"CTX1"};
    config.SetDefaultConsoleLogLevel(LogLevel::kInfo);
    EXPECT_FALSE(config.IsLogLevelEnabled(LogLevel::kVerbose, kCtx, true));
}

TEST(ConfigurationTestSuite, AppidWithMoreThanFourCharactersShallBeTruncated)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The application identifier shall be limited to four characters.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    config.SetAppId("12345");
    EXPECT_EQ(config.GetAppId(), std::string_view{"1234"});
}

TEST(ConfigurationTestSuite, EcuidWithMoreThanFourCharactersShallBeTruncated)
{
    RecordProperty("Requirement", "SCR-1633316");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The ECU identifier shall be limited to four characters.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    Configuration config{};

    config.SetEcuId("12345");
    EXPECT_EQ(config.GetEcuId(), std::string_view{"1234"});
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
