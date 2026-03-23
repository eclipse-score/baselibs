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

#ifndef SCORE_NOTHROW_TEST_CORE_DUMP_SUPPRESSION_H
#define SCORE_NOTHROW_TEST_CORE_DUMP_SUPPRESSION_H

namespace score::nothrow
{

void DisableCoreDumpsForTests() noexcept;

}  // namespace score::nothrow

#endif  // SCORE_NOTHROW_TEST_CORE_DUMP_SUPPRESSION_H
