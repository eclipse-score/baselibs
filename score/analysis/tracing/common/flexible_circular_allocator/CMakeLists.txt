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

# Flexible circular allocator library
add_library(score_analysis_tracing_flexible_circular_allocator
    custom_polymorphic_offset_ptr_allocator.cpp
    error_code.cpp
    flexible_circular_allocator.cpp
    flexible_circular_allocator_interface.cpp
    lockless_flexible_circular_allocator.cpp
)

target_include_directories(score_analysis_tracing_flexible_circular_allocator
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_analysis_tracing_flexible_circular_allocator
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
        score_memory
        score_static_reflection_visitor
        score_analysis_tracing_common_interface_types
)

# Create alias target
add_library(score::analysis_tracing_flexible_circular_allocator ALIAS score_analysis_tracing_flexible_circular_allocator)

# Install target
install(TARGETS score_analysis_tracing_flexible_circular_allocator
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    custom_polymorphic_offset_ptr_allocator.h
    error_code.h
    flexible_circular_allocator.h
    flexible_circular_allocator_factory.h
    flexible_circular_allocator_factory_interface.h
    flexible_circular_allocator_interface.h
    lockless_flexible_circular_allocator.h
    lockless_flexible_circular_allocator_factory.h
    lockless_flexible_circular_allocator_types.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/analysis/tracing/common/flexible_circular_allocator
)