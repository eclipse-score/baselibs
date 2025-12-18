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
#include "score/mw/log/detail/empty_recorder.h"

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

class EmptyRecorderFixture : public ::testing::Test
{
  public:
    EmptyRecorder emptyRecorder;
};

TEST_F(EmptyRecorderFixture, StartRecord)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains StartRecord function for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.StartRecord("DFLT", LogLevel::kInfo);
}

TEST_F(EmptyRecorderFixture, StopRecord)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633144");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains StopRecord function for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.StopRecord(SlotHandle{});
}

TEST_F(EmptyRecorderFixture, LogBool)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log boolean for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, bool{});
}

TEST_F(EmptyRecorderFixture, LogUint8_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log uint8_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::uint8_t{});
}

TEST_F(EmptyRecorderFixture, LogInt8_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log int8_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::int8_t{});
}

TEST_F(EmptyRecorderFixture, LogUint16_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log uint16_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::uint16_t{});
}

TEST_F(EmptyRecorderFixture, LogInt16_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log int16_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::int16_t{});
}

TEST_F(EmptyRecorderFixture, LogUint32_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log uint32_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::uint32_t{});
}

TEST_F(EmptyRecorderFixture, LogInt32_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log int32_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::int32_t{});
}

TEST_F(EmptyRecorderFixture, LogUint64_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log uint64_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::uint64_t{});
}

TEST_F(EmptyRecorderFixture, LogInt64_t)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log int64_t for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::int64_t{});
}

TEST_F(EmptyRecorderFixture, LogFloat)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log float for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, float{});
}

TEST_F(EmptyRecorderFixture, LogDouble)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log double for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, double{});
}

TEST_F(EmptyRecorderFixture, LogStringView)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log string view for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, std::string_view{});
}

TEST_F(EmptyRecorderFixture, Log_LogHex8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 8 bits with hex represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogHex8{});
}

TEST_F(EmptyRecorderFixture, Log_LogHex16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 16 bits with hex represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogHex16{});
}

TEST_F(EmptyRecorderFixture, Log_LogHex32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 32 bits with hex represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogHex32{});
}

TEST_F(EmptyRecorderFixture, Log_LogHex64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 64 bits with hex represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogHex64{});
}

TEST_F(EmptyRecorderFixture, Log_LogBin8)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 8 bits with bin represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogBin8{});
}

TEST_F(EmptyRecorderFixture, Log_LogBin16)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 16 bits with bin represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogBin16{});
}

TEST_F(EmptyRecorderFixture, Log_LogBin32)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 32 bits with bin represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogBin32{});
}

TEST_F(EmptyRecorderFixture, Log_LogBin64)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty(
        "Description",
        "EmptyRecorder contains log function which can log 64 bits with bin represenataton for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogBin64{});
}

TEST_F(EmptyRecorderFixture, Log_LogRawBuffer)
{
    RecordProperty("Requirement", "SCR-861534, SCR-1633236");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains log function which can log raw buffer for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogRawBuffer{nullptr, 0});
}

TEST_F(EmptyRecorderFixture, Log_LogSlog2Message)
{
    RecordProperty("ASIL", "B");
    RecordProperty("Description",
                   "EmptyRecorder contains log function which can log LogSlog2Message for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    emptyRecorder.Log(SlotHandle{}, LogSlog2Message{11, "slog message"});
}

TEST_F(EmptyRecorderFixture, IsLogEnabledShallReturnValue)
{
    RecordProperty("Requirement", "SCR-861534, SCR-2592577");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "EmptyRecorder contains IsLogEnabled for testing purpose.");
    RecordProperty("TestingTechnique", "requirements-based");
    RecordProperty("DerivationTechnique", "Analysis of requirements");

    EXPECT_FALSE(emptyRecorder.IsLogEnabled(LogLevel::kInfo, std::string_view{"DFLT"}));
}

}  // namespace
}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
