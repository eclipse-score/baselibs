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

#ifndef SCORE_JSON_INTERNAL_MODEL_ERROR_H
#define SCORE_JSON_INTERNAL_MODEL_ERROR_H

// Deprecated: include "score/json/error.h" instead.
// This forwarding header will be removed in a future breaking release.
#include "score/json/error.h"

namespace score::json::detail
{

// Referencing the deprecated alias emits a warning in every translation unit that includes this header.
using DeprecatedInternalModelErrorHeader
    [[deprecated("score/json/internal/model/error.h is deprecated, include score/json/error.h instead")]] = int;
static_assert(sizeof(DeprecatedInternalModelErrorHeader) > 0, "");

}  // namespace score::json::detail

#endif  // SCORE_JSON_INTERNAL_MODEL_ERROR_H
