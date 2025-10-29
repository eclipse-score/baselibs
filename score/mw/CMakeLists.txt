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

# Create main middleware library
add_library(score_mw INTERFACE)

target_include_directories(score_mw
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw
    INTERFACE
        score_compiler_flags
)

# Add subdirectories for sub-components
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/log" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/log/CMakeLists.txt")
    add_subdirectory(log)
    target_link_libraries(score_mw
        INTERFACE
            score_mw_log
    )
endif()

# Create alias target
add_library(score::mw ALIAS score_mw)

# Install target
install(TARGETS score_mw
    EXPORT score-baselibsTargets
)
