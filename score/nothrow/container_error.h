/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_NOTHROW_CONTAINER_ERROR_H
#define SCORE_NOTHROW_CONTAINER_ERROR_H

#include "score/result/result.h"

#include <string_view>

namespace score::nothrow
{

/// @brief Error codes used by score::nothrow container APIs.
///
/// These codes replace exception-based failure reporting used by many standard
/// container operations (for example `std::bad_alloc`).
enum class ContainerErrorCode : score::result::ErrorCode
{
    /// Allocation failed (including capacity overflow checks).
    kOutOfMemory = 1,
};

/// @brief Error domain for `ContainerErrorCode`.
class ContainerErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (code)
        {
            case static_cast<score::result::ErrorCode>(ContainerErrorCode::kOutOfMemory):
                return "Container allocation failed";
            default:
                return "Unknown container error";
        }
    }
};

/// @brief Creates a `score::result::Error` in the container error domain.
/// @param code Container-specific error code.
/// @param user_message Optional additional context.
score::result::Error MakeError(const ContainerErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_CONTAINER_ERROR_H
