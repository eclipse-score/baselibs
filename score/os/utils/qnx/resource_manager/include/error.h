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
#ifndef SCORE_LIB_OS_UTILS_QNX_RESOURCE_MANAGER_INCLUDE_ERROR_H
#define SCORE_LIB_OS_UTILS_QNX_RESOURCE_MANAGER_INCLUDE_ERROR_H

#include "score/result/error_code.h"
#include "score/result/error_domain.h"

#include "score/result/error.h"

#include <string.h>

namespace score
{
namespace os
{

enum class ErrorCode : score::result::ErrorCode
{
    kIllegalSeek = ESPIPE,
    kInvalidArgument = EINVAL,
    kUnsupportedFunction = ENOSYS,
};

score::result::Error MakeError(const ErrorCode code, const std::string_view user_message = "") noexcept;

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_UTILS_QNX_RESOURCE_MANAGER_INCLUDE_ERROR_H
