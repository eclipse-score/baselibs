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
#ifndef SCORE_LIB_OS_SIGEVENT_ERROR_H
#define SCORE_LIB_OS_SIGEVENT_ERROR_H

#include "score/result/error.h"

namespace score::os
{

enum class SigEventErrorCode : result::ErrorCode
{
    kInvalidArgument,
    kInvalidNotificationType,
    kInvalidThreadAttributesNotificationType,
    kInvalidThreadCallbackNotificationType,
    kInvalidSignalEventNotificationType,
    kInvalidSignalEventValue,
    kInvalidSignalNumber,
};

class SigEventErrorCodeDomain final : public result::ErrorDomain
{
  public:
    std::string_view MessageFor(const result::ErrorCode& code) const noexcept override;
};

result::Error MakeError(const SigEventErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace score::os

#endif  // SCORE_LIB_OS_SIGEVENT_ERROR_H
