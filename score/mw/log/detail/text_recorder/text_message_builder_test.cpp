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
#include "score/mw/log/detail/text_recorder/text_message_builder.h"
#include "score/mw/log/log_level.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

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

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::MatchesRegex;
using ::testing::StrEq;

const std::map<LogLevel, std::string> levels = {
    {LogLevel::kOff, "off"},
    {LogLevel::kFatal, "fatal"},
    {LogLevel::kError, "error"},
    {LogLevel::kWarn, "warn"},
    {LogLevel::kInfo, "info"},
    {LogLevel::kDebug, "debug"},
    {LogLevel::kVerbose, "verbose"},
};

class TextMessageBuilderFixture : public ::testing::TestWithParam<LogLevel>
{
  public:
    void SetUp() override
    {
        auto& log_entry = log_record_.getLogEntry();

        log_entry.app_id = LoggingIdentifier{"TMB"};
        log_entry.ctx_id = LoggingIdentifier{"CTX"};
        //  log_entry.timestamp_steady_nsec = 8791823749;
        //  log_entry.timestamp_system_nsec = 8872983746;
        log_entry.num_of_args = 7;
        log_entry.log_level = LogLevel::kWarn;
        log_entry.payload = ByteVector{'p', 'a', 'y', 'l', 'o', 'a', 'd'};
    }
    void TearDown() override {}

  protected:
    TextMessageBuilder unit_{"XECU"};
    LogRecord log_record_;
};

TEST_F(TextMessageBuilderFixture, ShallDepleteAfterHeaderAndPayload)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextMessageBuilder shall deplete after getting header and payload.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);
    const auto first = unit_.GetNextSpan();
    EXPECT_TRUE(first.has_value());
    const auto second = unit_.GetNextSpan();
    EXPECT_TRUE(second.has_value());
    const auto end = unit_.GetNextSpan();
    EXPECT_FALSE(end.has_value());
}

TEST_F(TextMessageBuilderFixture, HeaderShallHaveSpecificElements)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Header of TextMessageBuilder shall have specific elements like context id, application id, ecu id, "
                   "and number of args.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    const auto header_span = unit_.GetNextSpan().value();
    const auto string_content =
        std::string(reinterpret_cast<const char*>(header_span.data()), static_cast<std::size_t>(header_span.size()));
    EXPECT_THAT(string_content, HasSubstr(" TMB "));
    EXPECT_THAT(string_content, HasSubstr(" CTX "));
    EXPECT_THAT(string_content, HasSubstr(" 000 XECU "));
    EXPECT_THAT(string_content, HasSubstr(" 7 "));  // number of args
    EXPECT_THAT(string_content, HasSubstr(" verbose "));
    EXPECT_THAT(string_content, HasSubstr(" log "));
    EXPECT_THAT(string_content, HasSubstr(" warn "));
}

TEST_F(TextMessageBuilderFixture, PayloadShouldHaveSetText)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Payload of TextMessageBuilder shall have the set text.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    unit_.SetNextMessage(log_record_);

    std::ignore = unit_.GetNextSpan();
    const auto payload_span = unit_.GetNextSpan().value();
    const auto string_content =
        std::string(reinterpret_cast<const char*>(payload_span.data()), static_cast<std::size_t>(payload_span.size()));
    EXPECT_THAT(string_content, StrEq("payload"));
}

INSTANTIATE_TEST_SUITE_P(TestLevels,
                         TextMessageBuilderFixture,
                         ::testing::Values(LogLevel::kOff,
                                           LogLevel::kFatal,
                                           LogLevel::kError,
                                           LogLevel::kWarn,
                                           LogLevel::kInfo,
                                           LogLevel::kDebug,
                                           LogLevel::kVerbose));

TEST_P(TextMessageBuilderFixture, HeaderShallHaveLevelPrintedForAllParams)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Header of TextMessageBuilder shall have printed level for all parameters.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto& log_entry = log_record_.getLogEntry();
    log_entry.log_level = GetParam();
    std::string level_string{levels.at(GetParam())};
    unit_.SetNextMessage(log_record_);

    const auto header_span = unit_.GetNextSpan().value();
    const auto string_content =
        std::string(reinterpret_cast<const char*>(header_span.data()), static_cast<std::size_t>(header_span.size()));
    EXPECT_THAT(string_content, HasSubstr(level_string));
}

TEST_F(TextMessageBuilderFixture, LogLevelToStringShouldReturnUndefinedForInvalidLogLevel)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LogLevelToString should return 'undefined' for an invalid log level.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto& log_entry = log_record_.getLogEntry();

    // Create an invalid log level by casting back to LogLevel
    log_entry.log_level = static_cast<LogLevel>(static_cast<std::underlying_type<LogLevel>::type>(999));

    const std::string kLevelStringUndefined = "undefined";
    unit_.SetNextMessage(log_record_);

    const auto header_span = unit_.GetNextSpan().value();
    const auto string_content =
        std::string(reinterpret_cast<const char*>(header_span.data()), static_cast<std::size_t>(header_span.size()));

    EXPECT_THAT(string_content, HasSubstr(kLevelStringUndefined));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
