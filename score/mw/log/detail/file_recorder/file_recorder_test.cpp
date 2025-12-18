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
#include "score/mw/log/detail/file_recorder/file_recorder.h"

#include "score/mw/log/detail/backend_mock.h"

#include <limits>
#include <memory>

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
static_assert(static_cast<std::underlying_type_t<LogLevel>>(kActiveLogLevel) <
                  static_cast<std::underlying_type_t<LogLevel>>(kInActiveLogLevel),
              "Log Level setup for this test makes no sense.");

class FileRecorderFixtureWithLogLevelCheck : public testing::Test
{
  public:
    void SetUp() override
    {
        ON_CALL(*backend_, ReserveSlot()).WillByDefault(Return(slot_));
        ON_CALL(*backend_, GetLogRecord(slot_)).WillByDefault(ReturnRef(log_record_));
        const ContextLogLevelMap context_log_level_map{{LoggingIdentifier{context_id_}, kActiveLogLevel}};
        config_.SetContextLogLevel(context_log_level_map);
        recorder_ = std::make_unique<FileRecorder>(config_, std::move(backend_));
    }

    void TearDown() override {}

  protected:
    std::unique_ptr<BackendMock> backend_ = std::make_unique<BackendMock>();
    const std::string_view context_id_ = "DFLT";
    Configuration config_{};
    std::unique_ptr<FileRecorder> recorder_;
    SlotHandle slot_{};
    LogRecord log_record_{};
};

TEST_F(FileRecorderFixtureWithLogLevelCheck, WillObtainSlotForSufficientLogLevel)
{
    RecordProperty("Requirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "Verifies that recorder shall returns sufficient slots if it started recording successfully.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto slot = recorder_->StartRecord(context_id_, kActiveLogLevel);
    EXPECT_TRUE(slot.has_value());
}

TEST_F(FileRecorderFixtureWithLogLevelCheck, WillObtainEmptySlotForInsufficentLogLevel)
{
    RecordProperty("Requirement", "SCR-1633236, SCR-2592577");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies that recorder can not get slots in case of InActive LogLevel.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto slot = recorder_->StartRecord(context_id_, kInActiveLogLevel);
    EXPECT_FALSE(slot.has_value());
}

TEST_F(FileRecorderFixtureWithLogLevelCheck, DisablesOrEnablesLogAccordingToLevel)
{
    RecordProperty("Requirement", "SCR-1633236, SCR-2592577");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Verifies the ability of enabling or disabling specific log level");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_TRUE(recorder_->IsLogEnabled(kActiveLogLevel, context_id_));
    EXPECT_FALSE(recorder_->IsLogEnabled(kInActiveLogLevel, context_id_));
}

TEST_F(FileRecorderFixtureWithLogLevelCheck, WillObtainEmptySlotsWhenNoSlotsReserved)
{
    RecordProperty("Requirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder shall returns zero slots if no slots were reserved.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto backend_mock = std::make_unique<BackendMock>();
    ON_CALL(*backend_mock, ReserveSlot()).WillByDefault(Invoke([]() -> score::cpp::optional<SlotHandle> {
        return {};
    }));

    const auto recorder = std::make_unique<FileRecorder>(config_, std::move(backend_mock));

    const auto slot = recorder->StartRecord(context_id_, kActiveLogLevel);
    EXPECT_FALSE(slot.has_value());
}

class FileRecorderFixture : public ::testing::Test
{
  public:
    void SetUp() override
    {
        EXPECT_CALL(*backend_, ReserveSlot()).WillOnce(Return(slot_));
        EXPECT_CALL(*backend_, FlushSlot(slot_));
        ON_CALL(*backend_, GetLogRecord(slot_)).WillByDefault(ReturnRef(log_record_));
        recorder_ = std::make_unique<FileRecorder>(config_, std::move(backend_));
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
    const std::string_view context_id_ = "DFLT";
    Configuration config_{};
    std::unique_ptr<FileRecorder> recorder_;
    std::uint8_t expected_number_of_arguments_at_teardown_{1};
};

TEST_F(FileRecorderFixture, TooManyArgumentsWillYieldTruncatedLog)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log will be truncated in case of too many arguments.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    constexpr std::size_t type_info_byte_size_according_to_specification = 4;
    const std::size_t number_of_arguments = log_record_.getLogEntry().payload.capacity() /
                                            (type_info_byte_size_according_to_specification + sizeof(std::uint32_t));
    for (std::size_t i = 0; i < number_of_arguments + 5; ++i)
    {
        recorder_->Log(SlotHandle{}, std::uint32_t{});
    }
    EXPECT_LE(number_of_arguments, std::numeric_limits<decltype(expected_number_of_arguments_at_teardown_)>::max());
    expected_number_of_arguments_at_teardown_ =
        static_cast<decltype(expected_number_of_arguments_at_teardown_)>(number_of_arguments);
}

TEST_F(FileRecorderFixture, TooLargeSinglePayloadWillYieldTruncatedLog)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "The log will be truncated in case of too large single payload.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const std::size_t too_big_data_size = log_record_.getLogEntry().payload.capacity() + 1UL;
    std::vector<char> vec(too_big_data_size);
    std::fill(vec.begin(), vec.end(), 'o');
    recorder_->Log(SlotHandle{}, std::string_view{vec.data(), too_big_data_size});
    recorder_->Log(SlotHandle{}, std::string_view{"xxx"});

    //  Teardown checks if number of arguments is equal to one which means that second argument was ignored due to no
    //  space left in the buffer
}

TEST_F(FileRecorderFixture, LogUint8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log uint8_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint8_t{});
}

TEST_F(FileRecorderFixture, LogBool)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log boolean.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    recorder_->Log(SlotHandle{}, bool{});
}

TEST_F(FileRecorderFixture, LogInt8)
{

    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log int8_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int8_t{});
}

TEST_F(FileRecorderFixture, LogUint16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log uint16_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint16_t{});
}

TEST_F(FileRecorderFixture, LogInt16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log int16_t");
    RecordProperty("TestingTechnique", "Requirements-based test.");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int16_t{});
}

TEST_F(FileRecorderFixture, LogUint32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log uint32_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint32_t{});
}

TEST_F(FileRecorderFixture, LogInt32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log int32_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int32_t{});
}

TEST_F(FileRecorderFixture, LogUint64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log uint64_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::uint64_t{});
}

TEST_F(FileRecorderFixture, LogInt64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log int64_t.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::int64_t{});
}

TEST_F(FileRecorderFixture, LogFloat)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log float.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, float{});
}

TEST_F(FileRecorderFixture, LogDouble)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log double.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, double{});
}

TEST_F(FileRecorderFixture, LogStringView)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log string view.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, std::string_view{"Hello world"});
}

TEST_F(FileRecorderFixture, LogHex8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 8-bit values in hex representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex8{});
}

TEST_F(FileRecorderFixture, LogHex16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 16-bit values in hex representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex16{});
}

TEST_F(FileRecorderFixture, LogHex32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 32-bit values in hex representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex32{});
}

TEST_F(FileRecorderFixture, LogHex64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 64-bit values in hex representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogHex64{});
}

TEST_F(FileRecorderFixture, LogBin8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 8-bit values in bin representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogBin8{});
}

TEST_F(FileRecorderFixture, LogBin16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 16-bit values in bin representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogBin16{});
}

TEST_F(FileRecorderFixture, LogBin32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 32-bit values in bin representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogBin32{});
}

TEST_F(FileRecorderFixture, LogBin64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log 64-bit values in bin representation.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    recorder_->Log(SlotHandle{}, LogBin64{});
}

TEST_F(FileRecorderFixture, LogRawBuffer)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log raw buffer.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    recorder_->Log(SlotHandle{}, LogRawBuffer{"raw", 3});
}

TEST_F(FileRecorderFixture, LogSlog2Message)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "FileRecorder can log LogSlog2Message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");
    recorder_->Log(SlotHandle{}, LogSlog2Message{11, "slog message"});
}

TEST(FileRecorderTests, FileRecorderShouldClearSlotOnStart)
{
    RecordProperty("Requirement", "SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "Recorder should clean slots before reuse.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    // Test setup is here since we cannot reuse the fixture here because we have a specific construction use case.
    Configuration config{};
    config.SetDefaultLogLevel(kActiveLogLevel);
    auto backend = std::make_unique<NiceMock<BackendMock>>();
    ON_CALL(*backend, ReserveSlot()).WillByDefault(Return(SlotHandle{}));
    LogRecord log_record{};
    ON_CALL(*backend, GetLogRecord(testing::_)).WillByDefault(ReturnRef(log_record));
    auto recorder = std::make_unique<FileRecorder>(config, std::move(backend));

    // Simulate the case that a slot already contains data from a previous message.
    constexpr auto context = "ctx0";
    recorder->StartRecord(context, kActiveLogLevel);
    const auto payload = std::string_view{"Hello world"};
    recorder->Log(SlotHandle{}, payload);
    recorder->StopRecord(SlotHandle{});

    // Expect that the previous data is cleared.
    recorder->StartRecord(context, kActiveLogLevel);
    EXPECT_EQ(log_record.getVerbosePayload().GetSpan().size(), 0);
    EXPECT_EQ(log_record.getLogEntry().num_of_args, 0);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
