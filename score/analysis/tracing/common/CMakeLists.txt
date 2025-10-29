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

cmake_minimum_required(VERSION 3.16)

# Tracing common interface types library
add_library(score_analysis_tracing_common_interface_types STATIC
    interface_types/shared_memory_chunk.cpp
    interface_types/shared_memory_location.cpp
    interface_types/shared_memory_location_helpers.cpp
    interface_types/types.cpp
)

target_include_directories(score_analysis_tracing_common_interface_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(score_analysis_tracing_common_interface_types PUBLIC
    score_containers
    score_result
    score_memory_shared_resource
    score_static_reflection_with_serialization
)

# Add subdirectories for additional components
add_subdirectory(flexible_circular_allocator)
add_subdirectory(shared_list)

# Create alias
add_library(score::analysis_tracing_common_interface_types ALIAS score_analysis_tracing_common_interface_types)

# Install the library
install(TARGETS score_analysis_tracing_common_interface_types
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY interface_types/
    DESTINATION include/score/analysis/tracing/common/interface_types
    FILES_MATCHING PATTERN "*.h"
)
