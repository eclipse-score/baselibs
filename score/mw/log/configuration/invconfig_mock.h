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
#ifndef SCORE_MW_LOG_CONFIGURATION_INVCONFIG_MOCK_H
#define SCORE_MW_LOG_CONFIGURATION_INVCONFIG_MOCK_H

#include "score/mw/log/configuration/invconfig.h"

#include "gmock/gmock.h"

namespace score
{
namespace mw
{
namespace log
{

class INvConfigMock final : public INvConfig
{
  public:
    MOCK_METHOD(const config::NvMsgDescriptor*,
                GetDltMsgDesc,
                (const std::string& type_name),
                (const, noexcept, override));
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_INVCONFIG_MOCK_H
