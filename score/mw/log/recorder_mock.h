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
#ifndef SCORE_MW_LOG_RECORDER_MOCK_H
#define SCORE_MW_LOG_RECORDER_MOCK_H

#include "gmock/gmock.h"
#include "score/mw/log/recorder.h"

namespace score
{
namespace mw
{
namespace log
{

class RecorderMock : public Recorder
{
  public:
    MOCK_METHOD(score::cpp::optional<SlotHandle>,
                StartRecord,
                (const std::string_view context_id, const LogLevel log_level),
                (override, noexcept));
    MOCK_METHOD(void, StopRecord, (const SlotHandle& slot), (override, noexcept));

    MOCK_METHOD(void, LogBool, (const SlotHandle&, bool data), (noexcept));
    void Log(const SlotHandle& handle, bool value) noexcept override
    {
        LogBool(handle, value);
    }

    MOCK_METHOD(void, LogUint8, (const SlotHandle&, std::uint8_t), (noexcept));
    void Log(const SlotHandle& handle, std::uint8_t value) noexcept override
    {
        LogUint8(handle, value);
    }
    MOCK_METHOD(void, LogInt8, (const SlotHandle&, std::int8_t), (noexcept));
    void Log(const SlotHandle& handle, std::int8_t value) noexcept override
    {
        LogInt8(handle, value);
    }

    MOCK_METHOD(void, LogUint16, (const SlotHandle&, std::uint16_t), (noexcept));
    void Log(const SlotHandle& handle, std::uint16_t value) noexcept override
    {
        LogUint16(handle, value);
    }
    MOCK_METHOD(void, LogInt16, (const SlotHandle&, std::int16_t), (noexcept));
    void Log(const SlotHandle& handle, std::int16_t value) noexcept override
    {
        LogInt16(handle, value);
    }

    MOCK_METHOD(void, LogUint32, (const SlotHandle&, std::uint32_t), (noexcept));
    void Log(const SlotHandle& handle, std::uint32_t value) noexcept override
    {
        LogUint32(handle, value);
    }
    MOCK_METHOD(void, LogInt32, (const SlotHandle&, std::int32_t), (noexcept));
    void Log(const SlotHandle& handle, std::int32_t value) noexcept override
    {
        LogInt32(handle, value);
    }

    MOCK_METHOD(void, LogUint64, (const SlotHandle&, std::uint64_t), (noexcept));
    void Log(const SlotHandle& handle, std::uint64_t value) noexcept override
    {
        LogUint64(handle, value);
    }
    MOCK_METHOD(void, LogInt64, (const SlotHandle&, std::int64_t), (noexcept));
    void Log(const SlotHandle& handle, std::int64_t value) noexcept override
    {
        LogInt64(handle, value);
    }

    MOCK_METHOD(void, LogFloat, (const SlotHandle&, float), (noexcept));
    void Log(const SlotHandle& handle, float value) noexcept override
    {
        LogFloat(handle, value);
    }
    MOCK_METHOD(void, LogDouble, (const SlotHandle&, double), (noexcept));
    void Log(const SlotHandle& handle, double value) noexcept override
    {
        LogDouble(handle, value);
    }

    MOCK_METHOD(void, LogStringView, (const SlotHandle&, std::string_view), (noexcept));
    void Log(const SlotHandle& handle, std::string_view value) noexcept override
    {
        LogStringView(handle, value);
    }

    void Log(const SlotHandle& handle, LogHex8 value) noexcept override
    {
        LogUint8(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogHex16 value) noexcept override
    {
        LogUint16(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogHex32 value) noexcept override
    {
        LogUint32(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogHex64 value) noexcept override
    {
        LogUint64(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogBin8 value) noexcept override
    {
        LogUint8(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogBin16 value) noexcept override
    {
        LogUint16(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogBin32 value) noexcept override
    {
        LogUint32(handle, value.value);
    }

    void Log(const SlotHandle& handle, LogBin64 value) noexcept override
    {
        LogUint64(handle, value.value);
    }

    MOCK_METHOD(void, Log_LogRawBuffer, (const SlotHandle&, const void*, const uint64_t), (noexcept));
    void Log(const SlotHandle& handle, LogRawBuffer value) noexcept override
    {
        Log_LogRawBuffer(handle, value.data(), value.size());
    }

    MOCK_METHOD(void, Log_LogSlog2Message, (const SlotHandle&, const uint16_t, const LogString), (noexcept));
    void Log(const SlotHandle& handle, LogSlog2Message value) noexcept override
    {
        Log_LogSlog2Message(handle, value.GetCode(), value.GetMessage());
    }

    // TODO: To be uncommented once Ticket-106852 is resolved.
    // MOCK_METHOD(void, LogSpan, (const SlotHandle&, const void*, const uint64_t), (noexcept));

    MOCK_METHOD(bool, IsLogEnabled, (const LogLevel&, const std::string_view context), (override, noexcept, const));
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_RECORDER_MOCK_H
