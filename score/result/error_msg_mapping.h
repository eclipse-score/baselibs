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
#ifndef SCORE_LIB_RESULT_ERROR_MSG_MAPPING_H
#define SCORE_LIB_RESULT_ERROR_MSG_MAPPING_H

#include "score/result/error.h"

#include <string_view>

/// This function can be used to get the message for an error code in a C-compatible way. It is used by the Rust Result
/// bindings.
extern "C" void LibResultErrorDomainGetMessageForErrorCode(const score::result::ErrorDomain& domain,
                                                           score::result::ErrorCode code,
                                                           std::string_view& result) noexcept;

#endif
