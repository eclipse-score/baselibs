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

# JSON serialize library
add_library(score_json_writer_serialize
    json_serialize.cpp
    json_serialize.h
)

target_include_directories(score_json_writer_serialize
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_json_writer_serialize
    PUBLIC
        score_compiler_flags
        score_json_internal_model
        score_result
)

# Create alias
add_library(score::json_writer_serialize ALIAS score_json_writer_serialize)

# Install target
install(TARGETS score_json_writer_serialize
    EXPORT score-baselibsTargets
)