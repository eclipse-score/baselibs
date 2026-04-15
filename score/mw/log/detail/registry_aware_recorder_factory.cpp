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
#include "score/mw/log/detail/registry_aware_recorder_factory.h"

#include "score/mw/log/backend_table.h"
#include "score/mw/log/configuration/target_config_reader.h"
#include "score/mw/log/detail/composite_recorder.h"
#include "score/mw/log/detail/empty_recorder.h"
#include "score/mw/log/detail/error.h"
#include "score/mw/log/detail/initialization_reporter.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

std::unique_ptr<Recorder> RegistryAwareRecorderFactory::CreateRecorderFromLogMode(
    const LogMode& log_mode,
    const Configuration& config,
    score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(Error::kMemoryResourceError);
        return CreateStub();
    }

    auto recorder = CreateRecorderForMode(log_mode, config, memory_resource);
    if (recorder == nullptr)
    {
        ReportInitializationError(Error::kRecorderFactoryUnsupportedLogMode);
        return CreateStub();
    }

    return recorder;
}

/*
Deviation from Rule A15-5-3:
- The std::terminate() function shall not be called implicitly.
Justification:
- std::terminate() could be invoked as this function is noexcept function but result.value() may throw an exception.
- However, we only call result.value() after confirming result.has_value(), so there's no scenario where
std::bad_variant_access can occur.
- Therefore, it's safe to suppress this warning here.
*/
// coverity[autosar_cpp14_a15_5_3_violation]
std::unique_ptr<Recorder> RegistryAwareRecorderFactory::CreateFromConfiguration(
    const std::unique_ptr<const ITargetConfigReader> config_reader,
    score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(score::mw::log::detail::Error::kMemoryResourceError);
        return CreateStub();
    }
    const auto result = config_reader->ReadConfig();

    if (!result.has_value())
    {
        ReportInitializationError(result.error(), "Failed to load configuration files. Fallback to console logging.");
        return CreateWithConsoleLoggingOnly(memory_resource);
    }

    std::vector<std::unique_ptr<Recorder>> recorders;
    std::ignore = std::for_each(
        result->GetLogMode().begin(),
        result->GetLogMode().end(),
        [this, &result, &recorders, &memory_resource](const auto& log_mode) {
            // LCOV_EXCL_START : no branches to test
            std::ignore = recorders.emplace_back(CreateRecorderFromLogMode(log_mode, result.value(), memory_resource));
            // LCOV_EXCL_STOP
        });

    if (recorders.empty())
    {
        ReportInitializationError(Error::kNoLogModeSpecified);
        return std::make_unique<EmptyRecorder>();
    }

    if (recorders.size() == 1U)
    {
        return std::move(recorders[0]);
    }

    // Composite recorder is needed if there are more than one activate recorder.
    return std::make_unique<CompositeRecorder>(std::move(recorders));
}

/*
Deviation from Rule A15-5-3:
- The std::terminate() function shall not be called implicitly.
Justification:
- std::terminate() could be invoked as this function is noexcept but result.value() may throw.
- However, we only call result.value() after confirming result.has_value(), so no
  std::bad_variant_access can occur.
*/
// coverity[autosar_cpp14_a15_5_3_violation]
std::unique_ptr<Recorder> RegistryAwareRecorderFactory::CreateFromConfiguration(
    score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    if (memory_resource == nullptr)
    {
        ReportInitializationError(score::mw::log::detail::Error::kMemoryResourceError);
        return CreateWithConsoleLoggingOnly(memory_resource);
    }

    auto config_reader = std::make_unique<TargetConfigReader>(
        std::make_unique<ConfigurationFileDiscoverer>(score::os::Path::Default(memory_resource),
                                                      score::os::Stdlib::Default(memory_resource),
                                                      score::os::Unistd::Default(memory_resource)));

    const auto result = config_reader->ReadConfig();

    if (!result.has_value())
    {
        ReportInitializationError(result.error(), "Failed to load configuration files. Fallback to console logging.");
        return CreateWithConsoleLoggingOnly(memory_resource);
    }

    std::vector<std::unique_ptr<Recorder>> recorders;
    bool console_already_added = false;

    for (const auto& mode : result->GetLogMode())
    {
        if (IsBackendAvailable(mode))
        {
            auto recorder = CreateRecorderForMode(mode, result.value(), memory_resource);
            if (recorder != nullptr)
            {
                std::ignore = recorders.emplace_back(std::move(recorder));
                if (mode == LogMode::kConsole)
                {
                    console_already_added = true;
                }
            }
        }
        else
        {
            ReportInitializationError(Error::kRecorderFactoryUnsupportedLogMode,
                                      "Requested log mode not available. Falling back to console.");
            if ((!console_already_added) && IsBackendAvailable(LogMode::kConsole))
            {
                auto console = CreateRecorderForMode(LogMode::kConsole, result.value(), memory_resource);
                if (console != nullptr)
                {
                    std::ignore = recorders.emplace_back(std::move(console));
                    console_already_added = true;
                }
            }
        }
    }

    if (recorders.empty())
    {
        ReportInitializationError(Error::kNoLogModeSpecified);
        return CreateWithConsoleLoggingOnly(memory_resource);
    }

    if (recorders.size() == 1U)
    {
        return std::move(recorders[0]);
    }

    return std::make_unique<CompositeRecorder>(std::move(recorders));
}

std::unique_ptr<Recorder> RegistryAwareRecorderFactory::CreateWithConsoleLoggingOnly(
    score::cpp::pmr::memory_resource* memory_resource) const noexcept
{
    if (memory_resource == nullptr)
    {
        memory_resource = score::cpp::pmr::get_default_resource();
    }

    const Configuration config;  // default configuration
    auto recorder = CreateRecorderForMode(LogMode::kConsole, config, memory_resource);
    if (recorder != nullptr)
    {
        return recorder;
    }

    return std::make_unique<EmptyRecorder>();
}

std::unique_ptr<Recorder> RegistryAwareRecorderFactory::CreateStub() const noexcept
{
    return std::make_unique<EmptyRecorder>();
}

// The single definition of CreateRecorderFactory() — always linked via backend:minimal.
std::unique_ptr<IRecorderFactory> CreateRecorderFactory() noexcept
{
    return std::make_unique<RegistryAwareRecorderFactory>();
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
