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
#ifndef SCORE_LANGUAGE_SAFECPP_SAFE_ATOMICS_ERROR_H
#define SCORE_LANGUAGE_SAFECPP_SAFE_ATOMICS_ERROR_H

#include "score/result/error.h"
#include "score/result/error_code.h"
#include "score/result/error_domain.h"

namespace score::safe_atomics
{
enum class ErrorCode : score::result::ErrorCode
{
    kUnknown = 0,  // Value for default initialization - never returned on purpose
    kUnexpectedError,
    kExceedsNumericLimits,
    kMaxRetriesReached,
};

class ErrorDomain final : public score::result::ErrorDomain
{
  public:
    std::string_view MessageFor(const score::result::ErrorCode& code) const noexcept override;
};

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace score::safe_atomics

#endif  // SCORE_LANGUAGE_SAFECPP_SAFE_ATOMICS_ERROR_H
