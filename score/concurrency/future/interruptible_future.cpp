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
#include "score/concurrency/future/interruptible_future.h"
#include "score/concurrency/future/interruptible_shared_future.h"

score::concurrency::InterruptibleFuture<void>::InterruptibleFuture(
    std::shared_ptr<InterruptibleState<void>> state_ptr) noexcept
    : score::concurrency::detail::BaseInterruptibleFuture<void>{std::move(state_ptr)}
{
}

score::Result<void> score::concurrency::InterruptibleFuture<void>::Get(const score::cpp::stop_token& stop_token) noexcept
{
    score::cpp::expected_blank<Error> return_code = this->Wait(stop_token);
    if (!return_code.has_value())
    {
        return MakeUnexpected(return_code.error());
    }

    const score::Result<void> value = this->StealState()->GetValue();
    return value;
}

score::concurrency::InterruptibleSharedFuture<void> score::concurrency::InterruptibleFuture<void>::Share() noexcept
{
    const InterruptibleSharedFuture<void> shared_future{std::move(*this)};
    return shared_future;
}
