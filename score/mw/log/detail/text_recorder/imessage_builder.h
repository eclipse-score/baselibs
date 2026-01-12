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
#ifndef MW_LOG_DETAIL_IMESSAGE_BUILDER_H_
#define MW_LOG_DETAIL_IMESSAGE_BUILDER_H_

#include "score/mw/log/detail/log_record.h"

#include "score/optional.hpp"
#include "score/span.hpp"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class IMessageBuilder
{
  public:
    IMessageBuilder() noexcept = default;
    IMessageBuilder(IMessageBuilder&&) noexcept = delete;
    IMessageBuilder(const IMessageBuilder&) noexcept = delete;
    IMessageBuilder& operator=(IMessageBuilder&&) noexcept = delete;
    IMessageBuilder& operator=(const IMessageBuilder&) noexcept = delete;

    /// \brief Get next span for consecutive memory area for next part of message that is getting serialized
    /// \details Specific implementations may build and split message into different number of spans
    virtual score::cpp::optional<score::cpp::span<const std::uint8_t>> GetNextSpan() noexcept = 0;
    /// \brief Set data for building next message
    /// \details Build header and payload from data contained in LogRecorder
    virtual void SetNextMessage(LogRecord&) noexcept = 0;
    virtual ~IMessageBuilder() noexcept;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  MW_LOG_DETAIL_IMESSAGE_BUILDER_H_
