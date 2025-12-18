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
#include "gtest/gtest.h"

#include "score/os/mocklib/qnx/mock_slog2.h"
#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/detail/slog/slog_backend.h"

#include "score/assert_support.hpp"

#include <limits>

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
using ::testing::Return;
using ::testing::StrEq;

const std::string kDefaultApp{"a1"};
const std::string kDefaultContext{"c1"};
const std::string kDefaultMessage{"default message"};
const std::uint16_t kDefaultCode{0};

struct SlogBackendFixture : ::testing::Test
{
    void SetUp() override
    {
        slog2_mock_ = score::cpp::pmr::make_unique<score::os::qnx::MockSlog2>(score::cpp::pmr::get_default_resource());
        slog2_mock_raw_ptr_ = slog2_mock_.get();
    }

  protected:
    void SimulateLogging(LogLevel log_level,
                         const std::string& app_id = kDefaultApp,
                         const std::string& ctx_id = kDefaultContext,
                         const std::string& message = kDefaultMessage,
                         const std::uint16_t& code = kDefaultCode)
    {
        SlogBackend backend(config_.GetNumberOfSlots(), log_record_, app_id, std::move(slog2_mock_));

        auto slot = backend.ReserveSlot();
        EXPECT_TRUE(slot.has_value());

        auto& payload = backend.GetLogRecord(slot.value());
        auto& log_entry = payload.getLogEntry();
        log_entry.ctx_id = LoggingIdentifier(std::string_view(ctx_id));
        log_entry.log_level = log_level;
        log_entry.payload = ByteVector(message.begin(), message.end());
        log_entry.slog2_code = code;

        backend.FlushSlot(slot.value());
    }

    LogRecord log_record_{};
    Configuration config_{};
    score::cpp::pmr::unique_ptr<score::os::qnx::MockSlog2> slog2_mock_{};
    score::os::qnx::MockSlog2* slog2_mock_raw_ptr_;
};

TEST_F(SlogBackendFixture, SlogRegister)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies normal slog registering.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _));

    SlogBackend unit(config_.GetNumberOfSlots(), log_record_, config_.GetAppId(), std::move(slog2_mock_));
}

TEST_F(SlogBackendFixture, SlogRegisterWithCapacityBiggerThanTheMaximum)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies slog registering with slots' capacity bigger than the maximum.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    auto capacity = std::numeric_limits<SlotIndex>::max() + 1;
    config_.SetNumberOfSlots(capacity);
    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _));

    SlogBackend unit(config_.GetNumberOfSlots(), log_record_, config_.GetAppId(), std::move(slog2_mock_));
}

TEST_F(SlogBackendFixture, SlogRegisterShouldHandleError)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies slog registering in case of returning an error.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _))
        .Times(1)
        .WillOnce(Return(score::cpp::make_unexpected<score::os::Error>(score::os::Error::createFromErrno())));

    SlogBackend unit(config_.GetNumberOfSlots(), log_record_, config_.GetAppId(), std::move(slog2_mock_));
}

TEST_F(SlogBackendFixture, ReserveSlotShouldAcquireSlot)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of reserving slot.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _));

    SlogBackend unit(config_.GetNumberOfSlots(), log_record_, config_.GetAppId(), std::move(slog2_mock_));

    auto slot = unit.ReserveSlot();
    EXPECT_TRUE(slot.has_value());
}

TEST_F(SlogBackendFixture, LevelOffLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verbosity shall be invaild in case of disabling the logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_INVALID_VERBOSITY, _)).Times(1);

    SimulateLogging(LogLevel::kOff);
}

TEST_F(SlogBackendFixture, FatalLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging fatal message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_CRITICAL, _)).Times(1);

    SimulateLogging(LogLevel::kFatal);
}

TEST_F(SlogBackendFixture, ErrorLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging error message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_ERROR, _)).Times(1);

    SimulateLogging(LogLevel::kError);
}

TEST_F(SlogBackendFixture, WarningLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging waring message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_WARNING, _)).Times(1);

    SimulateLogging(LogLevel::kWarn);
}

TEST_F(SlogBackendFixture, InfoLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging info message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_INFO, _)).Times(1);

    SimulateLogging(LogLevel::kInfo);
}

TEST_F(SlogBackendFixture, DebugLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging debug message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_DEBUG1, _)).Times(1);

    SimulateLogging(LogLevel::kDebug);
}

TEST_F(SlogBackendFixture, VerboseLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of logging verbose message.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_DEBUG2, _)).Times(1);

    SimulateLogging(LogLevel::kVerbose);
}

TEST_F(SlogBackendFixture, DisableTheLog)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of disabling the logging.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));

    SimulateLogging(LogLevel::kOff);
}

TEST_F(SlogBackendFixture, MessageShouldContainAppCtxPayload)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies lgo message with application and context payload.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_DEBUG2, StrEq("MyAp,MyCt: Hello World"))).Times(1);

    SimulateLogging(LogLevel::kVerbose, "MyAp", "MyCt", "Hello World");
}

TEST_F(SlogBackendFixture, BackendShouldHandleEmptyPayload)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies the ability of hte backend of handling empty payload.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_DEBUG2, StrEq(",: "))).Times(1);

    SimulateLogging(LogLevel::kVerbose, "", "", "");
}

TEST_F(SlogBackendFixture, LongIdentifiersShouldBeCropped)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description",
                   "Verifies that the application or context IDs should be cropped if it exceeds 4 characters length.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_DEBUG2, StrEq("1234,4567: "))).Times(1);

    SimulateLogging(LogLevel::kVerbose, "12345", "45678", "");
}

TEST_F(SlogBackendFixture, Slog2CodeShouldBeForwarded)
{
    RecordProperty("Description", "Verify if slog2 code is forwarded to slog2.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, 100, SLOG2_DEBUG2, StrEq("MyAp,MyCt: Slog message"))).Times(1);

    SimulateLogging(LogLevel::kVerbose, "MyAp", "MyCt", "Slog message", 100);
}

TEST_F(SlogBackendFixture, NoSlotAvailableShouldReturnEmptyHandle)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies returning empty handler in case of no available slots.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    SlogBackend backend(config_.GetNumberOfSlots(), log_record_, config_.GetAppId(), std::move(slog2_mock_));

    for (std::size_t i = 0; i < config_.GetNumberOfSlots(); ++i)
    {
        EXPECT_TRUE(backend.ReserveSlot().has_value());
    }

    EXPECT_FALSE(backend.ReserveSlot().has_value());
}

TEST_F(SlogBackendFixture, TooMuchSlotsRequestedShallBeTruncated)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Verifies requesting too much slots shall be truncated.");
    RecordProperty("TestingTechnique", "Requirements-based test");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    const auto kMaxSlotCount = std::numeric_limits<SlotIndex>::max();
    const std::size_t kSlotNumberOverflow = static_cast<std::size_t>(kMaxSlotCount) + 2UL;

    SlogBackend backend(kSlotNumberOverflow, log_record_, config_.GetAppId(), std::move(slog2_mock_));

    for (std::size_t i = 0; i < kMaxSlotCount; ++i)
    {
        EXPECT_TRUE(backend.ReserveSlot().has_value());
    }

    EXPECT_FALSE(backend.ReserveSlot().has_value());
}

TEST_F(SlogBackendFixture, ToSloggerLogLevelInvalidLevel)
{
    RecordProperty("ParentRequirement", "SCR-8017664");
    RecordProperty("Description", "Tests ToSloggerLogLevel with an invalid log level.");
    RecordProperty("TestingTechnique", "Boundary value analysis");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_CALL(*slog2_mock_raw_ptr_, slog2_register(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*slog2_mock_raw_ptr_, MockedSlog2f(_, _, SLOG2_INVALID_VERBOSITY, _)).Times(1);

    // Pass a log level greater than GetMaxLogLevelValue() to trigger the `else` branch
    LogLevel invalid_log_level = static_cast<LogLevel>(static_cast<std::uint8_t>(LogLevel::kVerbose) + 1);
    SimulateLogging(invalid_log_level);
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
