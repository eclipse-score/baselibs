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

    // Check if the root object contains a List under key "my_array".
    const auto my_array_iter = obj.find("my_array");
    if (my_array_iter == obj.end())
    {
        score::mw::log::LogError() << "Error: could not find my_array.";
        return -1;
    }

    // Check that my_array is in fact a JSON list/array.
    const auto my_array_result = my_array_iter->second.As<score::json::List>();
    if (!my_array_result.has_value())
    {
        score::mw::log::LogError() << "Error: my_array is not a list.";
        return -1;
    }

    // Iterate over my_array
    for (const auto& element : my_array_result.value().get())
    {
        const auto integer = element.As<std::int64_t>();
        if (integer.has_value())
        {
            score::mw::log::LogInfo() << integer.value();
        }

        const auto& message = element.As<std::string>();
        if (message.has_value())
        {
            score::mw::log::LogInfo() << message.value().get();
        }

        const auto null = element.As<score::json::Null>();
        if (null.has_value())
        {
            score::mw::log::LogInfo() << "Null";
        }

        const auto pi = element.As<float>();
        if (pi.has_value())
        {
            score::mw::log::LogInfo() << pi.value();
        }
    }

    return 0;
}
