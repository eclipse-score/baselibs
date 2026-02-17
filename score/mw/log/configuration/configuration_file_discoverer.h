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
#ifndef SCORE_MW_LOG_CONFIGURATION_CONFIGURATION_FILE_DISCOVERER_H
#define SCORE_MW_LOG_CONFIGURATION_CONFIGURATION_FILE_DISCOVERER_H

#include "score/language/safecpp/string_view/zstring_view.h"
#include "score/mw/log/configuration/iconfiguration_file_discoverer.h"

#include <memory>

#include "score/optional.hpp"

#include "score/os/stdlib.h"
#include "score/os/unistd.h"
#include "score/os/utils/path.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class ConfigurationFileDiscoverer final : public IConfigurationFileDiscoverer
{
  public:
    ConfigurationFileDiscoverer(score::cpp::pmr::unique_ptr<os::Path>&& path,
                                score::cpp::pmr::unique_ptr<os::Stdlib>&& stdlib,
                                score::cpp::pmr::unique_ptr<os::Unistd>&& unistd);

    std::vector<std::string> FindConfigurationFiles() const noexcept override;

  private:
    // std::string_view as return is intended here on purpose since it points to a constexpr global variable.
    // It's better here for optimization purpose.
    score::cpp::optional<safecpp::zstring_view> GetGlobalConfigFile() const noexcept;
    bool FileExists(const safecpp::zstring_view& path) const noexcept;
    score::cpp::optional<std::string> FindLocalConfigFile() const noexcept;
    score::cpp::optional<std::string> FindEnvironmentConfigFile() const noexcept;
    score::cpp::optional<std::string> GetConfigFileByExecutableLocation() const noexcept;

    score::cpp::pmr::unique_ptr<os::Path> path_;
    score::cpp::pmr::unique_ptr<os::Stdlib> stdlib_;
    score::cpp::pmr::unique_ptr<os::Unistd> unistd_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_CONFIGURATION_FILE_DISCOVERER_H
