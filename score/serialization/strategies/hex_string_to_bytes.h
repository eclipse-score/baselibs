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

#ifndef SCORE_SERIALIZATION_STRATEGIES_HEX_STRING_TO_BYTES_H
#define SCORE_SERIALIZATION_STRATEGIES_HEX_STRING_TO_BYTES_H

#include <score/memory_resource.hpp>
#include <score/vector.hpp>
#include <cstdint>
#include <string>
#include <string_view>

namespace score
{
namespace serialization
{

/// @brief Change a string of hex digits into bytes (vector of uint8_t).
/// Must be a string of bytes (2 nibbles!), will assert if the argument is ill-formed.
/// @param hex string of hex digits, e.g. "BEEF".
/// @return a vector of bytes.
score::cpp::pmr::vector<std::uint8_t> HexStringToBytes(
    const std::string_view hex,
    score::cpp::pmr::memory_resource* mem_res = score::cpp::pmr::get_default_resource());

}  // namespace serialization
}  // namespace score

#endif  // SCORE_SERIALIZATION_STRATEGIES_HEX_STRING_TO_BYTES_H
