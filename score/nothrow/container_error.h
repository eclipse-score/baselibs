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

enum class ContainerErrorCode : score::result::ErrorCode
{
    kOutOfMemory = 1,
    kOutOfRange,
};

class ContainerErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override
    {
        switch (code)
        {
            case static_cast<score::result::ErrorCode>(ContainerErrorCode::kOutOfMemory):
                return "Container allocation failed";
            case static_cast<score::result::ErrorCode>(ContainerErrorCode::kOutOfRange):
                return "Container index out of range";
            default:
                return "Unknown container error";
        }
    }
};

score::result::Error MakeError(const ContainerErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_CONTAINER_ERROR_H
