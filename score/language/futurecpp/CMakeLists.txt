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

# Create futurecpp library with actual implementation
add_library(score_futurecpp
    src/assert.cpp
    src/charconv.cpp
    src/hash.cpp
    src/intrusive_forward_list.cpp
    src/jthread.cpp
    src/math.c
    src/memory_resource.cpp
    src/stop_token.cpp
    src/string.cpp
)

target_include_directories(score_futurecpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_futurecpp
    PUBLIC
        score_compiler_flags
)

# Add examples subdirectory
add_subdirectory(examples)

# Install target
install(TARGETS score_futurecpp
    EXPORT score-baselibsTargets
)

# Install futurecpp headers directly under include/score/ for easy access
install(DIRECTORY include/score/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Note: The main CMakeLists.txt excludes futurecpp/include to prevent nested structure