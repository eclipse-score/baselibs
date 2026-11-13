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

#ifndef SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_ADAPTER_H
#define SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_ADAPTER_H

#include "score/stop_token.hpp"

#include <memory>

namespace score::language::rust::stop_token
{

/// @brief Returns whether stop has been requested on a stop token.
bool StopTokenStopRequested(const score::cpp::stop_token& token) noexcept;

/// @brief Returns whether stop remains possible on a stop token.
bool StopTokenStopPossible(const score::cpp::stop_token& token) noexcept;

/// @brief Returns whether two stop tokens share the same stop state.
bool StopTokenEqual(const score::cpp::stop_token& lhs, const score::cpp::stop_token& rhs) noexcept;

/// @brief Returns whether stop has been requested on a stop source.
bool StopSourceStopRequested(const score::cpp::stop_source& source) noexcept;

/// @brief Returns whether a stop source has associated stop state.
bool StopSourceStopPossible(const score::cpp::stop_source& source) noexcept;

/// @brief Requests stop through a stop source.
bool StopSourceRequestStop(const score::cpp::stop_source& source) noexcept;

/// @brief Returns a token associated with a stop source.
std::unique_ptr<score::cpp::stop_token> StopSourceGetToken(const score::cpp::stop_source& source) noexcept;

/// @brief Returns whether two stop sources share the same stop state.
bool StopSourceEqual(const score::cpp::stop_source& lhs, const score::cpp::stop_source& rhs) noexcept;

/// @brief Creates a default-constructed stop token.
std::unique_ptr<score::cpp::stop_token> MakeStopToken() noexcept;

/// @brief Creates a stop source with a new stop state.
std::unique_ptr<score::cpp::stop_source> MakeStopSource() noexcept;

}  // namespace score::language::rust::stop_token

#endif  // SCORE_LANGUAGE_RUST_STOP_TOKEN_CPP_STOP_TOKEN_ADAPTER_H
