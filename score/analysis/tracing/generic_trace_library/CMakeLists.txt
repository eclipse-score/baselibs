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

# Tracing error code library
add_library(score_analysis_tracing_error_code STATIC
    interface_types/error_code/error_code.cpp
)

target_include_directories(score_analysis_tracing_error_code PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(score_analysis_tracing_error_code PUBLIC
    score_result
)

# Tracing chunk list library
add_library(score_analysis_tracing_chunk_list STATIC
    interface_types/chunk_list/local_data_chunk_list.cpp
    interface_types/chunk_list/shm_data_chunk_list.cpp
)

target_include_directories(score_analysis_tracing_chunk_list PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(score_analysis_tracing_chunk_list PUBLIC
    score_analysis_tracing_common_interface_types
    score_analysis_tracing_error_code
    score_containers
    score_result
)

# Tracing interface types library  
add_library(score_analysis_tracing_interface_types STATIC
    interface_types/ara_com_properties.cpp
    interface_types/ara_com_meta_info.cpp
)

target_include_directories(score_analysis_tracing_interface_types PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(score_analysis_tracing_interface_types PUBLIC
    score_analysis_tracing_chunk_list
    score_analysis_tracing_error_code
    score_containers
    score_result
)

# Tracing stub implementation library
add_library(score_analysis_tracing_stub STATIC
    stub_implementation/generic_trace_api_stub.cpp
)

target_include_directories(score_analysis_tracing_stub PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(score_analysis_tracing_stub PUBLIC
    score_analysis_tracing_interface_types
    score_analysis_tracing_chunk_list
    score_analysis_tracing_error_code
)

# Tracing mock library (for testing)
add_library(score_analysis_tracing_mock STATIC
    mock/trace_library_mock.cpp
)

target_include_directories(score_analysis_tracing_mock PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../..>
    $<INSTALL_INTERFACE:include>
)

# Find Google Mock for the mock library
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(GMOCK gmock)
endif()

if(GMOCK_FOUND)
    target_link_libraries(score_analysis_tracing_mock PUBLIC
        score_analysis_tracing_interface_types
        score_analysis_tracing_stub
        ${GMOCK_LIBRARIES}
    )
    target_include_directories(score_analysis_tracing_mock PRIVATE ${GMOCK_INCLUDE_DIRS})
    target_compile_options(score_analysis_tracing_mock PRIVATE ${GMOCK_CFLAGS_OTHER})
else()
    message(STATUS "GMock not found, building mock library without Google Mock support")
    target_link_libraries(score_analysis_tracing_mock PUBLIC
        score_analysis_tracing_interface_types
        score_analysis_tracing_stub
    )
endif()

# Main generic trace library combining all components
add_library(score_analysis_tracing_generic INTERFACE)

target_link_libraries(score_analysis_tracing_generic INTERFACE
    score_analysis_tracing_interface_types
    score_analysis_tracing_chunk_list
    score_analysis_tracing_error_code
    score_analysis_tracing_stub
)

# Create aliases
add_library(score::analysis_tracing_error_code ALIAS score_analysis_tracing_error_code)
add_library(score::analysis_tracing_chunk_list ALIAS score_analysis_tracing_chunk_list)
add_library(score::analysis_tracing_interface_types ALIAS score_analysis_tracing_interface_types)
add_library(score::analysis_tracing_stub ALIAS score_analysis_tracing_stub)
add_library(score::analysis_tracing_mock ALIAS score_analysis_tracing_mock)
add_library(score::analysis_tracing_generic ALIAS score_analysis_tracing_generic)

# Install all libraries
install(TARGETS 
    score_analysis_tracing_error_code
    score_analysis_tracing_chunk_list
    score_analysis_tracing_interface_types
    score_analysis_tracing_stub
    score_analysis_tracing_mock
    score_analysis_tracing_generic
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY interface_types/
    DESTINATION include/score/analysis/tracing/generic_trace_library/interface_types
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY mock/
    DESTINATION include/score/analysis/tracing/generic_trace_library/mock
    FILES_MATCHING PATTERN "*.h"
)