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

#ifndef SCORE_MW_LOG_PLUGIN_API_H
#define SCORE_MW_LOG_PLUGIN_API_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief ABI version for the mw::log plugin interface.
///
/// \details A major.minor scheme for versioning.:
/// Major: Incremented on binary incompatible changes.
/// Minor: Incremented on backward-compatible additions (e.g., non-breaking extensions).
/// A plugin is accepted if its major matches and its minor is less than or equal to the host's
/// minor version.
typedef struct
{
    uint16_t major;
    uint16_t minor;
} MwLogAbiVersion;

/// \brief Current ABI major version. Increment on breaking changes.
#define MW_LOG_ABI_VERSION_MAJOR 1U

/// \brief Current ABI minor version. Increment on backward-compatible additions.
#define MW_LOG_ABI_VERSION_MINOR 0U

/// \brief Checks whether a plugin version is compatible with the host.
/// \public
/// \thread-safe
///
/// \param plugin_version
/// \return Non-zero if the plugin is compatible, zero otherwise.
static inline int MwLogIsVersionCompatible(MwLogAbiVersion plugin_version)
{
    return (plugin_version.major == MW_LOG_ABI_VERSION_MAJOR) && (plugin_version.minor <= MW_LOG_ABI_VERSION_MINOR);
}

/// \brief Returns the ABI version this plugin was built against.
/// \public
/// \thread-safe
///
/// \details Each backend .so exports this symbol.
MwLogAbiVersion MwLogVersion(void);

/// \brief Registers the backend's recorder creator functions into the
/// global backend table.
/// \public
///
/// \details Each backend .so exports this symbol.
/// Must be called from a single thread during initialization.
void MwLogRegisterBackends(void) noexcept;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SCORE_MW_LOG_PLUGIN_API_H
