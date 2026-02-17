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
#ifndef SCORE_MW_LOG_CONFIGURATION_ICONFIGURATION_FILE_DISCOVERER_H
#define SCORE_MW_LOG_CONFIGURATION_ICONFIGURATION_FILE_DISCOVERER_H

#include <cstdint>
#include <string>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class IConfigurationFileDiscoverer
{
  public:
    IConfigurationFileDiscoverer() noexcept = default;
    IConfigurationFileDiscoverer(IConfigurationFileDiscoverer&&) noexcept = delete;
    IConfigurationFileDiscoverer(const IConfigurationFileDiscoverer&) noexcept = delete;
    IConfigurationFileDiscoverer& operator=(IConfigurationFileDiscoverer&&) noexcept = delete;
    IConfigurationFileDiscoverer& operator=(const IConfigurationFileDiscoverer&) noexcept = delete;

    virtual ~IConfigurationFileDiscoverer() noexcept;

    /// \brief Find and return the path to the global and application specific config files.
    /// \details Only existing paths are returned.
    /// Global configuration:
    /// 1. /etc/logging.json if it exists.
    ///
    /// Application configuration:
    /// 1. <cwd>/etc/logging.json
    /// 2. <cwd>/logging.json
    /// 3. <binary path>/../etc/logging.json
    virtual std::vector<std::string> FindConfigurationFiles() const noexcept = 0;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_ICONFIGURATION_FILE_DISCOVERER_H
