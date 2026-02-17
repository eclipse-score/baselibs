/********************************************************************************
 * Copyright (c) 2021 Contributors to the Eclipse Foundation
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

///
/// \file
/// \copyright Copyright (c) 2021 Contributors to the Eclipse Foundation
///
/// \brief Score.Futurecpp.Charconv component
///

#ifndef SCORE_LANGUAGE_FUTURECPP_CHARCONV_HPP
#define SCORE_LANGUAGE_FUTURECPP_CHARCONV_HPP

#include <cstdint>
#include <system_error>

namespace score::cpp
{

/// Result type of std::to_chars
struct to_chars_result
{
    char* ptr;
    std::errc ec;
};

/// \{
/// \brief Converts value into a character string by successively filling the range [first, last), where [first, last)
/// is required to be a valid range.
///
/// \pre Only base = 16 for hex conversion is supported.
/// \returns Returns an lowercase string of where the characters correspond to the bytes of value in big-endian order.
/// E.g. to_chars(first, last, 0xcafe, 16) returns "cafe"
to_chars_result to_chars(char* first, char* last, std::int8_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::uint8_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::int16_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::uint16_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::int32_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::uint32_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::int64_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, std::uint64_t value, int base = 10);
to_chars_result to_chars(char* first, char* last, bool value, int base = 10) = delete;
/// \}

} // namespace score::cpp

#endif // SCORE_LANGUAGE_FUTURECPP_CHARCONV_HPP
