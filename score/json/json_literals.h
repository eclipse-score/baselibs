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

#ifndef SCORE_JSON_JSON_LITERALS_H
#define SCORE_JSON_JSON_LITERALS_H

#include "score/json/any.h"

#include <cstddef>

namespace score::json::literals
{

/// @brief Parses a JSON string literal into a tree of JSON data, for building test fixtures.
/// @return root to the tree of JSON data; on a parsing error, prints the error to std::cerr and terminates
Any operator""_json(const char* const data, const std::size_t size);

}  // namespace score::json::literals

#endif  // SCORE_JSON_JSON_LITERALS_H
