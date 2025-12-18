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
#include "score/mw/log/detail/slog/slog_backend.h"

#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/initialization_reporter.h"

#include <score/assert.hpp>
#include <algorithm>
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
constexpr auto SLOG_BUFFER_DEFAULT = 0;
constexpr auto SLOG_VERBOSITY_DEFAULT = SLOG2_DEBUG2;

std::size_t CheckTheMaxCapacity(const std::size_t capacity) noexcept
{
    const auto is_within_max_capacity = (capacity <= std::numeric_limits<SlotIndex>::max());
    if (is_within_max_capacity)
    {
        return capacity;
    }
    else
    {
        return static_cast<std::size_t>(std::numeric_limits<SlotIndex>::max());
    }
}

enum class SlogLogLevel : std::uint8_t
{
    kDebug2 = SLOG2_DEBUG2,
    kDebug1 = SLOG2_DEBUG1,
    kInfo = SLOG2_INFO,
    kWarning = SLOG2_WARNING,
    kError = SLOG2_ERROR,
    kCritical = SLOG2_CRITICAL,
    kInvalid = SLOG2_INVALID_VERBOSITY
};

constexpr SlogLogLevel ConvertMwLogLevelToSlogLevel(const LogLevel level)
{
    SlogLogLevel slog_log_level = SlogLogLevel::kInvalid;
    switch (level)
    {
        case LogLevel::kVerbose:
            slog_log_level = SlogLogLevel::kDebug2;
            break;
        case LogLevel::kDebug:
            slog_log_level = SlogLogLevel::kDebug1;
            break;
        case LogLevel::kInfo:
            slog_log_level = SlogLogLevel::kInfo;
            break;
        case LogLevel::kWarn:
            slog_log_level = SlogLogLevel::kWarning;
            break;
        case LogLevel::kError:
            slog_log_level = SlogLogLevel::kError;
            break;
        case LogLevel::kFatal:
            slog_log_level = SlogLogLevel::kCritical;
            break;
        case LogLevel::kOff:
        default:
            slog_log_level = SlogLogLevel::kInvalid;
            break;
    }
    return slog_log_level;
}

constexpr SlogLogLevel ToSloggerLogLevel(const LogLevel log_level) noexcept
{
    if (log_level <= GetMaxLogLevelValue())
    {
        return ConvertMwLogLevelToSlogLevel(log_level);
    }
    else
    {
        return SlogLogLevel::kInvalid;
    }
}

}  //  namespace

SlogBackend::SlogBackend(
    const std::size_t number_of_slots,
    const LogRecord& initial_slot_value,
    const std::string_view app_id,
    score::cpp::pmr::unique_ptr<score::os::qnx::Slog2> slog2_instance /* = score::os::qnx::Slog2() */) noexcept
    : Backend::Backend(),
      app_id_{app_id.data(), app_id.size()},
      buffer_{CheckTheMaxCapacity(number_of_slots), initial_slot_value},
      slog_buffer_{},
      slog_buffer_config_{},
      slog2_instance_{std::move(slog2_instance)}
{
    Init(static_cast<std::uint8_t>(SLOG_VERBOSITY_DEFAULT));
}

score::cpp::optional<SlotHandle> SlogBackend::ReserveSlot() noexcept
{
    const auto& slot = buffer_.AcquireSlotToWrite();
    if (slot.has_value())
    {
        if (slot.value() < std::numeric_limits<SlotIndex>::max())  // LCOV_EXCL_BR_LINE: As it always true case,we can't
        //  control slot.value() it is received from AcquireSlotToWrite() function
        // which wraps around and resulting in a value within the valid range.
        {
            // CircularAllocator has capacity limited by CheckFoxMaxCapacity thus the cast is valid:
            // We intentionally static cast to SlotIndex(uint8_t) to limit memory allocations
            // to the required levels during startup, since there is no need to support slots greater
            // than uint8 as per the current system needs.
            // coverity[autosar_cpp14_a4_7_1_violation]
            return SlotHandle{static_cast<SlotIndex>(slot.value())};
        }
    }
    return {};
}

LogRecord& SlogBackend::GetLogRecord(const SlotHandle& slot) noexcept
{
    // static cast from std::uint8_t to std::size_t
    return buffer_.GetUnderlyingBufferFor(static_cast<std::size_t>(slot.GetSlotOfSelectedRecorder()));
}

void SlogBackend::FlushSlot(const SlotHandle& slot) noexcept
{
    // static cast from std::uint8_t to std::size_t
    auto& log_entry =
        buffer_.GetUnderlyingBufferFor(static_cast<std::size_t>(slot.GetSlotOfSelectedRecorder())).getLogEntry();

    constexpr std::size_t max_id_length{4U};

    // Cast appid length to int32 without overflow.
    const std::int32_t app_id_length = static_cast<int32_t>(std::min(max_id_length, app_id_.size()));

    // Cast context length to int32 without overflow.
    const std::int32_t ctx_id_length =
        static_cast<int32_t>(std::min(max_id_length, log_entry.ctx_id.GetStringView().size()));

    // Cast payload size to int32_t without overflow.
    const std::int32_t payload_length = static_cast<int32_t>(
        std::min(static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()), log_entry.payload.size()));

// We need to support Linux variant for testing.
// coverity[autosar_cpp14_a16_0_1_violation]
#if defined __QNX__
    const auto slog2 = log_entry.slog2_code;
// coverity[autosar_cpp14_a16_0_1_violation]
#else
    const auto slog2 = 0;
// coverity[autosar_cpp14_a16_0_1_violation]
#endif
    // Log message with appid and ctxid.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg) no available alternative for slog2f
    std::ignore = slog2_instance_->slog2f(slog_buffer_,
                                          slog2,
                                          static_cast<std::uint8_t>(ToSloggerLogLevel(log_entry.log_level)),
                                          "%.*s,%.*s: %.*s",
                                          app_id_length,
                                          app_id_.c_str(),
                                          ctx_id_length,
                                          // above variable `ctx_id_length` contains corresponding length information
                                          // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage) justified above
                                          log_entry.ctx_id.GetStringView().data(),
                                          payload_length,
                                          log_entry.payload.data());

    buffer_.ReleaseSlot(static_cast<std::size_t>(slot.GetSlotOfSelectedRecorder()));
}

void SlogBackend::Init(const std::uint8_t verbosity) noexcept
{
    slog_buffer_config_.num_buffers = 1;
    slog_buffer_config_.buffer_set_name = app_id_.c_str();
    slog_buffer_config_.verbosity_level = verbosity;
    slog_buffer_config_.buffer_config[SLOG_BUFFER_DEFAULT].buffer_name = app_id_.c_str();
    slog_buffer_config_.buffer_config[SLOG_BUFFER_DEFAULT].num_pages = 16;  // 16*4kB = 64kB

    const auto result = slog2_instance_->slog2_register(&slog_buffer_config_, &slog_buffer_, 0U);
    if (result.has_value() == false)
    {
        const auto underlying_error = result.error().ToStringContainer(result.error());
        ReportInitializationError(Error::kSloggerError, underlying_error.data());
    }
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
