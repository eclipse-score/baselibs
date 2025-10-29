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

# Shared list library
add_library(score_analysis_tracing_shared_list
    shared_list.cpp
)

target_include_directories(score_analysis_tracing_shared_list
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_analysis_tracing_shared_list
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
        score_analysis_tracing_flexible_circular_allocator
)

# Create alias target
add_library(score::analysis_tracing_shared_list ALIAS score_analysis_tracing_shared_list)

# Install target
install(TARGETS score_analysis_tracing_shared_list
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    shared_list.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/analysis/tracing/common/shared_list
)