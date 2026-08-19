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
#ifndef SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_INTERPROCESS_CONDITION_VARIABLE_H
#define SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_INTERPROCESS_CONDITION_VARIABLE_H

#include "score/concurrency/condition_variable.h"

#include "score/os/utils/interprocess/interprocess_conditional_variable.h"
#include "score/os/utils/interprocess/interprocess_mutex.h"

namespace score
{
namespace concurrency
{

using InterruptibleInterprocessConditionalVariable =
    score::concurrency::InterruptibleConditionalVariableBasic<score::os::InterprocessMutex,
                                                              score::os::InterprocessConditionalVariable>;

}  // namespace concurrency
}  // namespace score

#endif  // SCORE_LIB_CONCURRENCY_INTERRUPTIBLE_INTERPROCESS_CONDITION_VARIABLE_H
