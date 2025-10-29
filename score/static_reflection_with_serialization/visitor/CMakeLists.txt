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

# Static reflection visitor library - header only
add_library(score_static_reflection_visitor INTERFACE)

target_include_directories(score_static_reflection_visitor
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_static_reflection_visitor
    INTERFACE
        score_compiler_flags
)

# Create alias
add_library(score::static_reflection_visitor ALIAS score_static_reflection_visitor)

# Install target
install(TARGETS score_static_reflection_visitor
    EXPORT score-baselibsTargets
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)