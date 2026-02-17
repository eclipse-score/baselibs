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

#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/flexible_circular_allocator/error_code.h"

std::string_view score::analysis::tracing::FlexibleAllocatorErrorDomain::MessageFor(
    const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_message;

    switch (code)
    {
        case static_cast<score::result::ErrorCode>(FlexibleAllocatorErrorCode::kNotEnoughMemory):
            error_message = "Not enough memory available for allocation request";
            break;
        case static_cast<score::result::ErrorCode>(FlexibleAllocatorErrorCode::kInvalidDeallocationAddress):
            error_message = "Invalid deallocation address provided";
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
