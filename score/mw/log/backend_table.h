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
#ifndef SCORE_MW_LOG_BACKEND_TABLE_H
#define SCORE_MW_LOG_BACKEND_TABLE_H

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/log_mode.h"
#include "score/mw/log/recorder.h"

#include <score/memory.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace score
{
namespace mw
{
namespace log
{

/// \brief Plain function pointer type for backend recorder creation.
using RecorderCreatorFn = std::unique_ptr<Recorder> (*)(const detail::Configuration& config,
                                                        score::cpp::pmr::memory_resource* memory_resource);

/// \brief Maximum number of supported log modes. Matches the LogMode enum.
/// kConsole=0, kFile=1, kRemote=2, kSystem=3, kCustom=4, plus one spare.
static constexpr std::size_t kMaxBackendSlots{6U};

/// \brief Maps LogMode enum value to array index. Returns kMaxBackendSlots on invalid input.
constexpr std::size_t ModeToSlotIndex(const LogMode mode) noexcept
{
    switch (mode)
    {
        case LogMode::kConsole:
            return 0U;
        case LogMode::kFile:
            return 1U;
        case LogMode::kRemote:
            return 2U;
        case LogMode::kSystem:
            return 3U;
        case LogMode::kCustom:
            return 4U;
        case LogMode::kInvalid:
            [[fallthrough]];
        default:
            return kMaxBackendSlots;
    }
}

/// \brief The global backend creator table.
///
/// \details Constant-initialized
///
/// Each slot holds either nullptr (backend not registered) or a function pointer to
/// a creator function for that LogMode.
extern std::array<RecorderCreatorFn, kMaxBackendSlots> gBackendCreators;

/// \brief Register a backend creator for a given log mode.
///
/// \pre Must be called during static initialization (before main).
/// \pre mode must not be LogMode::kInvalid.
inline void RegisterBackend(const LogMode mode, RecorderCreatorFn creator) noexcept
{
    const auto index = ModeToSlotIndex(mode);
    if (index < kMaxBackendSlots)
    {
        gBackendCreators[index] = creator;
    }
}

/// \brief Query whether a backend for the given mode is registered.
inline bool IsBackendAvailable(const LogMode mode) noexcept
{
    const auto index = ModeToSlotIndex(mode);
    return (index < kMaxBackendSlots) && (gBackendCreators[index] != nullptr);
}

/// \brief Create a recorder using the registered backend for the given mode.
/// \return A Recorder instance, or nullptr if no backend is registered for this mode.
inline std::unique_ptr<Recorder> CreateRecorderForMode(const LogMode mode,
                                                       const detail::Configuration& config,
                                                       score::cpp::pmr::memory_resource* memory_resource) noexcept
{
    const auto index = ModeToSlotIndex(mode);
    if ((index < kMaxBackendSlots) && (gBackendCreators[index] != nullptr))
    {
        return gBackendCreators[index](config, memory_resource);
    }
    return nullptr;
}

/// \brief RAII helper for static-initialization-time backend registration.
///
/// \details Each backend plugin creates a namespace-scope instance of this struct.
/// The constructor calls RegisterBackend() during static initialization.
struct BackendRegistrant final
{
    BackendRegistrant(const LogMode mode, RecorderCreatorFn creator) noexcept
    {
        RegisterBackend(mode, creator);
    }
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_BACKEND_TABLE_H
