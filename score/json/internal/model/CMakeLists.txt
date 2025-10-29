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

# JSON internal model library
add_library(score_json_internal_model
    any.cpp
    any.h
    error.cpp
    error.h
    lossless_cast.cpp
    lossless_cast.h
    null.cpp
    null.h
    number.cpp
    number.h
    object.h
)

target_include_directories(score_json_internal_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_json_internal_model
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
        score_memory_string_comparison_adaptor
        score_result
)

# Create alias
add_library(score::json_internal_model ALIAS score_json_internal_model)

# Install target
install(TARGETS score_json_internal_model
    EXPORT score-baselibsTargets
)

# Install all headers recursively with directory exclusions
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/json/internal/model
    FILES_MATCHING 
    PATTERN "*.h"
)
