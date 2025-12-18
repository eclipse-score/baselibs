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
#ifndef SCORE_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_H_
#define SCORE_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_H_

#include "score/mw/log/detail/file_recorder/dlt_message_builder_types.h"
#include "score/mw/log/detail/text_recorder/imessage_builder.h"

#include <atomic>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

// This method was kept here in this namespace for testing purposes.
void ConstructDltStandardHeaderTypes(DltStandardHeader& standard,
                                     const std::uint16_t msg_size,
                                     const std::uint8_t message_count,
                                     const bool use_extended_header = false) noexcept;

class DltMessageBuilder : public IMessageBuilder
{
  public:
    explicit DltMessageBuilder(const std::string_view ecu_id) noexcept;
    score::cpp::optional<score::cpp::span<const std::uint8_t>> GetNextSpan() noexcept override;
    void SetNextMessage(LogRecord& log_record) noexcept override;

  private:
    enum class ParsingPhase : std::uint8_t
    {
        kHeader = 0,
        kPayload,
        kReinitialize,
    };
    score::cpp::optional<std::reference_wrapper<LogRecord>> log_record_;
    ByteVector header_memory_;
    detail::VerbosePayload header_payload_;
    ParsingPhase parsing_phase_;
    LoggingIdentifier ecu_id_;
    //  message_count_ variable must be one byte and must be allowed to overflow!
    std::atomic<uint8_t> message_count_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  SCORE_MW_LOG_DETAIL_DLT_MESSAGE_BUILDER_H_
