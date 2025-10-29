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

# Core shared memory interfaces
add_library(score_memory_shared_interfaces
    i_shared_memory_factory.cpp
    i_shared_memory_resource.cpp
)

target_include_directories(score_memory_shared_interfaces
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_interfaces
    PUBLIC
        score_compiler_flags
        score_result
)

# Memory region utilities
add_library(score_memory_shared_region
    memory_region_bounds.cpp
    memory_region_map.cpp
    pointer_arithmetic_util.cpp
)

target_include_directories(score_memory_shared_region
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_region
    PUBLIC
        score_compiler_flags
        score_memory_shared_interfaces
        score_static_reflection_with_serialization
)

# Lock utilities (moved before resource to resolve dependencies)
add_library(score_memory_shared_sync
    lock_file.cpp
)

target_include_directories(score_memory_shared_sync
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_sync
    PUBLIC
        score_compiler_flags
        score_os
)

# Memory resource implementations
add_library(score_memory_shared_resource
    memory_resource_proxy.cpp
    memory_resource_registry.cpp
    managed_memory_resource.cpp
    new_delete_delegate_resource.cpp
    shared_memory_resource.cpp
    shared_memory_error.cpp
    atomic_indirector.cpp
    atomic_mock.cpp
)

target_include_directories(score_memory_shared_resource
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_resource
    PUBLIC
        score_compiler_flags
        score_memory_shared_interfaces
        score_memory_shared_region
        score_memory_shared_sync
        score_memory_shared_sealedshm
        score_memory_shared_typedshm
        Boost::boost
)

# Shared memory factory
add_library(score_memory_shared_factory
    shared_memory_factory.cpp
    shared_memory_factory_impl.cpp
    shared_memory_factory_mock.cpp
)

target_include_directories(score_memory_shared_factory
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_factory
    PUBLIC
        score_compiler_flags
        score_memory_shared_resource
        Boost::boost
)

# Collections and containers
add_library(score_memory_shared_containers
    string.cpp
    vector.cpp
    map.cpp
    polymorphic_offset_ptr_allocator.cpp
    offset_ptr.cpp
    offset_ptr_bounds_check.cpp
)

target_include_directories(score_memory_shared_containers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_containers
    PUBLIC
        score_compiler_flags
        score_memory_shared_resource
        Boost::boost
)

# Test resources and utilities
add_library(score_memory_shared_test_resources
    shared_memory_test_resources.cpp
    shared_memory_factory_mock.cpp
    shared_memory_resource_mock.cpp
    shared_memory_resource_heap_allocator_mock.cpp
)

target_include_directories(score_memory_shared_test_resources
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_test_resources
    PUBLIC
        score_compiler_flags
        score_memory_shared_factory
)

# Add subdirectories for sub-components
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/flock" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/flock/CMakeLists.txt")
    add_subdirectory(flock)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sealedshm" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sealedshm/CMakeLists.txt")
    add_subdirectory(sealedshm)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/typedshm" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/typedshm/CMakeLists.txt")
    add_subdirectory(typedshm)
endif()

# Create aliases
add_library(score::memory_shared_interfaces ALIAS score_memory_shared_interfaces)
add_library(score::memory_shared_region ALIAS score_memory_shared_region)
add_library(score::memory_shared_resource ALIAS score_memory_shared_resource)
add_library(score::memory_shared_factory ALIAS score_memory_shared_factory)
add_library(score::memory_shared_containers ALIAS score_memory_shared_containers)
add_library(score::memory_shared_sync ALIAS score_memory_shared_sync)
add_library(score::memory_shared_test_resources ALIAS score_memory_shared_test_resources)

# Install all targets
install(TARGETS 
    score_memory_shared_interfaces
    score_memory_shared_region
    score_memory_shared_resource
    score_memory_shared_factory
    score_memory_shared_containers
    score_memory_shared_sync
    score_memory_shared_test_resources
    EXPORT score-baselibsTargets
)

# Install all headers recursively with directory exclusions
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/memory/shared
    FILES_MATCHING 
    PATTERN "*.h"
    # Exclude directories
    PATTERN "fake" EXCLUDE
    PATTERN "flags" EXCLUDE
    PATTERN "flock" EXCLUDE
    PATTERN "sealedshm" EXCLUDE
    PATTERN "test_offset_ptr" EXCLUDE
    PATTERN "typedshm" EXCLUDE
)
