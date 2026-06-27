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
#include "score/concurrency/future/interruptible_promise.h"

score::cpp::expected_blank<score::concurrency::Error>
score::concurrency::InterruptiblePromise<void>::SetValue() noexcept
{
    constexpr auto strategy = [](InterruptibleState<void>& state) noexcept {
        return state.SetValue();
    };
    return this->SetValueInternalWithStrategy(strategy);
}
