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
#include "test_basic.hpp"

#include <cassert>
#include <iostream>
#include <unordered_map>

#include "../../../score/json/json_parser.h"
#include "tracing.hpp"

namespace
{

    struct TestLogic
    {
        std::string json_path;
    };

    TestLogic map_to_params(const std::string &data)
    {
        using namespace score::json;

        JsonParser parser;
        auto any_res{parser.FromBuffer(data)};
        if (!any_res)
        {
            throw std::runtime_error{"Failed to parse JSON data"};
        }
        const auto &map_root{any_res.value().As<Object>().value().get().at("test_logic")};
        const auto &obj_root{map_root.As<Object>().value().get()};

        TestLogic params;

        params.json_path = obj_root.at("json_path").As<std::string>().value();

        return params;
    }

    const std::string kTargetName{"cpp_test_scenarios::basic::basic"};

} // namespace

std::string BasicScenario::name() const { return "basic"; }

void BasicScenario::run(const std::string &input) const
{

    // Print and parse parameters.
    std::cerr << input << std::endl;

    auto params{map_to_params(input)};

    using namespace score::json;

    JsonParser parser;
    auto data{parser.FromFile(params.json_path)};

    // String value extraction and tracing.
    std::string string_key = "string_key";
    std::string value = data.value().As<Object>().value().get().at(string_key).As<std::string>().value();
    TRACING_INFO(kTargetName, std::pair{string_key, value});

    // Number value extraction and tracing.
    std::string number_key = "number";
    int number_value = data.value().As<Object>().value().get().at(number_key).As<int>().value();
    TRACING_INFO(kTargetName, std::pair{number_key, number_value});
}
