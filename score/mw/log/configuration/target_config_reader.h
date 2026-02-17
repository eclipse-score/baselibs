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
#ifndef SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_H
#define SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_H

#include "score/os/unistd.h"
#include "score/os/utils/path_impl.h"
#include "score/mw/log/configuration/configuration_file_discoverer.h"
#include "score/mw/log/configuration/itarget_config_reader.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class TargetConfigReader final : public ITargetConfigReader
{
  public:
    explicit TargetConfigReader(std::unique_ptr<IConfigurationFileDiscoverer> discoverer) noexcept;

    score::Result<Configuration> ReadConfig() const noexcept override;

  private:
    std::unique_ptr<IConfigurationFileDiscoverer> discoverer_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_H
