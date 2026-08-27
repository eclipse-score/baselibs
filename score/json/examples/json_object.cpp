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

#include "score/mw/log/logging.h"
#include "score/os/unistd.h"

int main()
{
    // Change directory so that score::mw::log will find logging.json for console logging.
    if (!score::os::Unistd::instance().chdir("score/json/examples").has_value())
    {
        std::terminate();
    }

    // Load and parse the JSON document from a file path.
    const score::json::JsonParser json_parser_obj;
    const auto root = json_parser_obj.FromFile("example.json");
    if (!root.has_value())
    {
        score::mw::log::LogError() << "Failed to load json: " << root.error();
        return -1;
    }

    // Access the root element of the document as an object/dictionary type.
    const auto& obj_result = root.value().As<score::json::Object>();
    if (!obj_result.has_value())
    {
        score::mw::log::LogError() << "Error: root element is not an object.";
        return -1;
    }
    const auto& obj = obj_result.value().get();

    // Iterating over a dictionary elements with key and value
    for (const auto& element : obj)
    {
        if (element.second.As<std::string>().has_value())
        {
            score::mw::log::LogInfo() << element.first.GetAsStringView().data()                   // key
                                      << ": " << element.second.As<std::string>().value().get();  // value
        }
    }

    // JSON objects be used just like an std::unordered_map.
    const auto foo = obj.find("key");
    if (foo == obj.end())
    {
        score::mw::log::LogInfo() << "Failed to find 'key' in object.";
        return -1;
    }
    const auto foo_value = foo->second.As<bool>();
    if (foo_value.has_value())
    {
        score::mw::log::LogInfo() << "key: " << foo_value.value();
    }

    return 0;
}
