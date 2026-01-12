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
#ifndef SCORE_MW_LOG_TEXT_RECORDER_H_
#define SCORE_MW_LOG_TEXT_RECORDER_H_

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/detail/backend.h"
#include "score/mw/log/recorder.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class TextRecorder : public Recorder
{
  public:
    TextRecorder(const detail::Configuration& config,
                 std::unique_ptr<detail::Backend> backend,
                 const bool check_log_level_for_console) noexcept;
    TextRecorder(TextRecorder&&) noexcept = delete;
    TextRecorder(const TextRecorder&) noexcept = delete;
    TextRecorder& operator=(TextRecorder&&) noexcept = delete;
    TextRecorder& operator=(const TextRecorder&) noexcept = delete;

    ~TextRecorder() override = default;

    score::cpp::optional<SlotHandle> StartRecord(const std::string_view context_id,
                                          const LogLevel log_level) noexcept override;
    void StopRecord(const SlotHandle& slot) noexcept override;

    void Log(const SlotHandle& slot, const bool data) noexcept override;
    void Log(const SlotHandle& slot, const std::uint8_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::int8_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::uint16_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::int16_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::uint32_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::int32_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::uint64_t data) noexcept override;
    void Log(const SlotHandle& slot, const std::int64_t data) noexcept override;
    void Log(const SlotHandle& slot, const float data) noexcept override;
    void Log(const SlotHandle& slot, const double data) noexcept override;
    void Log(const SlotHandle& slot, const LogRawBuffer data) noexcept override;
    void Log(const SlotHandle& slot, const std::string_view data) noexcept override;
    void Log(const SlotHandle& slot, const LogHex8 data) noexcept override;
    void Log(const SlotHandle& slot, const LogHex16 data) noexcept override;
    void Log(const SlotHandle& slot, const LogHex32 data) noexcept override;
    void Log(const SlotHandle& slot, const LogHex64 data) noexcept override;
    void Log(const SlotHandle& slot, const LogBin8 data) noexcept override;
    void Log(const SlotHandle& slot, const LogBin16 data) noexcept override;
    void Log(const SlotHandle& slot, const LogBin32 data) noexcept override;
    void Log(const SlotHandle& slot, const LogBin64 data) noexcept override;
    void Log(const SlotHandle& slot, const LogSlog2Message data) noexcept override;

    bool IsLogEnabled(const LogLevel&, const std::string_view) const noexcept override;

  private:
    std::unique_ptr<detail::Backend> backend_;

    detail::Configuration config_;
    bool check_log_level_for_console_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  //  SCORE_MW_LOG_TEXT_RECORDER_H_
