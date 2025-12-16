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
#include "score/mw/log/detail/text_recorder/text_recorder.h"

#include "score/mw/log/detail/backend_mock.h"

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

using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

constexpr LogLevel kActiveLogLevel = LogLevel::kError;
constexpr LogLevel kInActiveLogLevel = LogLevel::kInfo;
constexpr auto kContext = "ctx0";
static_assert(static_cast<std::underlying_type_t<LogLevel>>(kActiveLogLevel) <
                  static_cast<std::underlying_type_t<LogLevel>>(kInActiveLogLevel),
              "Log Level setup for this test makes no sense.");

class TextRecorderFixtureWithLogLevelCheck : public testing::Test
{
  public:
    void SetUp() override
    {
        ON_CALL(*backend_, ReserveSlot()).WillByDefault(Return(slot_));
        ON_CALL(*backend_, GetLogRecord(slot_)).WillByDefault(ReturnRef(log_record_));
        const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{context_id_}, kActiveLogLevel}};
        config_.SetContextLogLevel(context_log_level_map);
        recorder_ = std::make_unique<TextRecorder>(config_, std::move(backend_), true);
    }

    void TearDown() override {}

  protected:
    std::unique_ptr<NiceMock<BackendMock>> backend_ = std::make_unique<NiceMock<BackendMock>>();
    const std::string_view context_id_ = "DFLT";
    Configuration config_{};
    std::unique_ptr<TextRecorder> recorder_;
    SlotHandle slot_{};
    LogRecord log_record_{};
};

TEST_F(TextRecorderFixtureWithLogLevelCheck, WillObtainSlotForSufficientLogLevel)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The required slots will be returned in case of insufficent log level");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto slot = recorder_->StartRecord(context_id_, kActiveLogLevel);
    EXPECT_TRUE(slot.has_value());
}

TEST_F(TextRecorderFixtureWithLogLevelCheck, WillObtainEmptySlotForInsufficentLogLevel)
{
    RecordProperty("Requirement", "SCR-861534, SCR-2592577");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Empty slots will be returned in case of insufficent log level");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto slot = recorder_->StartRecord(context_id_, kInActiveLogLevel);
    EXPECT_FALSE(slot.has_value());
}

TEST_F(TextRecorderFixtureWithLogLevelCheck, DisablesOrEnablesLogAccordingToLevel)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of enabling or disabling specific log level");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_TRUE(recorder_->IsLogEnabled(kActiveLogLevel, context_id_));
    EXPECT_FALSE(recorder_->IsLogEnabled(kInActiveLogLevel, context_id_));
}

TEST_F(TextRecorderFixtureWithLogLevelCheck, WillObtainEmptySlotsWhenNoSlotsReserved)
{
    RecordProperty("Requirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder shall returns zero slots if no slots were reserved.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto backend_mock = std::make_unique<BackendMock>();
    ON_CALL(*backend_mock, ReserveSlot()).WillByDefault(Invoke([]() -> score::cpp::optional<SlotHandle> {
        return {};
    }));

    constexpr auto check_log_level_for_console = true;
    const auto recorder = std::make_unique<TextRecorder>(config_, std::move(backend_mock), check_log_level_for_console);

    const auto slot = recorder->StartRecord(context_id_, kActiveLogLevel);
    EXPECT_FALSE(slot.has_value());
}

class TextRecorderFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        config_.SetDefaultConsoleLogLevel(kActiveLogLevel);
        EXPECT_CALL(*backend_, ReserveSlot()).WillOnce(Return(slot_));
        EXPECT_CALL(*backend_, FlushSlot(slot_));
        ON_CALL(*backend_, GetLogRecord(slot_)).WillByDefault(ReturnRef(log_record_));
        recorder_ = std::make_unique<TextRecorder>(config_, std::move(backend_), true);
        recorder_->StartRecord(context_id_, kActiveLogLevel);
    }

    void TearDown() override
    {
        const auto& log_entry = log_record_.getLogEntry();
        EXPECT_EQ(log_entry.ctx_id.GetStringView(), context_id_);
        EXPECT_EQ(log_entry.log_level, log_level_);
        EXPECT_EQ(log_entry.num_of_args, expected_number_of_arguments_at_teardown_);
        recorder_->StopRecord(slot_);
    }

  protected:
    std::unique_ptr<NiceMock<BackendMock>> backend_ = std::make_unique<NiceMock<BackendMock>>();
    SlotHandle slot_{};
    LogRecord log_record_{};
    LogLevel log_level_ = kActiveLogLevel;
    Configuration config_{};
    std::unique_ptr<TextRecorder> recorder_;
    const std::string_view context_id_ = "DFLT";
    std::uint8_t expected_number_of_arguments_at_teardown_{1};
};

TEST_F(TextRecorderFixture, TooManyArgumentsWillYieldTruncatedLog)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log will be truncated in case of too many arguments");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr std::size_t byte_size_of_space_seperator = 1;
    const std::string_view message{"byte"};

    const std::size_t number_of_arguments =
        log_record_.getLogEntry().payload.capacity() / (message.size() + byte_size_of_space_seperator);
    for (std::size_t i = 0; i < number_of_arguments + 5; ++i)
    {
        recorder_->Log(SlotHandle{}, message);
    }
    EXPECT_LE(number_of_arguments, std::numeric_limits<decltype(expected_number_of_arguments_at_teardown_)>::max());
    expected_number_of_arguments_at_teardown_ =
        static_cast<decltype(expected_number_of_arguments_at_teardown_)>(number_of_arguments);
}

TEST_F(TextRecorderFixture, TooLargeSinglePayloadWillYieldTruncatedLog)
{
    RecordProperty("Requirement", "SCR-861534");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The logs will be truncated in case of too large single payload");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::size_t too_big_data_size = log_record_.getLogEntry().payload.capacity() + 1UL;
    std::vector<char> vec(too_big_data_size);
    std::fill(vec.begin(), vec.end(), 'o');
    recorder_->Log(SlotHandle{}, std::string_view{vec.data(), too_big_data_size});
    recorder_->Log(SlotHandle{}, std::string_view{"xxx"});

    //  Teardown checks if number of arguments is equal to one which means that second argument was ignored due to no
    //  space left in the buffer
}

TEST_F(TextRecorderFixture, LogUint8_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log uint8_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint8_t{});
}

TEST_F(TextRecorderFixture, LogBool)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log boolean.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, bool{});
}

TEST_F(TextRecorderFixture, LogInt8_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log int8_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int8_t{});
}

TEST_F(TextRecorderFixture, LogUint16_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log uint16_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint16_t{});
}

TEST_F(TextRecorderFixture, LogInt16_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log int16_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int16_t{});
}

TEST_F(TextRecorderFixture, LogUint32_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log uint32_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint32_t{});
}

TEST_F(TextRecorderFixture, LogInt32_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log int32_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int32_t{});
}

TEST_F(TextRecorderFixture, LogUint64_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log uint64_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint64_t{});
}

TEST_F(TextRecorderFixture, LogInt64_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log int64_t.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int64_t{});
}

TEST_F(TextRecorderFixture, LogFloat)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log float.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, float{});
}

TEST_F(TextRecorderFixture, LogDouble)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log double.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, double{});
}

TEST_F(TextRecorderFixture, LogStringView)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log string view.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::string_view{"Hello world"});
}

TEST_F(TextRecorderFixture, LogHex8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 8 bits with hex representation.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex8{});
}

TEST_F(TextRecorderFixture, LogHex16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 16 bits with hex representation.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex16{});
}

TEST_F(TextRecorderFixture, LogHex32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 32 bits with hex representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogHex32{});
}

TEST_F(TextRecorderFixture, LogHex64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 64 bits with hex representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogHex64{});
}

TEST_F(TextRecorderFixture, LogBin8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 8 bits with bin representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogBin8{});
}

TEST_F(TextRecorderFixture, LogBin16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 16 bits with bin representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogBin16{});
}

TEST_F(TextRecorderFixture, LogBin32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 32 bits with bin representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogBin32{});
}

TEST_F(TextRecorderFixture, LogBin64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log 64 bits with bin representation.");
    RecordProperty("TestingTechnique", "requirements-based");

    recorder_->Log(SlotHandle{}, LogBin64{});
}

TEST_F(TextRecorderFixture, LogRawBuffer)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log raw buffer.");
    RecordProperty("TestingTechnique", "requirements-based");
    recorder_->Log(SlotHandle{}, LogRawBuffer{"raw", 3});
}

TEST_F(TextRecorderFixture, LogSlog2Message)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "TextRecorder can log LogSlog2Message.");
    RecordProperty("TestingTechnique", "requirements-based");
    recorder_->Log(SlotHandle{}, LogSlog2Message{11, "slog message"});
}

TEST(TextRecorderTests, DefaultLogLevelShallBeUsedIfCheckForConsoleIsDisabled)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that the default log level will be used in case of disabling the console logging.");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    constexpr auto default_log_level = LogLevel::kDebug;
    constexpr auto more_than_default_log_level = LogLevel::kVerbose;
    constexpr auto console_log_level = LogLevel::kOff;
    static_assert(more_than_default_log_level > default_log_level,
                  "Test only makes sense if more_than_default_log_level is higher than default_log_level.");
    static_assert(default_log_level > console_log_level,
                  "Test only makes sense if default log level is higher than console log level.");

    // Test setup is here since we cannot reuse the fixture here because we have a specific construction use case.
    Configuration config{};
    config.SetDefaultLogLevel(default_log_level);
    config.SetDefaultConsoleLogLevel(console_log_level);
    auto backend = std::make_unique<NiceMock<BackendMock>>();
    ON_CALL(*backend, ReserveSlot()).WillByDefault(Return(SlotHandle{}));
    LogRecord log_record{};
    ON_CALL(*backend, GetLogRecord(testing::_)).WillByDefault(ReturnRef(log_record));

    // When the check for console is disabled...
    constexpr auto check_log_level_for_console = false;
    auto recorder = std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);
    // expect that the default log level is checked;
    EXPECT_TRUE(recorder->StartRecord(kContext, default_log_level).has_value());
    EXPECT_FALSE(recorder->StartRecord(kContext, more_than_default_log_level).has_value());
}

TEST(TextRecorderTests, TextRecorderShouldClearSlotOnStart)
{
    RecordProperty("Requirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder should clean slots before reuse.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Test setup is here since we cannot reuse the fixture here because we have a specific construction use case.
    Configuration config{};
    config.SetDefaultLogLevel(kActiveLogLevel);
    auto backend = std::make_unique<NiceMock<BackendMock>>();
    ON_CALL(*backend, ReserveSlot()).WillByDefault(Return(SlotHandle{}));
    LogRecord log_record{};
    ON_CALL(*backend, GetLogRecord(testing::_)).WillByDefault(ReturnRef(log_record));
    constexpr auto check_log_level_for_console = false;
    auto recorder = std::make_unique<TextRecorder>(config, std::move(backend), check_log_level_for_console);

    // Simulate the case that a slot already contains data from a previous message.
    recorder->StartRecord(kContext, kActiveLogLevel);
    const auto payload = std::string_view{"Hello world"};
    recorder->Log(SlotHandle{}, payload);
    recorder->StopRecord(SlotHandle{});

    // Expect that the previous data is cleared.
    recorder->StartRecord(kContext, kActiveLogLevel);
    EXPECT_EQ(log_record.getVerbosePayload().GetSpan().size(), 0);
    EXPECT_EQ(log_record.getLogEntry().num_of_args, 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
