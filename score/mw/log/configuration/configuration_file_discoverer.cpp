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
#include "score/mw/log/configuration/configuration_file_discoverer.h"

#include "score/span.hpp"

#include <algorithm>
#include <string_view>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

/* KW_SUPPRESS_START:AUTOSAR.ARRAY.CSTYLE: Tolerated to have constexpr array of chars */
/* KW_SUPPRESS_START:MISRA.VAR.MIN.VIS: Constant is used only within the file*/
/* KW_SUPPRESS_START:AUTOSAR.BUILTIN_NUMERIC: Tolerated, Char is used to construct array of chars*/
static constexpr const std::string_view kGlobalConfigPath{"/etc/ecu_logging_config.json"};
static constexpr const std::string_view kLocalEtcConfigPath{"etc/logging.json"};
static constexpr const std::string_view kCwdConfigPath{"logging.json"};
static constexpr const std::string_view kEnvironmentVariableConfig{"MW_LOG_CONFIG_FILE"};
/* KW_SUPPRESS_END:AUTOSAR.BUILTIN_NUMERIC*/
/* KW_SUPPRESS_END:MISRA.VAR.MIN.VIS*/
/* KW_SUPPRESS_END:AUTOSAR.ARRAY.CSTYLE*/

}  // namespace

ConfigurationFileDiscoverer::ConfigurationFileDiscoverer(score::cpp::pmr::unique_ptr<os::Path>&& path,
                                                         score::cpp::pmr::unique_ptr<os::Stdlib>&& stdlib,
                                                         score::cpp::pmr::unique_ptr<os::Unistd>&& unistd)
    : IConfigurationFileDiscoverer{}, path_(std::move(path)), stdlib_(std::move(stdlib)), unistd_(std::move(unistd))
{
}

std::vector<std::string> ConfigurationFileDiscoverer::FindConfigurationFiles() const noexcept
{
    std::vector<std::string> existing_config_files;

    const auto global_file = GetGlobalConfigFile();
    if (global_file.has_value())
    {
        std::ignore = existing_config_files.emplace_back(global_file.value());
    }

    const auto env_file = FindEnvironmentConfigFile();
    if (env_file.has_value())
    {
        std::ignore = existing_config_files.emplace_back(env_file.value());
    }
    else
    {
        const auto local_file = FindLocalConfigFile();
        if (local_file.has_value())
        {
            std::ignore = existing_config_files.emplace_back(local_file.value());
        }
    }

    return existing_config_files;
}

score::cpp::optional<std::string_view> ConfigurationFileDiscoverer::GetGlobalConfigFile() const noexcept
{
    if (FileExists(kGlobalConfigPath))
    {
        return kGlobalConfigPath;
    }
    return {};
}

/// \brief Return true if the file with the given path exists.
/// Yes, a similar utility already exists in lib/filesystem, but we cannot use it here since lib/filesystem is using
/// logging.
bool ConfigurationFileDiscoverer::FileExists(const std::string_view& path) const noexcept
{
    return unistd_->access(path.data(), score::os::Unistd::AccessMode::kExists).has_value();
}

score::cpp::optional<std::string> ConfigurationFileDiscoverer::FindLocalConfigFile() const noexcept
{
    const std::vector<score::cpp::optional<std::string>> candidates{
        {GetConfigFileByExecutableLocation(), kLocalEtcConfigPath, kCwdConfigPath}};

    const auto result = std::find_if(candidates.cbegin(), candidates.cend(), [this](const auto& candidate) {
        return (candidate.has_value() == true) && (FileExists(candidate.value()) == true);
    });

    /* KW_SUPPRESS_START:UNINIT.STACK.MUST:False positive: candidates data is initialised. */
    if (result != candidates.cend())
    {
        return *result;
    }

    return {};
    /* KW_SUPPRESS_END:UNINIT.STACK.MUST */
}

/* KW_SUPPRESS_START:MISRA.STDLIB.ABORT: Used genenv from Stdlib-wrapper */
score::cpp::optional<std::string> ConfigurationFileDiscoverer::FindEnvironmentConfigFile() const noexcept
{
    const auto environmental_config_path = stdlib_->getenv(kEnvironmentVariableConfig.data());
    if (environmental_config_path != nullptr)
    {
        if (FileExists(environmental_config_path) == true)
        {
            return environmental_config_path;
        }
    }

    return {};
}
/* KW_SUPPRESS_END:MISRA.STDLIB.ABORT: Used genenv from Stdlib-wrapper */

score::cpp::optional<std::string> ConfigurationFileDiscoverer::GetConfigFileByExecutableLocation() const noexcept
{
    const auto exec_path = path_->get_exec_path();
    if (exec_path.has_value())
    {
        const auto bin_path = path_->get_parent_dir(exec_path.value());
        const auto app_path = path_->get_parent_dir(bin_path);
        std::string result{app_path + "/"};
        result += kLocalEtcConfigPath;
        return result;
    }
    return {};
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
