# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

# nlohmann JSON parser implementation
add_library(score_json_parser_nlohmann
    json_builder.cpp
    json_builder.h
    nlohmann_parser.cpp
    nlohmann_parser.h
)

target_include_directories(score_json_parser_nlohmann
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_json_parser_nlohmann
    PUBLIC
        score_compiler_flags
        score_json_internal_model
)

# Link nlohmann/json if available via CMake package
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(score_json_parser_nlohmann PUBLIC nlohmann_json::nlohmann_json)
elseif(TARGET PkgConfig::NLOHMANN_JSON)
    target_link_libraries(score_json_parser_nlohmann PUBLIC PkgConfig::NLOHMANN_JSON)
else()
    # Try to create a dummy target for header-only mode
    target_include_directories(score_json_parser_nlohmann PRIVATE /home/visdks/Projects/032_VAF_790/CMakeProject/install/include)
endif()

# Create alias
add_library(score::json_parser_nlohmann ALIAS score_json_parser_nlohmann)

# Install target
install(TARGETS score_json_parser_nlohmann
    EXPORT score-baselibsTargets
)