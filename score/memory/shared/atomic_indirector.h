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
#ifndef SCORE_LIB_MEMORY_SHARED_ATOMICINDIRECTOR_H
#define SCORE_LIB_MEMORY_SHARED_ATOMICINDIRECTOR_H

// MIGRATION SHIM: AtomicIndirectorReal/AtomicIndirectorMock moved to score/concurrency/atomic_indirector.h. This
// header is kept so that existing bazel targets, include paths ("score/memory/shared/atomic_indirector.h") and the
// score::memory::shared namespace keep working unchanged. New code should include
// "score/concurrency/atomic_indirector.h" directly and use the score::concurrency namespace.
#include "score/concurrency/atomic_indirector.h"

namespace score::memory::shared
{

using score::concurrency::AtomicIndirectorReal;
using score::concurrency::AtomicIndirectorMock;

}  // namespace score::memory::shared

#endif  // SCORE_LIB_MEMORY_SHARED_ATOMICINDIRECTOR_H
