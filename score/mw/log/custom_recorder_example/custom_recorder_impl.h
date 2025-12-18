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
#ifndef SCORE_MW_LOG_CUSTOM_RECORDER_IMPL_H
#define SCORE_MW_LOG_CUSTOM_RECORDER_IMPL_H

#include "score/mw/log/recorder.h"

namespace user
{
namespace specific
{
namespace impl
{
namespace detail
{

using SlotHandle = score::mw::log::SlotHandle;

class CustomRecorderImpl final : public score::mw::log::Recorder
{
  public:
    score::cpp::optional<score::mw::log::SlotHandle> StartRecord(const std::string_view context_id,
                                                        const score::mw::log::LogLevel log_level) noexcept override;

    void StopRecord(const SlotHandle& slot) noexcept override;

    void Log(const SlotHandle&, const bool data) noexcept override;
    void Log(const SlotHandle&, const std::uint8_t) noexcept override;
    void Log(const SlotHandle&, const std::int8_t) noexcept override;
    void Log(const SlotHandle&, const std::uint16_t) noexcept override;
    void Log(const SlotHandle&, const std::int16_t) noexcept override;
    void Log(const SlotHandle&, const std::uint32_t) noexcept override;
    void Log(const SlotHandle&, const std::int32_t) noexcept override;
    void Log(const SlotHandle&, const std::uint64_t) noexcept override;
    void Log(const SlotHandle&, const std::int64_t) noexcept override;
    void Log(const SlotHandle&, const float) noexcept override;
    void Log(const SlotHandle&, const double) noexcept override;
    void Log(const SlotHandle&, const std::string_view) noexcept override;

    void Log(const SlotHandle&, const score::mw::log::LogHex8) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogHex16) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogHex32) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogHex64) noexcept override;

    void Log(const SlotHandle&, const score::mw::log::LogBin8) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogBin16) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogBin32) noexcept override;
    void Log(const SlotHandle&, const score::mw::log::LogBin64) noexcept override;

    void Log(const SlotHandle&, const score::mw::log::LogRawBuffer) noexcept override;

    void Log(const SlotHandle&, const score::mw::log::LogSlog2Message) noexcept override;

    bool IsLogEnabled(const score::mw::log::LogLevel&, const std::string_view) const noexcept override;
};

}  // namespace detail
}  // namespace impl
}  // namespace specific
}  // namespace user

#endif  // SCORE_MW_LOG_CUSTOM_RECORDER_IMPL_H
