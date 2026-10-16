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

#include "score/string_manipulation/arguments/arguments.h"
#include <score/assert.hpp>
#include <cstring>

std::vector<score::safecpp::zstring_view> score::string_manipulation::GetArguments(int argc, const char* argv[])
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION(argc >= 0);
    std::vector<safecpp::zstring_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; i++)
    {
        // We need to use strlen because zstring_view does not support char* construct
        arguments.emplace_back(argv[i], std::strlen(argv[i]));
    }
    return arguments;
}
