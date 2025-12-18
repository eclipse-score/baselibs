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
#ifndef SCORE_MW_LOG_DETAIL_COMMON_COMPOSITE_RECORDER_H
#define SCORE_MW_LOG_DETAIL_COMMON_COMPOSITE_RECORDER_H

#include "score/mw/log/recorder.h"

#include <memory>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief CompositeRecorder forwards the log statement to one or more concrete Recorder(s).
class CompositeRecorder final : public Recorder
{
  public:
    explicit CompositeRecorder(std::vector<std::unique_ptr<Recorder>> recorders) noexcept;

    score::cpp::optional<SlotHandle> StartRecord(const std::string_view context_id,
                                          const LogLevel log_level) noexcept override;

    void StopRecord(const SlotHandle& slot) noexcept override;

    void Log(const SlotHandle& composite_slot, const bool data) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::uint8_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::int8_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::uint16_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::int16_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::uint32_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::int32_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::uint64_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::int64_t arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const float arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const double arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const std::string_view arg) noexcept override;

    void Log(const SlotHandle& composite_slot, const LogHex8 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogHex16 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogHex32 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogHex64 arg) noexcept override;

    void Log(const SlotHandle& composite_slot, const LogBin8 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogBin16 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogBin32 arg) noexcept override;
    void Log(const SlotHandle& composite_slot, const LogBin64 arg) noexcept override;

    void Log(const SlotHandle& composite_slot, const LogRawBuffer arg) noexcept override;

    void Log(const SlotHandle& composite_slot, const LogSlog2Message) noexcept override;

    const std::vector<std::unique_ptr<Recorder>>& GetRecorders() const noexcept;

    bool IsLogEnabled(const LogLevel& log_level, const std::string_view context) const noexcept override;

  private:
    std::vector<std::unique_ptr<Recorder>> recorders_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_COMMON_COMPOSITE_RECORDER_H
