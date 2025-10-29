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

# String literal header-only library
add_library(score_memory_string_literal INTERFACE)

target_include_directories(score_memory_string_literal
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_string_literal
    INTERFACE
        score_compiler_flags
)

# Endianness library
add_library(score_memory_endianness
    endianness.cpp
    endianness.h
)

target_include_directories(score_memory_endianness
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_endianness
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
)

# String comparison adaptor library
add_library(score_memory_string_comparison_adaptor
    string_comparison_adaptor.cpp
    string_comparison_adaptor.h
)

target_include_directories(score_memory_string_comparison_adaptor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_string_comparison_adaptor
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
        score_memory_string_literal
)

# Split string view library
add_library(score_memory_split_string_view
    split_string_view.cpp
    split_string_view.h
)

target_include_directories(score_memory_split_string_view
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_split_string_view
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
)

# Main memory library (interface combining all components)
add_library(score_memory INTERFACE)

target_link_libraries(score_memory
    INTERFACE
        score_memory_string_literal
        score_memory_endianness
        score_memory_string_comparison_adaptor
        score_memory_split_string_view
)

# Create alias targets
add_library(score::memory_string_literal ALIAS score_memory_string_literal)
add_library(score::memory_endianness ALIAS score_memory_endianness)
add_library(score::memory_string_comparison_adaptor ALIAS score_memory_string_comparison_adaptor)
add_library(score::memory_split_string_view ALIAS score_memory_split_string_view)
add_library(score::memory ALIAS score_memory)

# Install targets
install(TARGETS 
    score_memory_string_literal
    score_memory_endianness
    score_memory_string_comparison_adaptor
    score_memory_split_string_view
    score_memory
    EXPORT score-baselibsTargets
)

# Add subdirectories for sub-components
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shared" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shared/CMakeLists.txt")
    add_subdirectory(shared)
endif()
