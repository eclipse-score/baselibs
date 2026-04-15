/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_MW_LOG_DETAIL_REGISTRY_AWARE_RECORDER_FACTORY_H
#define SCORE_MW_LOG_DETAIL_REGISTRY_AWARE_RECORDER_FACTORY_H

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/configuration/itarget_config_reader.h"
#include "score/mw/log/detail/irecorder_factory.h"
#include "score/mw/log/log_mode.h"
#include "score/mw/log/recorder.h"

#include <score/memory.hpp>

#include <memory>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief IRecorderFactory implementation that queries the global backend table for available backends.
///
/// \details This is the single factory implementation linked into every binary. It:
/// - Reads configuration files to determine which LogModes are requested.
/// - Queries IsBackendAvailable() for each requested mode.
/// - Creates recorders for available modes.
/// - Falls back to console (LogMode::kConsole) for unavailable modes.
/// - Wraps multiple recorders in CompositeRecorder if needed.
class RegistryAwareRecorderFactory final : public IRecorderFactory
{
  public:
    RegistryAwareRecorderFactory() noexcept = default;

    std::unique_ptr<Recorder> CreateFromConfiguration(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;

    std::unique_ptr<Recorder> CreateFromConfiguration(const std::unique_ptr<const ITargetConfigReader> config_reader,
                                                      score::cpp::pmr::memory_resource* memory_resource) const noexcept;

    std::unique_ptr<Recorder> CreateWithConsoleLoggingOnly(
        score::cpp::pmr::memory_resource* memory_resource) const noexcept override;

    std::unique_ptr<Recorder> CreateStub() const noexcept override;

    /// \brief Create a recorder for a specific log mode using the backend table.
    ///
    /// \details Delegates to CreateRecorderForMode() from backend_table.h.
    /// Returns an EmptyRecorder stub if no backend is registered for the requested mode.
    std::unique_ptr<Recorder> CreateRecorderFromLogMode(
        const LogMode& log_mode,
        const Configuration& config,
        score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource()) const noexcept;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_REGISTRY_AWARE_RECORDER_FACTORY_H
