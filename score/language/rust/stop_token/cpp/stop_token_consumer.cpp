/*******************************************************************************
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
 *******************************************************************************/

#include "score/language/rust/stop_token/cpp/stop_token_consumer.h"

namespace score::language::rust::stop_token::test
{

bool BorrowedStopTokenStopRequested(const score::cpp::stop_token& token) noexcept
{
    return token.stop_requested();
}

}  // namespace score::language::rust::stop_token::test
