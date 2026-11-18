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
#ifndef SCORE_MW_LOG_RECORDER_H
#define SCORE_MW_LOG_RECORDER_H

#include "score/mw/log/log_level.h"
#include "score/mw/log/log_types.h"
#include "score/mw/log/slot_handle.h"

#include "score/optional.hpp"
#include <string_view>

#include <cstdint>

namespace score
{
namespace mw
{
namespace log
{

/// \brief Base class for any actual recorder implementation
///
/// \details The main idea of an recorder to is to store logging information in a reserved memory slot.
/// As an interface it is the correct way to mock any actual recorder.
/// A Recorder will normally interact with a `Backend` and a `Formatter` to store streamed data in the right format into
/// correct memory slot.
///
/// Please be advised, if you plan to extend our logging API with your personal complex type, this header is _not_ the
/// right place. The idea is that a `Recorder` only supports the basic types in C++. If you want to ensure that your
/// custom complex type is loggable, please provide a custom overload of the operator<<(LogStream).
class Recorder
{
  public:
    /// \brief We don't support copy or move operations for our Recorder, since it shall act as single instance.
    ///
    /// An unattended copy or move could cause weired side-effect, since implementations should own storage (e.g. shared
    /// memory).
    Recorder() = default;
    virtual ~Recorder();
    Recorder(const Recorder&) noexcept = delete;
    Recorder(Recorder&&) noexcept = delete;
    Recorder& operator=(const Recorder&) noexcept = delete;
    Recorder& operator=(Recorder&&) noexcept = delete;

    /// \brief Acquire a slot from a respective backend to enable streaming into it.
    ///
    /// \param context_id The context under which the message shall be identified.
    /// \param log_level The log level under which the message shall be treated.
    /// \return SlotHandle if able to be acquired, empty otherwise.
    ///
    /// \post A call to Log() with respective SlotHandle is possible.
    virtual score::cpp::optional<SlotHandle> StartRecord(const std::string_view context_id,
                                                         const LogLevel log_level) noexcept = 0;

    /// \brief Indicate that a message has finished and free slot for next write
    ///
    /// \param slot The slot to be finished up
    virtual void StopRecord(const SlotHandle& slot) noexcept = 0;

    /// In the following we specify all basic types that can be logged our Recorders.
    /// The idea is that a slot must be provided, where the respective data will be stored.

    virtual void Log(const SlotHandle&, const bool data) noexcept = 0;

    virtual void Log(const SlotHandle&, const std::uint8_t) noexcept = 0;
    virtual void Log(const SlotHandle&, const std::int8_t) noexcept = 0;

    virtual void Log(const SlotHandle&, const std::uint16_t) noexcept = 0;
    virtual void Log(const SlotHandle&, const std::int16_t) noexcept = 0;

    virtual void Log(const SlotHandle&, const std::uint32_t) noexcept = 0;
    virtual void Log(const SlotHandle&, const std::int32_t) noexcept = 0;

    virtual void Log(const SlotHandle&, const std::uint64_t) noexcept = 0;
    virtual void Log(const SlotHandle&, const std::int64_t) noexcept = 0;

    virtual void Log(const SlotHandle&, const float) noexcept = 0;
    virtual void Log(const SlotHandle&, const double) noexcept = 0;

    virtual void Log(const SlotHandle&, const std::string_view) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogHex8) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogHex16) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogHex32) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogHex64) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogBin8) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogBin16) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogBin32) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogBin64) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogRawBuffer) noexcept = 0;

    virtual void Log(const SlotHandle&, const LogSlog2Message) noexcept = 0;

    virtual bool IsLogEnabled(const LogLevel&, const std::string_view context) const noexcept = 0;
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_RECORDER_H
