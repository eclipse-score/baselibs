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
#include "score/mw/log/detail/file_logging/dlt_message_builder.h"

#include "score/os/utils/high_resolution_steady_clock.h"
#include "score/mw/log/detail/dlt_format.h"
#include "score/mw/log/detail/file_logging/svp_time.h"
#include "score/mw/log/log_level.h"

#include "serialization/for_logging.h"
#include "visitor/visit.h"
#include "visitor/visit_as_struct.h"

#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <type_traits>

namespace
{

void ConstructDltStorageHeader(score::mw::log::detail::DltStorageHeader& storagehdr,
                               const std::uint32_t secs,
                               const std::int32_t microsecs) noexcept
{
    storagehdr.pattern[0] = static_cast<std::uint8_t>('D');
    storagehdr.pattern[1] = static_cast<std::uint8_t>('L');
    storagehdr.pattern[2] = static_cast<std::uint8_t>('T');
    storagehdr.pattern[3] = 0x01UL;
    storagehdr.seconds = secs;
    storagehdr.microseconds = microsecs;
    storagehdr.ecu[0] = static_cast<std::uint8_t>('E');
    storagehdr.ecu[1] = static_cast<std::uint8_t>('C');
    storagehdr.ecu[2] = static_cast<std::uint8_t>('U');
    storagehdr.ecu[3] = static_cast<std::uint8_t>('\0');
}

void ConstructDltStandardHeaderExtra(score::mw::log::detail::DltStandardHeaderExtra& standard_extra_header,
                                     const score::mw::log::detail::LoggingIdentifier& ecu,
                                     const std::uint32_t tmsp) noexcept
{
    static_assert(sizeof(decltype(standard_extra_header.ecu)) == sizeof(decltype(ecu.data_)),
                  "Types storing logging ID should be the same.");
    std::ignore = std::copy(ecu.data_.begin(), ecu.data_.end(), standard_extra_header.ecu.begin());
    // htonl is library function which uses c-style conversion
    // coverity[autosar_cpp14_a5_2_2_violation]
    standard_extra_header.tmsp = htonl(tmsp);
}

void ConstructDltExtendedHeader(score::mw::log::detail::DltExtendedHeader& extended_header,
                                const score::mw::log::LogLevel log_level,
                                const std::uint8_t number_of_arguments,
                                const score::mw::log::detail::LoggingIdentifier& app_id,
                                const score::mw::log::detail::LoggingIdentifier& ctx_id) noexcept
{
    static_assert(sizeof(std::uint32_t) > sizeof(log_level), "Casting to a more capable type expected");
    const std::uint32_t level_normalized = static_cast<std::uint32_t>(log_level) & static_cast<std::uint32_t>(0b111UL);
    const std::uint32_t message_info = (score::mw::log::detail::kDltTypeLOG << score::mw::log::detail::kDltMsinMstpShift) |
                                       (level_normalized << score::mw::log::detail::kDltMsinMtinShift) |
                                       (score::mw::log::detail::kDltMsinVerb);
    //  static_cast operation within uint8_t range
    extended_header.msin = static_cast<std::uint8_t>(message_info);
    extended_header.noar = number_of_arguments;
    static_assert(sizeof(decltype(extended_header.apid)) == sizeof(decltype(app_id.data_)),
                  "Types storing logging ID should be the same.");
    std::ignore = std::copy(app_id.data_.begin(), app_id.data_.end(), extended_header.apid.begin());
    static_assert(sizeof(decltype(extended_header.ctid)) == sizeof(decltype(ctx_id.data_)),
                  "Types storing logging ID should be the same.");
    std::ignore = std::copy(ctx_id.data_.begin(), ctx_id.data_.end(), extended_header.ctid.begin());
}

void ConstructStorageVerbosePacket(score::mw::log::detail::VerbosePayload& header_payload,
                                   const score::mw::log::detail::LogEntry& entry,
                                   const score::mw::log::detail::LoggingIdentifier& ecu,
                                   const std::uint8_t message_count,
                                   const score::mw::log::detail::SVPTime& svp_time) noexcept
{
    // truncate the message to max size if the msg size is exceeding the available buffer size
    static_assert(score::mw::log::detail::kDltMessageSize >
                      (score::mw::log::detail::kDltStorageHeaderSize + score::mw::log::detail::kDltHeaderSize),
                  "DLT constant values causes undefined behavior");
    const std::size_t size =
        std::min(entry.payload.size(),
                 score::mw::log::detail::kDltMessageSize -
                     (score::mw::log::detail::kDltStorageHeaderSize + score::mw::log::detail::kDltHeaderSize));
    static_assert(score::mw::log::detail::kDltMessageSize <= std::numeric_limits<std::uint16_t>::max(),
                  "Maximum size of DLT message is too big");
    //  'size' is truncated to allocate header without overflowing uint16_t value
    const auto header_size = static_cast<std::uint16_t>(score::mw::log::detail::kDltHeaderSize + size);

    score::mw::log::detail::DltStorageHeader storage_header{};
    ConstructDltStorageHeader(storage_header, svp_time.sec, svp_time.ms);

    std::ignore = header_payload.Put([&storage_header](const score::cpp::span<score::mw::log::detail::Byte> destination) {
        const auto destination_size = static_cast<std::size_t>(destination.size());
        const auto copy_size = std::min(destination_size, sizeof(storage_header));
        // NOLINTNEXTLINE(score-banned-function) memcpy is needed here
        std::ignore = std::memcpy(destination.data(), &storage_header, copy_size);
        return copy_size;
    });

    score::mw::log::detail::DltVerboseHeader dlt_header{};
    ::score::mw::log::detail::ConstructDltStandardHeaderTypes(dlt_header.standard, header_size, message_count, true);
    ConstructDltStandardHeaderExtra(dlt_header.extra, ecu, svp_time.timestamp);

    ConstructDltExtendedHeader(dlt_header.extended, entry.log_level, entry.num_of_args, entry.app_id, entry.ctx_id);

    std::ignore = header_payload.Put([&dlt_header](const score::cpp::span<score::mw::log::detail::Byte> destination) {
        const auto copy_size = std::min(static_cast<std::size_t>(destination.size()), sizeof(dlt_header));
        // NOLINTNEXTLINE(score-banned-function) memcpy is needed here
        std::ignore = std::memcpy(destination.data(), &dlt_header, copy_size);
        return copy_size;
    });
}

}  //  anonymous namespace

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

void ConstructDltStandardHeaderTypes(DltStandardHeader& standard,
                                     const std::uint16_t msg_size,
                                     const std::uint8_t message_count,
                                     const bool use_extended_header) noexcept
{
    //  static_cast allowed due to flags values within uint8_t range
    standard.htyp = static_cast<std::uint8_t>(kDltHtypWEID | kDltHtypWTMS | kDltHtypVERS);
    if (use_extended_header)
    {
        standard.htyp |= static_cast<std::uint8_t>(kDltHtypUEH);
    }
    standard.mcnt = message_count;
    // htons is library function which uses c-style conversion
    // coverity[autosar_cpp14_a5_2_2_violation]
    standard.len = htons(msg_size);
}

using timestamp_t = score::os::HighResolutionSteadyClock::time_point;
using systime_t = std::chrono::system_clock::time_point;
using dlt_duration_t = std::chrono::duration<std::uint32_t, std::ratio<1, 10000>>;

DltMessageBuilder::DltMessageBuilder(const std::string_view ecu_id) noexcept
    : IMessageBuilder(),
      header_payload_(kMaxDltHeaderSize, header_memory_),
      parsing_phase_{ParsingPhase::kHeader},
      ecu_id_{ecu_id},
      message_count_{0U}
{
}

void DltMessageBuilder::SetNextMessage(LogRecord& log_record) noexcept
{
    log_record_ = log_record;

    const auto& entry = log_record.getLogEntry();
    const auto time_stamp = timestamp_t::clock::now().time_since_epoch();
    const auto time_epoch = systime_t::clock::now().time_since_epoch();

    using secs_u32 = std::chrono::duration<std::uint32_t, std::ratio<1>>;
    const std::uint32_t seconds = std::chrono::duration_cast<secs_u32>(time_epoch).count();
    const auto secs_remainder = time_epoch - std::chrono::seconds(seconds);

    using microsecs_i32 = std::chrono::duration<std::int32_t, std::micro>;
    const std::int32_t microsecs = std::chrono::duration_cast<microsecs_i32>(secs_remainder).count();
    const std::uint32_t timestamp = std::chrono::duration_cast<dlt_duration_t>(time_stamp).count();

    /*
    Deviation from AUTOSAR C++14 Rule A4-7-1
    - An integer expression shall not lead to data loss
    Justification:
    - Message Counter overflow is intended by design by DLT protocol. It is just an information about sequence of DLT
    messages and overflow does not lead to any data loss.
    */
    // coverity[autosar_cpp14_a4_7_1_violation] see above
    const auto dlt_payload_message_count_value = message_count_.fetch_add(1UL);
    ConstructStorageVerbosePacket(header_payload_,
                                  entry,
                                  score::mw::log::detail::LoggingIdentifier{ecu_id_.GetStringView()},
                                  dlt_payload_message_count_value,
                                  score::mw::log::detail::SVPTime{timestamp, seconds, microsecs});
}

score::cpp::optional<score::cpp::span<const std::uint8_t>> DltMessageBuilder::GetNextSpan() noexcept
{
    if (!log_record_.has_value())
    {
        return {};
    }

    score::cpp::optional<score::cpp::span<const std::uint8_t>> return_result = {};

    detail::VerbosePayload& verbose_payload = log_record_.value().get().getVerbosePayload();
    switch (parsing_phase_)  // LCOV_EXCL_BR_LINE: exclude the "default" branch.
    {
        case ParsingPhase::kHeader:
            parsing_phase_ = ParsingPhase::kPayload;
            return_result = header_payload_.GetSpan();
            break;
        case ParsingPhase::kPayload:
            parsing_phase_ = ParsingPhase::kReinitialize;
            return_result = verbose_payload.GetSpan();
            break;
        case ParsingPhase::kReinitialize:
            parsing_phase_ = ParsingPhase::kHeader;
            header_payload_.Reset();
            verbose_payload.Reset();
            log_record_.reset();
            break;
        // LCOV_EXCL_START
        // This part of the code should never be reached due to logic in processing state transition in other switch
        // cases
        default:
            std::cerr << "Enum ParsingPhase contains wrong value\n";
            return_result = score::cpp::nullopt;
            break;
            // LCOV_EXCL_STOP
    }
    return return_result;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
