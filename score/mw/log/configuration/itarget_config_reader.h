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
#ifndef SCORE_MW_LOG_CONFIGURATION_ITARGET_CONFIG_READER_H
#define SCORE_MW_LOG_CONFIGURATION_ITARGET_CONFIG_READER_H

#include "score/result/result.h"
#include "score/mw/log/configuration/configuration.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class ITargetConfigReader
{
  public:
    ITargetConfigReader() noexcept = default;
    ITargetConfigReader(ITargetConfigReader&&) noexcept = delete;
    ITargetConfigReader(const ITargetConfigReader&) noexcept = delete;
    ITargetConfigReader& operator=(ITargetConfigReader&&) noexcept = delete;
    ITargetConfigReader& operator=(const ITargetConfigReader&) noexcept = delete;
    virtual ~ITargetConfigReader() noexcept;

    virtual score::Result<Configuration> ReadConfig() const noexcept = 0;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_ITARGET_CONFIG_READER_H
