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
#include "score/mw/log/log_stream.h"

#include "score/mw/log/detail/thread_local_guard.h"
#include "score/mw/log/recorder.h"

#include <cstdint>
#include <cstring>

namespace score
{
namespace mw
{
namespace log
{

namespace
{

constexpr std::string_view kDefaultContextInStream{"DFLT"};

}  // namespace

LogStream::LogStream(Recorder& recorder,
                     Recorder& fallback_recorder,
                     const LogLevel log_level,
                     const std::string_view context_id) noexcept
    : recorder_{recorder},
      fallback_recorder_{fallback_recorder},
      context_id_(context_id.data() == nullptr ? kDefaultContextInStream : context_id),
      log_level_{log_level}
{
    // Construction fallback handled in log_stream_factory (using here CallRecorder, would give a false impression)
    slot_ = recorder_.StartRecord(context_id_.GetStringView(), log_level_);
}

LogStream::~LogStream() noexcept
{
    if (slot_.has_value())
    {
        CallOnRecorder(&Recorder::StopRecord, slot_.value());
    }
}

/*
Deviation from Rule A12-8-4:
- Move constructor shall not initialize its class members and base classes using copy semantics.

Deviation from Rule A18-9-2:
- Forwarding values to other functions shall be done via:
- (1) std::move if the value is an rvalue reference,
- (2) std::forward if the value is forwarding reference.
Justification:
- need to initialize the class member by copying the value.
*/
LogStream::LogStream(LogStream&& other) noexcept
    : recorder_{other.recorder_},
      fallback_recorder_{other.fallback_recorder_},
      // coverity[autosar_cpp14_a12_8_4_violation] see above
      // coverity[autosar_cpp14_a18_9_2_violation] see above
      slot_{other.slot_},
      // coverity[autosar_cpp14_a12_8_4_violation] see above
      context_id_{other.context_id_},
      log_level_{other.log_level_}
{
    // Detach the moved-from log stream from the slot to ensure the slot is only owned by one LogStream.
    // coverity[autosar_cpp14_a18_9_2_violation] see above
    other.slot_.reset();

    other.context_id_ = detail::LoggingIdentifier{""};
    other.log_level_ = LogLevel::kOff;
}

void LogStream::Flush() noexcept
{
    if (slot_.has_value())
    {
        CallOnRecorder(&Recorder::StopRecord, slot_.value());
    }
    slot_ = CallOnRecorder(&Recorder::StartRecord, context_id_.GetStringView(), log_level_);
}

LogStream& LogStream::Log(const bool value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int8_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int16_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int32_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::int64_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint8_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint16_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint32_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const std::uint64_t value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const float value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const double value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogString value) noexcept
{
    if (value.Data() == nullptr)
    {
        return *this;
    }
    return LogWithRecorder(std::string_view{value.Data(), value.Size()});
}

LogStream& LogStream::Log(const LogHex8& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex16& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex32& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogHex64& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin8& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin16& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin32& value) noexcept
{
    return LogWithRecorder(value);
}

LogStream& LogStream::Log(const LogBin64& value) noexcept
{
    return LogWithRecorder(value);
}

/*
Deviation from Rule A0-1-3:
- Every function defined in an anonymous namespace, or static function with internal linkage,
    or private member function shall be used.
Justification:
- The function is used.
*/
// coverity[autosar_cpp14_a0_1_3_violation : FALSE] see above
LogStream& LogStream::Log(const LogSlog2Message& value) noexcept
{
    return LogWithRecorder(value);
}

template <>
// Log method for `LogRawBuffer` must be templated so that the overload for `LogString` always
// has higher precedence during overload resolution in case the parameter type is string-like.
// coverity[autosar_cpp14_a14_8_2_violation]
// coverity[autosar_cpp14_a14_7_2_violation]
LogStream& LogStream::Log(const LogRawBuffer& value) noexcept
{
    if (value.data() == nullptr)
    {
        return *this;
    }
    return LogWithRecorder(value);
}

template <typename T>
LogStream& LogStream::LogWithRecorder(const T value) noexcept
{
    if (slot_.has_value())
    {
        CallOnRecorder<void, const SlotHandle&, const T>(&Recorder::Log, slot_.value(), value);
    }
    return *this;
}

/*
Deviation from Rule A8-2-1:
- When declaring function templates, the trailing return type syntax shall be used if the return type depends on the
  type of parameters.
Justification:
- the return type of the template function does not depend on the type of parameters.
*/
// Magic function, to dispatch any recorder function to either the default recorder (if not in logging stack) or the
// fallback recorder if called within the logging stack. #templatemagic
template <typename ReturnValue, typename... ArgsOfFunction, typename... ArgsPassed>
// Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
// NOLINTBEGIN(score-no-pointer-to-member): See above
// coverity[autosar_cpp14_a8_2_1_violation] see above
ReturnValue LogStream::CallOnRecorder(ReturnValue (Recorder::*arbitrary_function)(ArgsOfFunction...) noexcept,
                                      ArgsPassed&&... args) noexcept
// NOLINTEND(score-no-pointer-to-member): See above
{
    if (not detail::ThreadLocalGuard::IsWithingLogging())
    {
        /*
        Deviation from Rule M0-1-3:
        - A project shall not contain unused variables.

        Deviation from Rule M0-1-9:
        - There shall be no dead code.
        Justification:
        - The ThreadLocalGuard is used to indicate whether we are within logging stack or not.
        */
        // coverity[autosar_cpp14_m0_1_3_violation]
        // coverity[autosar_cpp14_m0_1_9_violation]
        detail::ThreadLocalGuard guard{};
        // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
        // NOLINTNEXTLINE(score-no-pointer-to-member): See above
        return (recorder_.*arbitrary_function)(std::forward<ArgsPassed>(args)...);
    }
    else
    {
        // Checker overly strict, pointer of object not null and only existing functions called (ensure by typeset)
        // NOLINTNEXTLINE(score-no-pointer-to-member): See above
        return (fallback_recorder_.*arbitrary_function)(std::forward<ArgsPassed>(args)...);
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
