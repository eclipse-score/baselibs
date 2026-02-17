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
#ifndef SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_MOCK_H
#define SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_MOCK_H

#include "score/mw/log/configuration/itarget_config_reader.h"

#include "gmock/gmock.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class TargetConfigReaderMock final : public ITargetConfigReader
{
  public:
    MOCK_METHOD((score::Result<Configuration>), ReadConfig, (), (const, noexcept, override));
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_TARGET_CONFIG_READER_MOCK_H
