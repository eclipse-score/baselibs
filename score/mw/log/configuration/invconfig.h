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
#ifndef SCORE_MW_LOG_INVCONFIG_H_
#define SCORE_MW_LOG_INVCONFIG_H_

#include "nvmsgdescriptor.h"

namespace score
{
namespace mw
{
namespace log
{

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class INvConfig
{
  public:
    virtual ~INvConfig() noexcept;

    enum class ReadResult : std::uint8_t
    {
        kOK = 0,
        kERROR_PARSE,
        kERROR_CONTENT
    };

    // Pure virtual functions
    virtual const config::NvMsgDescriptor* getDltMsgDesc(const std::string& typeName) const noexcept = 0;

  protected:
    INvConfig() noexcept = default;
    INvConfig(INvConfig&&) noexcept = default;
    INvConfig(const INvConfig&) noexcept = default;
    INvConfig& operator=(INvConfig&&) noexcept = default;
    INvConfig& operator=(const INvConfig&) noexcept = default;
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_NVCONFIG_H_
