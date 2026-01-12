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
#include "score/os/sigevent_error.h"

#include <string_view>

namespace score::os
{

std::string_view SigEventErrorCodeDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{
    std::string_view message;

    switch (code)
    {
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidArgument):
        {
            message = "Invalid argument";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidNotificationType):
        {
            message = "Invalid notification type";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidThreadAttributesNotificationType):
        {
            message = "Invalid notification type for the thread attributes";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidThreadCallbackNotificationType):
        {
            message = "Invalid notification type for the thread callback";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventNotificationType):
        {
            message = "Invalid signal event notification type";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalEventValue):
        {
            message = "Invalid signal event value";
            break;
        }
        case static_cast<score::result::ErrorCode>(SigEventErrorCode::kInvalidSignalNumber):
        {
            message = "Invalid signal number";
            break;
        }
        default:
        {
            message = "Unknown error";
            break;
        }
    }
    return message;
}

namespace
{
constexpr SigEventErrorCodeDomain sig_event_error_code_domain;
}

result::Error MakeError(const SigEventErrorCode code, const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), sig_event_error_code_domain, user_message};
}

}  // namespace score::os
