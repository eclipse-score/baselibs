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
#include "score/mw/log/detail/common/composite_recorder.h"

#include "score/mw/log/recorder_mock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "score/callback.hpp"

#include <cstring>
#include <limits>

using testing::_;
using testing::Return;

using namespace std::literals;

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

const std::string_view kContext{"aCtx"};
const LogLevel kLogLevel{LogLevel::kInfo};

const bool kBool{true};
const std::uint8_t kUint8{std::numeric_limits<std::uint8_t>::max()};
const std::uint16_t kUint16{std::numeric_limits<std::uint16_t>::max()};
const std::uint32_t kUint32{std::numeric_limits<std::uint32_t>::max()};
const std::uint64_t kUint64{std::numeric_limits<std::uint64_t>::max()};
const std::int8_t kInt8{std::numeric_limits<std::int8_t>::max()};
const std::int16_t kInt16{std::numeric_limits<std::int16_t>::max()};
const std::int32_t kInt32{std::numeric_limits<std::int32_t>::max()};
const std::int64_t kInt64{std::numeric_limits<std::int64_t>::max()};
const float kFloat{std::numeric_limits<float>::max()};
const double kDouble{std::numeric_limits<double>::max()};
const std::string_view kStringView{"Hello World"sv};
// subtract one from max uint values to expect hex logging.
const LogHex8 kHex8{kUint8 - 1};
const LogHex16 kHex16{kUint16 - 1};
const LogHex32 kHex32{kUint32 - 1};
const LogHex64 kHex64{kUint64 - 1};

// subtract 2 from max uint values to expect binary logging.
const LogBin8 kBin8{kUint8 - 2};
const LogBin16 kBin16{kUint16 - 2};
const LogBin32 kBin32{kUint32 - 2};
const LogBin64 kBin64{kUint64 - 2};

LogRawBuffer log_raw_buffer{nullptr, 0};
LogSlog2Message log_Slog2Message{1, "Hello World"};

MATCHER_P(LogStringEquals, expected, "matches LogString objects")
{
    return (arg.Size() == expected.Size()) && (std::memcmp(arg.Data(), expected.Data(), arg.Size()) == 0);
}
class CompositeRecorderFixture : public ::testing::Test
{
  public:
    void SetUp() override {}

    void TearDown() override {}

    void AddRecorder(std::unique_ptr<Recorder> recorder) noexcept
    {
        recorders_.emplace_back(std::move(recorder));
    }

    void CreateAllAvailableRecorders(score::cpp::callback<std::unique_ptr<Recorder>(size_t)> create_recorder) noexcept
    {
        for (size_t i = 0; i < SlotHandle::kMaxRecorders; ++i)
        {
            AddRecorder(create_recorder(i));
        }
    }

    CompositeRecorder& CreateCompositeRecorder() noexcept
    {
        composite_recorder_ = std::make_unique<CompositeRecorder>(std::move(recorders_));

        return *composite_recorder_;
    }

  protected:
    std::vector<std::unique_ptr<Recorder>> recorders_{};
    std::unique_ptr<CompositeRecorder> composite_recorder_{};
};

TEST_F(CompositeRecorderFixture, CompositeRecorderShallCropExceedingNumberOfRecorders)
{
    RecordProperty("Requirement", "SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "adding recorders to composite recorder which exceed number of recorders will have no effect and "
                   "will be cropped.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;

    // Add the number of allowed recorders
    CreateAllAvailableRecorders([](std::size_t /*recorder*/) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        EXPECT_CALL(*mock_recorder, StartRecord(kContext, kLogLevel));
        return mock_recorder;
    });

    {
        // Add one recorder exceeding the number of allowed recorders.
        auto mock_recorder = std::make_unique<RecorderMock>();

        // Since this recorder shall be dropped, StartRecord shall not be called.
        EXPECT_CALL(*mock_recorder, StartRecord(kContext, kLogLevel)).Times(0);

        AddRecorder(std::move(mock_recorder));
    }

    auto& composite_recorder = CreateCompositeRecorder();

    composite_recorder.StartRecord(kContext, kLogLevel);
}

TEST_F(CompositeRecorderFixture, StartRecordWithSlotAvailableShallRetainCorrectSlot)
{
    RecordProperty("Requirement", "SCR-861578, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "if a slot is available, starting record in CompositeRecorder shall retain a correct slot.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;

    CreateAllAvailableRecorders([](std::size_t recorder) {
        auto mock_recorder = std::make_unique<RecorderMock>();

        // Set the slot index to be equal to the recorder index.
        EXPECT_CALL(*mock_recorder, StartRecord(kContext, kLogLevel)).WillOnce(Return(recorder));

        EXPECT_CALL(*mock_recorder, StopRecord(SlotHandle{static_cast<SlotIndex>(recorder)}));
        return mock_recorder;
    });

    auto& composite_recorder = CreateCompositeRecorder();

    const auto slot = composite_recorder.StartRecord(kContext, kLogLevel);
    ASSERT_TRUE(slot.has_value());
    composite_recorder.StopRecord(slot.value());

    for (std::size_t recorder = 0; recorder < SlotHandle::kMaxRecorders; ++recorder)
    {
        EXPECT_TRUE(slot->IsRecorderActive(SlotHandle::RecorderIdentifier{recorder}));

        // Slot index shall be equal to the recorder index in this test case.
        EXPECT_EQ(slot->GetSlot(SlotHandle::RecorderIdentifier{recorder}), recorder);
    }
}

TEST_F(CompositeRecorderFixture, StartRecordWithNoSlotAvailableShallDropRecorder)
{
    RecordProperty("Requirement", "SCR-861578, SCR-1016719");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "if no slot is available, starting record CompositeRecorder shall drop recorder.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;

    CreateAllAvailableRecorders([](std::size_t /*recorder*/) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        EXPECT_CALL(*mock_recorder, StartRecord(kContext, kLogLevel)).WillOnce(Return(score::cpp::optional<SlotHandle>{}));
        EXPECT_CALL(*mock_recorder, StopRecord(_)).Times(0);
        return mock_recorder;
    });

    auto& composite_recorder = CreateCompositeRecorder();

    const auto slot = composite_recorder.StartRecord(kContext, kLogLevel);
    ASSERT_TRUE(slot.has_value());
    composite_recorder.StopRecord(slot.value());

    for (std::size_t recorder = 0; recorder < SlotHandle::kMaxRecorders; ++recorder)
    {
        EXPECT_FALSE(slot->IsRecorderActive(SlotHandle::RecorderIdentifier{recorder}));
    }
}

TEST_F(CompositeRecorderFixture, LogInvocationShallBeForwardedToAllAvailableRecorders)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "log invocation shall be forworded to any supported logging type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;

    CreateAllAvailableRecorders([](std::size_t recorder) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        SlotHandle recorder_slot{static_cast<SlotIndex>(recorder)};
        EXPECT_CALL(*mock_recorder, StartRecord(kContext, kLogLevel)).WillOnce(Return(recorder_slot));
        EXPECT_CALL(*mock_recorder, LogBool(recorder_slot, kBool));
        EXPECT_CALL(*mock_recorder, LogUint8(recorder_slot, kUint8));
        EXPECT_CALL(*mock_recorder, LogUint16(recorder_slot, kUint16));
        EXPECT_CALL(*mock_recorder, LogUint32(recorder_slot, kUint32));
        EXPECT_CALL(*mock_recorder, LogUint64(recorder_slot, kUint64));
        EXPECT_CALL(*mock_recorder, LogInt8(recorder_slot, kInt8));
        EXPECT_CALL(*mock_recorder, LogInt16(recorder_slot, kInt16));
        EXPECT_CALL(*mock_recorder, LogInt32(recorder_slot, kInt32));
        EXPECT_CALL(*mock_recorder, LogInt64(recorder_slot, kInt64));
        EXPECT_CALL(*mock_recorder, LogFloat(recorder_slot, kFloat));
        EXPECT_CALL(*mock_recorder, LogDouble(recorder_slot, kDouble));
        EXPECT_CALL(*mock_recorder, LogStringView(recorder_slot, kStringView));
        EXPECT_CALL(*mock_recorder, LogUint8(recorder_slot, kHex8.value));
        EXPECT_CALL(*mock_recorder, LogUint16(recorder_slot, kHex16.value));
        EXPECT_CALL(*mock_recorder, LogUint32(recorder_slot, kHex32.value));
        EXPECT_CALL(*mock_recorder, LogUint64(recorder_slot, kHex64.value));

        EXPECT_CALL(*mock_recorder, LogUint8(recorder_slot, kBin8.value));
        EXPECT_CALL(*mock_recorder, LogUint16(recorder_slot, kBin16.value));
        EXPECT_CALL(*mock_recorder, LogUint32(recorder_slot, kBin32.value));
        EXPECT_CALL(*mock_recorder, LogUint64(recorder_slot, kBin64.value));

        EXPECT_CALL(
            *mock_recorder,
            Log_LogRawBuffer(recorder_slot, log_raw_buffer.data(), static_cast<uint64_t>(log_raw_buffer.size())));
        return mock_recorder;
    });

    auto& composite_recorder = CreateCompositeRecorder();

    const auto slot = composite_recorder.StartRecord(kContext, kLogLevel);
    ASSERT_TRUE(slot.has_value());

    composite_recorder.Log(slot.value(), kBool);
    composite_recorder.Log(slot.value(), kUint8);
    composite_recorder.Log(slot.value(), kUint16);
    composite_recorder.Log(slot.value(), kUint32);
    composite_recorder.Log(slot.value(), kUint64);
    composite_recorder.Log(slot.value(), kInt8);
    composite_recorder.Log(slot.value(), kInt16);
    composite_recorder.Log(slot.value(), kInt32);
    composite_recorder.Log(slot.value(), kInt64);
    composite_recorder.Log(slot.value(), kFloat);
    composite_recorder.Log(slot.value(), kDouble);
    composite_recorder.Log(slot.value(), kStringView);
    composite_recorder.Log(slot.value(), kHex8);
    composite_recorder.Log(slot.value(), kHex16);
    composite_recorder.Log(slot.value(), kHex32);
    composite_recorder.Log(slot.value(), kHex64);

    composite_recorder.Log(slot.value(), kBin8);
    composite_recorder.Log(slot.value(), kBin16);
    composite_recorder.Log(slot.value(), kBin32);
    composite_recorder.Log(slot.value(), kBin64);

    composite_recorder.Log(slot.value(), log_raw_buffer);
}

TEST_F(CompositeRecorderFixture, LogSlog2MessageAvailableRecorders)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "log invocation shall be forworded to any supported logging type.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;
    CreateAllAvailableRecorders([](std::size_t recorder) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        SlotHandle recorder_slot{static_cast<SlotIndex>(recorder)};
        EXPECT_CALL(
            *mock_recorder,
            Log_LogSlog2Message(recorder_slot,
                                static_cast<uint16_t>(log_Slog2Message.GetCode()),
                                LogStringEquals(static_cast<score::mw::log::LogString>(log_Slog2Message.GetMessage()))))
            .Times(0);
        return mock_recorder;
    });
    auto& composite_recorder = CreateCompositeRecorder();

    const auto slot = composite_recorder.StartRecord(kContext, kLogLevel);
    ASSERT_TRUE(slot.has_value());
    composite_recorder.Log(slot.value(), log_Slog2Message);
}

TEST_F(CompositeRecorderFixture, LogShallBeEnabledIfAtLeastOneRecorderIsEnabled)
{
    RecordProperty("Requirement", "SCR-861578");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "if at least one recorder is enabled, the logging shall be enabled.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto enable_first_recorder = [](std::size_t recorder_index) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        // Return enabled only for the first recorder.
        EXPECT_CALL(*mock_recorder, IsLogEnabled(kLogLevel, kContext)).WillRepeatedly(Return(recorder_index == 0));
        return mock_recorder;
    };

    CreateAllAvailableRecorders(enable_first_recorder);

    auto& composite_recorder = CreateCompositeRecorder();
    EXPECT_TRUE(composite_recorder.IsLogEnabled(kLogLevel, kContext));
}

TEST_F(CompositeRecorderFixture, LogShallBeDisabledIfAllRecorderAreDisabled)
{
    RecordProperty("Requirement", "SCR-861578, SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "if all recorders is disabled, the logging shall be disabled.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    std::vector<std::unique_ptr<Recorder>> recorders;

    CreateAllAvailableRecorders([](std::size_t /*recorder*/) {
        auto mock_recorder = std::make_unique<RecorderMock>();
        EXPECT_CALL(*mock_recorder, IsLogEnabled(kLogLevel, kContext)).WillRepeatedly(Return(false));
        return mock_recorder;
    });

    auto& composite_recorder = CreateCompositeRecorder();
    EXPECT_FALSE(composite_recorder.IsLogEnabled(kLogLevel, kContext));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
