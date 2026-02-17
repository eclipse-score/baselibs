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
#ifndef SCORE_MW_LOG_CONFIGURATION_NVCONFIG_H
#define SCORE_MW_LOG_CONFIGURATION_NVCONFIG_H

#include "invconfig.h"

#include <unordered_map>

namespace score
{
namespace mw
{
namespace log
{

// coverity[autosar_cpp14_m3_2_3_violation : FALSE]
class NvConfig : public INvConfig
{
  public:
    using TypemapT = std::unordered_map<std::string, config::NvMsgDescriptor>;

    explicit NvConfig(std::unordered_map<std::string, config::NvMsgDescriptor> map);

    ~NvConfig() noexcept override = default;
    NvConfig(const NvConfig&) = default;

    NvConfig& operator=(const NvConfig& other) noexcept
    {
        if (this != &other)
        {
            // Only assign typemap_
            typemap_ = other.typemap_;
        }
        return *this;
    }

    NvConfig(NvConfig&&) noexcept = default;

    NvConfig& operator=(NvConfig&& other) noexcept
    {
        if (this != &other)
        {
            // Only move typemap_
            typemap_ = std::move(other.typemap_);
        }
        return *this;
    }

    const config::NvMsgDescriptor* GetDltMsgDesc(const std::string& type_name) const noexcept override;

  private:
    TypemapT typemap_;
};

}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_CONFIGURATION_NVCONFIG_H
