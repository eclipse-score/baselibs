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
#include "score/analysis/tracing/common/flexible_circular_allocator/error_code.h"
// SCORE_CCM_NO_LINT No harm from  using switch_case
std::string_view score::analysis::tracing::FlexibleAllocatorErrorDomain::MessageFor(
    const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_message;  // Variable to hold the error message
    // No harm from going outside the range as that will jump to the default state in switch_case(a7_2_1),No harm to
    // define the switch in that format (m6_4_3)
    //  coverity[autosar_cpp14_a7_2_1_violation]
    //  coverity[autosar_cpp14_m6_4_3_violation]
    switch (static_cast<score::analysis::tracing::FlexibleAllocatorErrorCode>(code))
    {
        case FlexibleAllocatorErrorCode::kNoError:
            error_message = "No error";
            break;
        case FlexibleAllocatorErrorCode::kBaseAddressVoid:
            error_message = "Base address is nullptr";
            break;
        case FlexibleAllocatorErrorCode::kCorruptedBufferBlock:
            error_message = "Corrupted buffer block detected, integrity check failed";
            break;
        case FlexibleAllocatorErrorCode::kInvalidListEntryOffset:
            error_message = "Invalid list entry offset, out of bounds access prevented";
            break;
        case FlexibleAllocatorErrorCode::kSizeIsZero:
            error_message = "Size is zero";
            break;
        default:
            error_message = "Unknown error";
            break;
    }
    return error_message;
}

namespace
{
constexpr score::analysis::tracing::FlexibleAllocatorErrorDomain flexible_allocator_error_domain;
}

score::result::Error score::analysis::tracing::MakeError(const score::analysis::tracing::FlexibleAllocatorErrorCode code,
                                                     const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), flexible_allocator_error_domain, user_message};
}
