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

#include "score/json/json_literals.h"

#include "score/json/json_parser.h"

#include <exception>
#include <iostream>
#include <string_view>
#include <utility>

namespace score::json::literals
{

auto operator""_json(const char* const data, const std::size_t size) -> Any
{
    auto result{JsonParser{}.FromBuffer(std::string_view{data, size})};
    if (!result.has_value())
    {
        std::cerr << "JSON could not be parsed: " << result.error() << '\n';
        std::terminate();
    }
    return std::move(result).value();
}

}  // namespace score::json::literals
