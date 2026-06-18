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
#include "score/os/utils/qnx/resource_manager/include/error.h"

#include <string_view>

namespace score
{
namespace os
{

namespace
{
class ResourceManagerErrorDomain final : public score::result::ErrorDomain
{
  public:
    /* KW_SUPPRESS_START:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    /* Compiler warn suggests override instead of final, final is already specified for the entire class */
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    /* KW_SUPPRESS_END:AUTOSAR.MEMB.VIRTUAL.FINAL: Compiler warn suggests override */
    {
        /* KW_SUPPRESS_START:AUTOSAR.SWITCH.CASECOUNT: error codes expected to be extended so switch is more suitable */
        switch (static_cast<ErrorCode>(code))
        {
            case ErrorCode::kIllegalSeek:
            case ErrorCode::kInvalidArgument:
            case ErrorCode::kUnsupportedFunction:
                return strerror(code);

            default:
                return "Unknown error.";
        }
        /* KW_SUPPRESS_END:AUTOSAR.SWITCH.CASECOUNT: error codes expected to be extended so switch is more suitable */
    }
};

constexpr ResourceManagerErrorDomain kResourceManagerErrorDomain;
}  // namespace

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), kResourceManagerErrorDomain, user_message};
}

}  // namespace os
}  // namespace score
