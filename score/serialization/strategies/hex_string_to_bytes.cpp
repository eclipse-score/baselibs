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

#include "score/serialization/strategies/hex_string_to_bytes.h"

#include <score/assert.hpp>

#include <cctype>
#include <iterator>

namespace score
{
namespace serialization
{

score::cpp::pmr::vector<std::uint8_t> HexStringToBytes(const std::string_view hex,
                                                       score::cpp::pmr::memory_resource* mem_res)
{
    score::cpp::pmr::vector<std::uint8_t> bytes{mem_res};
    bytes.reserve(hex.size() / 2U);
    SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(((hex.size() % 2U) == 0U),
                                                "incorrect number of hex digits (please specify upper+lower nibbles)");

    auto iterator = hex.cbegin();
    while (iterator != hex.cend())
    {
        const auto first_char = *iterator;
        iterator = std::next(iterator);
        // safe to access without out of bounds check due to above assertion
        const auto second_char = *iterator;
        iterator = std::next(iterator);

        const std::array<std::string_view::value_type, 3> byte_string{{first_char, second_char, '\0'}};

        // Suppress "AUTOSAR C++14 M5-0-3" rule findings. This rule states: "A cvalue expression shall
        // not be implicitly converted to a different underlying type". False positive. Violation shown on empty line.
        // Suppress "AUTOSAR C++14 M5-0-11" rule findings. This rule states: "The plain char type shall
        // only be used for the storage and use of character values. False positive. Violation shown on empty line.
        // Suppress "AUTOSAR C++14 A21-8-1" rule findings. This rule states: "Arguments to character-handling functions
        // shall be representable as an unsigned char." This is suppressed because the input values (first_char and
        // second_char) are guaranteed to be valid ASCII characters and fall within the range of unsigned char (0-255).
        // coverity[autosar_cpp14_a21_8_1_violation]
        // coverity[autosar_cpp14_m5_0_11_violation : FALSE]
        // coverity[autosar_cpp14_m5_0_3_violation : FALSE]
        SCORE_LANGUAGE_FUTURECPP_ASSERT_PRD_MESSAGE(
            (static_cast<bool>(std::isxdigit(first_char)) && static_cast<bool>(std::isxdigit(second_char))),
            " expected characters to be hex digits");

        // Suppress"AUTOSAR C++14 A4-7-1" rule finding. This rule states: "An integer expression shall not lead to data
        // loss." In this context, there is no data loss because the assert above ensures the value is hex.
        // coverity[autosar_cpp14_a4_7_1_violation]
        const auto byte = static_cast<std::uint8_t>(std::stoul(byte_string.data(), nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

}  // namespace serialization
}  // namespace score
