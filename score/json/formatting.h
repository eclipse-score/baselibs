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

#ifndef SCORE_LIB_JSON_FORMATTING_H
#define SCORE_LIB_JSON_FORMATTING_H

#include <cstdint>

namespace score::json
{

/// @brief Layout of the serialized JSON representation.
enum class Formatting : std::uint8_t
{
    /// @brief No insignificant whitespace is emitted.
    kCompact,
    /// @brief The output is indented and spread over multiple lines to be human readable.
    kPrettyPrint,
};

}  // namespace score::json

#endif  // SCORE_LIB_JSON_FORMATTING_H
