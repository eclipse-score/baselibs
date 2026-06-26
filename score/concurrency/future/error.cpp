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
#include "score/concurrency/future/error.h"

std::string_view score::concurrency::FutureErrorDomain::MessageFor(const score::result::ErrorCode& code) const noexcept
{
    // Suppress "AUTOSAR C++14 A7-2-1" rule finding: "An expression with enum underlying type shall only have values
    // corresponding to the enumerators of the enumeration.".
    // The value of "code" can be converted back and forth because the underlying type of "score::concurrency::Error" is
    // "score::result::ErrorCode" (aka std::int32_t), and the enumeration value is guaranteed to be in the range of
    // "score::concurrency::Error" unless ErrorCode was created by the corresponding MakeError() function.
    // coverity[autosar_cpp14_a7_2_1_violation]
    // coverity[autosar_cpp14_m6_4_3_violation]
    switch (static_cast<score::concurrency::Error>(code))
    {
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kPromiseBroken:
            return "Promise broken";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kFutureAlreadyRetrieved:
            return "Future already retrieved";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kPromiseAlreadySatisfied:
            return "Promise already satisfied";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kNoState:
            return "No shared state associated";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kStopRequested:
            return "Stop requested";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kTimeout:
            return "Timeout";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kUnset:
            return "Value was not set";
        // coverity[autosar_cpp14_m6_4_5_violation]
        case Error::kUnknown:
        default:
            return "Unknown error";
    }
}

namespace
{
constexpr score::concurrency::FutureErrorDomain future_error_domain;
}

score::result::Error score::concurrency::MakeError(const score::concurrency::Error code,
                                                   const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), future_error_domain, user_message};
}
