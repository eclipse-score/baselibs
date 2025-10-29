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

# vajson JSON parser implementation (disabled - requires AMSR libraries)
# add_library(score_json_parser_vajson
#     vajson_parser.cpp
#     vajson_parser.h
# )
# 
# target_include_directories(score_json_parser_vajson
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )
# 
# target_link_libraries(score_json_parser_vajson
#     PUBLIC
#         score_compiler_flags
#         score_json_internal_model
#         # TODO: Add vajson library when available
# )
# 
# # Install target
# install(TARGETS score_json_parser_vajson
#     EXPORT score-baselibsTargets
# )

# Create placeholder library for now
add_library(score_json_parser_vajson INTERFACE)
target_link_libraries(score_json_parser_vajson INTERFACE score_compiler_flags)