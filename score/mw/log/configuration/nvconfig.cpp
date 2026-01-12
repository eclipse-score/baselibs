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
#include "nvconfig.h"

#include "score/json/json_parser.h"

#include <string_view>

namespace score
{
namespace mw
{
namespace log
{

NvConfig::NvConfig(std::unordered_map<std::string, config::NvMsgDescriptor> map) : INvConfig(), typemap_{std::move(map)}
{
}

const config::NvMsgDescriptor* NvConfig::getDltMsgDesc(const std::string& typeName) const noexcept
{
    auto desc =
        typemap_.find(typeName);  //  Future C++20 optimiazation by directly using std::string_view as find() argument

    if (desc != typemap_.end())
    {
        return &desc->second;
    }
    else
    {
        return nullptr;
    }
}

}  // namespace log
}  // namespace mw
}  // namespace score
