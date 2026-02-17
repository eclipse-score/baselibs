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
#include "score/analysis/tracing/common/flexible_circular_allocator/error_codes/factory/error_code.h"

std::string_view score::analysis::tracing::FlexibleAllocatorFactoryErrorDomain::MessageFor(
    const score::result::ErrorCode& code) const noexcept
{
    std::string_view error_message;

    switch (code)
    {
        case static_cast<score::result::ErrorCode>(FlexibleAllocatorFactoryErrorCode::kBaseAddressVoid):
            error_message = "Base address is nullptr";
            break;
        case static_cast<score::result::ErrorCode>(FlexibleAllocatorFactoryErrorCode::kSizeIsZero):
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
constexpr score::analysis::tracing::FlexibleAllocatorFactoryErrorDomain flexible_allocator_factory_error_domain;
}

score::result::Error score::analysis::tracing::MakeError(
    const score::analysis::tracing::FlexibleAllocatorFactoryErrorCode code,
    const std::string_view user_message) noexcept
{
    return {static_cast<score::result::ErrorCode>(code), flexible_allocator_factory_error_domain, user_message};
}
