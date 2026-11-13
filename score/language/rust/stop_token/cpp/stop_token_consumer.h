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

#ifndef SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_CONSUMER_H
#define SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_CONSUMER_H

#include "score/stop_token.hpp"

namespace score::language::rust::stop_token::test
{

/// @brief Returns whether stop has been requested through a borrowed token.
bool BorrowedStopTokenStopRequested(const score::cpp::stop_token& token) noexcept;

}  // namespace score::language::rust::stop_token::test

#endif  // SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_CONSUMER_H
