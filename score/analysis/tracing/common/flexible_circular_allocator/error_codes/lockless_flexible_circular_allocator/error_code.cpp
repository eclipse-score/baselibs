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
#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/lockless_flexible_circular_allocator/error_code.h"

std::string_view score::analysis::tracing::LocklessFlexibleAllocatorErrorDomain::MessageFor(
    const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_message;

    switch (code)
    {
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kCorruptedBufferBlock):
            error_message = "Corrupted buffer block detected, integrity check failed";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kInvalidListEntryOffset):
            error_message = "Invalid list entry offset, out of bounds access prevented";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kOverFlowOccurred):
            error_message = "Overflow occurred during allocation";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kNotEnoughMemory):
            error_message = "Not enough memory available for allocation request";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kViolatedMaximumRetries):
            error_message = "Violated maximum retries";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kInvalidDeallocationAddress):
            error_message = "Invalid deallocation address provided";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kInvalidOffsetValue):
            error_message = "Invalid offset value detected";
            break;
        case static_cast<score::result::ErrorCode>(LocklessFlexibleAllocatorErrorCode::kAddressNotAligned):
            error_message = "Address is not properly aligned";
            break;
        default:
            error_message = "Unknown error";
            break;
    }
    return error_message;
}

namespace
{
constexpr score::analysis::tracing::LocklessFlexibleAllocatorErrorDomain lockless_flexible_allocator_error_domain;
}

score::result::Error score::analysis::tracing::MakeError(
    const score::analysis::tracing::LocklessFlexibleAllocatorErrorCode code,
    const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), lockless_flexible_allocator_error_domain, user_message};
}
