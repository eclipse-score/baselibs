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

#include "score/json/json_parser.h"

#include "score/json/internal/parser/nlohmann/nlohmann_parser.h"

#include <iostream>

namespace score
{
namespace json
{

auto JsonParser::FromFile(const std::string_view file_path) const noexcept -> score::Result<Any>
{
    return NlohmannParser::FromFile(file_path);
}

auto JsonParser::FromBuffer(const std::string_view buffer) const noexcept -> score::Result<Any>
{
    return NlohmannParser::FromBuffer(buffer);
}

// False positive, user defined literal operator is used to perform conversion.
// coverity[autosar_cpp14_a13_1_3_violation : FALSE]
auto operator"" _json(const char* const data, const size_t size) -> Any
{
    const JsonParser json_parser_obj;
    auto result = json_parser_obj.FromBuffer(std::string_view{data, size});
    if (!result.has_value())
    {
        // False positive, no function call.
        //  coverity[autosar_cpp14_m8_4_4_violation : FALSE]
        std::cerr << "JSON could not be parsed!" << std::endl;
        std::terminate();
    }
    return std::move(result).value();
}

}  // namespace json
}  // namespace score
