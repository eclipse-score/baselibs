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
#ifndef SCORE_MW_LOG_LOGGING_H
#define SCORE_MW_LOG_LOGGING_H

// Be careful what you include here. Each additional header will be exposed to the user.
// We need to try to keep the includes low to reduce the compile footprint of using this library.
#include "score/mw/log/log_stream.h"

#include <string_view>

namespace score
{
namespace mw
{

namespace log
{
// We forward declare Recorder to reduce the include chain
class Recorder;

// @todo: extract LogStream from `log` namespace. To make code more readable, since log::LogStream names logging twice.
using LogStream = log::LogStream;

/// \brief Creates a LogStream to log messages of criticality `Fatal` (highest).
/// \public
/// \thread-safe
///
/// \details Fatal shall be used on errors that cannot be recovered and will lead to an overall failure in the system.
/// Since no context id is provided, the default configured context id will be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogFatal() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Error` (2nd highest).
/// \public
/// \thread-safe
///
/// \details Error shall be used on errors that can be recovered and would lead to an failure in the system.
/// Since no context id is provided, the default configured context id will be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogError() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Warn` (3rd highest).
/// \public
/// \thread-safe
///
/// \details Warnings shall be used on errors that might be no error but desired state or could to an error later on.
/// Since no context id is provided, the default configured context id will be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogWarn() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Info` (4th highest).
/// \public
/// \thread-safe
///
/// \details Infos shall be used on messages that are of interest to analyze issues and understand overall program flow.
/// Since no context id is provided, the default configured context id will be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogInfo() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Debug` (5th highest).
/// \public
/// \thread-safe
///
/// \details Debug shall be used on messages that are of interest to analyze issues in depth.
/// Since no context id is provided, the default configured context id will be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogDebug() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Verbose` (lowest).
/// \public
/// \thread-safe
///
/// \details Verbose shall be used on messages that are of interest to analyze issues in depth but lead to super high
/// bandwidth (e.g. sending every millisecond). Since no context id is provided, the default configured context id will
/// be used.
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogVerbose() noexcept;

/// \brief Creates a LogStream to log messages of criticality `Fatal` (highest).
/// \public
/// \thread-safe
///
/// \details Fatal shall be used on errors that cannot be recovered and will lead to an overall failure in the system.
/// The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogFatal(const std::string_view context_id) noexcept;

/// \brief Creates a LogStream to log messages of criticality `Error` (2nd highest).
/// \public
/// \thread-safe
///
/// \details Error shall be used on errors that can be recovered and would lead to an failure in the system.
/// The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogError(const std::string_view context_id) noexcept;

/// \brief Creates a LogStream to log messages of criticality `Warn` (3rd highest).
/// \public
/// \thread-safe
///
/// \details Warnings shall be used on errors that might be no error but desired state or could to an error later on.
/// The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogWarn(const std::string_view context_id) noexcept;

/// \brief Creates a LogStream to log messages of criticality `Info` (4th highest).
/// \public
/// \thread-safe
///
/// \details Infos shall be used on messages that are of interest to analyze issues and understand overall program flow.
/// The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogInfo(const std::string_view context_id) noexcept;

/// \brief Creates a LogStream to log messages of criticality `Debug` (5th highest).
/// \public
/// \thread-safe
///
/// \details Debug shall be used on messages that are of interest to analyze issues in depth.
/// The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogDebug(const std::string_view context_id) noexcept;

/// \brief Creates a LogStream to log messages of criticality `Verbose` (lowest).
/// \public
/// \thread-safe
///
/// \details Verbose shall be used on messages that are of interest to analyze issues in depth but lead to super high
/// bandwidth (e.g. sending every millisecond). The provided context_id will be used (only first four byte).
/// \return LogStream which can be used stream verbose logging messages (will be flushed on destruction)
LogStream LogVerbose(const std::string_view context_id) noexcept;

/// \brief Can be used by the user to get the underlying Recorder (where logging messages will be stored)
///
/// \details In a normal case the user does not want to use this API. It is only exposed e.g. for testing purposes
/// \return The currently process global configured Recorder
log::Recorder& GetDefaultLogRecorder() noexcept;

/// \brief Returns the process global fallback Recorder
///
/// \details This API is primarily exposed for testing purposes and to allow early initialization of the fallback
/// recorder.
/// \return The process global fallback Recorder
log::Recorder& GetFallbackLogRecorder() noexcept;

/// \brief Set a given Recorder as the process global configured Recorder for all log messages
///
/// \details In a normal case the user does not want to use this API. It is only exposed e.g. for testing purposes
void SetLogRecorder(score::mw::log::Recorder* const recorder) noexcept;

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_LOGGING_H
