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

#include "score/json/internal/parser/nlohmann/nlohmann_parser.h"
#include "score/json/internal/parser/nlohmann/json_builder.h"

#include "nlohmann/json.hpp"
#include "score/json/internal/model/any.h"
#include "score/json/internal/model/error.h"
#include "score/json/internal/model/object.h"
#include "score/result/result.h"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>

auto score::json::NlohmannParser::FromFile(const std::string_view file_path) -> score::Result<score::json::Any>
{
    score::Result<score::json::Any> result = MakeUnexpected(Error::kParsingError);
    // read from file
    std::string file_name{std::string{file_path.data(), file_path.size()}};
    std::ifstream file(file_name);

    if (!file.is_open())
    {
        return MakeUnexpected(Error::kParsingError, "Failed to open file");
    }

    score::json::JsonBuilder json_builder{};
    auto JsonParserCallBack = [&json_builder](
                                  int, nlohmann::json::parse_event_t event, nlohmann::json& parsed) -> bool {
        return json_builder.HandleEvent(event, parsed);
    };

    bool throw_exception{false};
    auto json_data = nlohmann::json::parse(file, JsonParserCallBack, throw_exception);
    if (json_data.is_discarded())
    {
        return MakeUnexpected(Error::kParsingError, "Invalid json encountered");
    }

    return json_builder.GetData();
}

auto score::json::NlohmannParser::FromBuffer(const std::string_view buffer) -> score::Result<score::json::Any>
{
    score::json::JsonBuilder json_builder{};
    auto JsonParserCallBack = [&json_builder](
                                  int, nlohmann::json::parse_event_t event, nlohmann::json& parsed) -> bool {
        return json_builder.HandleEvent(event, parsed);
    };

    auto json_data = nlohmann::json::parse(buffer, JsonParserCallBack, false);
    if (json_data.is_discarded())
    {
        return MakeUnexpected(Error::kParsingError, "Invalid json encountered");
    }

    return json_builder.GetData();
}
