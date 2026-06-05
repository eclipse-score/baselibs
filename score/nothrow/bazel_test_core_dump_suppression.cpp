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

#include "score/nothrow/test_core_dump_suppression.h"

#if defined(__linux__)
#include <sys/prctl.h>
#endif
#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

namespace score::nothrow
{

void DisableCoreDumpsForTests() noexcept
{
#if defined(__unix__) || defined(__APPLE__)
    rlimit core_limit{};
    core_limit.rlim_cur = 0U;
    core_limit.rlim_max = 0U;

    // If setting the limit fails, continue running tests unchanged.
    (void)setrlimit(RLIMIT_CORE, &core_limit);
#endif

#if defined(__linux__)
    // On Linux, also mark the process as non-dumpable.
    (void)prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);
#endif
}

}  // namespace score::nothrow

namespace
{

class CoreDumpSuppressionInitializer
{
  public:
    CoreDumpSuppressionInitializer() noexcept
    {
        score::nothrow::DisableCoreDumpsForTests();
    }
};

const CoreDumpSuppressionInitializer kCoreDumpSuppressionInitializer{};

}  // namespace
