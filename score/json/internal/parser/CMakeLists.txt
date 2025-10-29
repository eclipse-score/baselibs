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

# Add subdirectories
add_subdirectory(nlohmann)
add_subdirectory(vajson)

# Parser test suite library (only built when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score_json_parser_test_suite
        parsers_test_suite.cpp
        parsers_test_suite.h
        number_parser_test_suite.cpp
        number_parser_test_suite.h
    )

    target_include_directories(score_json_parser_test_suite
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_json_parser_test_suite
        PUBLIC
            score_compiler_flags
            score_language_futurecpp
            score_result
            GTest::gtest
            # Will be linked to by the main JSON library
    )
endif()

# Configuration option for JSON parser backend
option(SCORE_USE_NLOHMANN_JSON "Use nlohmann/json as JSON parser backend" ON)

# Conditional parser selection - alias to the chosen implementation
if(SCORE_USE_NLOHMANN_JSON)
    add_library(score_json_parser ALIAS score_json_parser_nlohmann)
    message(STATUS "Using nlohmann/json as JSON parser backend")
else()
    add_library(score_json_parser ALIAS score_json_parser_vajson)
    message(STATUS "Using vajson as JSON parser backend")
endif()

# Install targets
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    install(TARGETS score_json_parser_test_suite
        EXPORT score-baselibsTargets
    )
endif()