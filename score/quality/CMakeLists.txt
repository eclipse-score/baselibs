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

# Create quality library with compiler warnings implementation
add_library(score_quality
    compiler_warnings/warnings.cpp
)

target_include_directories(score_quality
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_quality
    PUBLIC
        score_compiler_flags
)
# Create alias target
add_library(score::quality ALIAS score_quality)

# Install target
install(TARGETS score_quality
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    compiler_warnings/warnings.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/quality/compiler_warnings
)
