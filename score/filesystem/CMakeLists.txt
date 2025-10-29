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

# Path library
add_library(score_filesystem_path
    path.cpp
    path.h
)

target_include_directories(score_filesystem_path
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem_path
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
)

# Error library
add_library(score_filesystem_error
    error.cpp
    error.h
)

target_include_directories(score_filesystem_error
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem_error
    PUBLIC
        score_compiler_flags
        score_result
)

# File status library
add_library(score_filesystem_file_status
    file_status.cpp
    file_status.h
)

target_include_directories(score_filesystem_file_status
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem_file_status
    PUBLIC
        score_compiler_flags
        score_os  # Now that OS library is available
)

# Standard filesystem interface
add_library(score_filesystem_i_standard
    i_standard_filesystem.cpp
    i_standard_filesystem.h
)

target_include_directories(score_filesystem_i_standard
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem_i_standard
    PUBLIC
        score_compiler_flags
        score_filesystem_error
        score_filesystem_file_status
        score_filesystem_path
)

# Main filesystem library
add_library(score_filesystem
    filesystem.h
    filesystem_struct.cpp
    filesystem_struct.h
    
    # Core implementation files
    details/standard_filesystem.cpp
    
    # Factory implementation
    factory/filesystem_factory.cpp
    factory/i_filesystem_factory.cpp
    
    # File utilities implementation
    file_utils/file_utils.cpp
    file_utils/i_file_utils.cpp
    
    # Iterator functionality
    iterator/directory_entry.cpp
    iterator/directory_iterator.cpp
    iterator/recursive_directory_iterator.cpp
)

# Filesystem Factory Fake library for testing (only built when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score_filesystem_factory_fake
        factory/filesystem_factory_fake.cpp
        factory/filesystem_factory_fake.h
        factory/filesystem_factory_mock_objects.cpp
        factory/filesystem_factory_mock_objects.h
        factory/filesystem_factory_mock.h
        standard_filesystem_fake.cpp
        standard_filesystem_fake.h
        file_utils/file_utils_fake.cpp
        file_utils/file_utils_fake.h
        file_utils/file_utils_mock.h
        iterator/dirent_fake.cpp
        iterator/dirent_fake.h
    )

    target_include_directories(score_filesystem_factory_fake
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_filesystem_factory_fake
        PUBLIC
            score_compiler_flags
            score_filesystem
            score_filesystem_filestream_fake
    )

    # Create alias target
    add_library(score::score_filesystem_factory_fake ALIAS score_filesystem_factory_fake)
endif()

target_include_directories(score_filesystem
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem
    PUBLIC
        score_compiler_flags
        score_filesystem_path
        score_filesystem_error
        score_filesystem_file_status
        score_filesystem_i_standard
        score_filesystem_filestream
        score_os  # Add OS dependency for filesystem operations
)

# Create alias targets
add_library(score::filesystem_path ALIAS score_filesystem_path)
add_library(score::filesystem_error ALIAS score_filesystem_error)
add_library(score::filesystem_file_status ALIAS score_filesystem_file_status)
add_library(score::filesystem_i_standard ALIAS score_filesystem_i_standard)
add_library(score::filesystem ALIAS score_filesystem)

# Install targets
set(FILESYSTEM_TARGETS
    score_filesystem_path
    score_filesystem_error
    score_filesystem_file_status
    score_filesystem_i_standard
    score_filesystem
)

if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND FILESYSTEM_TARGETS score_filesystem_factory_fake)
endif()

install(TARGETS ${FILESYSTEM_TARGETS}
    EXPORT score-baselibsTargets
)

# Install headers
install(FILES
    path.h
    error.h
    file_status.h
    i_standard_filesystem.h
    filesystem.h
    filesystem_struct.h
    factory/i_filesystem_factory.h
    file_utils/i_file_utils.h
    iterator/directory_entry.h
    iterator/directory_iterator.h
    iterator/recursive_directory_iterator.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/filesystem
)

# Install fake headers when testing is enabled
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    install(FILES
        factory/filesystem_factory_fake.h
        factory/filesystem_factory_mock_objects.h
        factory/filesystem_factory_mock.h
        standard_filesystem_fake.h
        file_utils/file_utils_fake.h
        file_utils/file_utils_mock.h
        iterator/dirent_fake.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/filesystem
    )
endif()

# Add subdirectories
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/filestream")
    add_subdirectory(filestream)
endif()