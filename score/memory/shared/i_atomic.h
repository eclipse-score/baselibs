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
#ifndef SCORE_LIB_MEMORY_SHARED_MOCK_IATOMIC_H
#define SCORE_LIB_MEMORY_SHARED_MOCK_IATOMIC_H

// MIGRATION SHIM: IAtomic moved to score/concurrency/i_atomic.h. This header is kept so that existing bazel
// targets, include paths ("score/memory/shared/i_atomic.h") and the score::memory::shared namespace keep working
// unchanged. New code should include "score/concurrency/i_atomic.h" directly.
#include "score/concurrency/i_atomic.h"

namespace score::memory::shared
{

using score::concurrency::IAtomic;

}  // namespace score::memory::shared

#endif  // SCORE_LIB_MEMORY_SHARED_MOCK_IATOMIC_H
