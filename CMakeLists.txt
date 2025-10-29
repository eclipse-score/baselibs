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

cmake_minimum_required(VERSION 3.20)

# Set policies to handle deprecations gracefully
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)  # Keep using FindBoost
endif()


project(score-baselibs
    VERSION 0.1.0
    DESCRIPTION "Eclipse SCORE Base Libraries"
    LANGUAGES C CXX
)

# Include standard modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clangd and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings configuration
set(SCORE_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wno-error=deprecated-declarations
    -Wno-error=narrowing
)

# Treat warnings as errors in strict mode (disabled to allow builds with sign-conversion warnings)
option(SCORE_TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
if(SCORE_TREAT_WARNINGS_AS_ERRORS)
    list(APPEND SCORE_WARNING_FLAGS -Werror)
endif()

# Add additional warning flags
set(SCORE_ADDITIONAL_WARNING_FLAGS
    -Wcast-qual
    -Wformat=2
    -Wundef
    -Wwrite-strings
    -Wconversion
    -Wno-sign-conversion  # Disabled to avoid build errors with size_t conversions
    -Wunused-parameter
    -Wredundant-decls
    -Wshadow
)

# Testing configuration
option(SCORE_BUILD_TESTING "Build tests" ON)
set(GTEST_FOUND FALSE)
if(SCORE_BUILD_TESTING)
    enable_testing()
    
    # Find GoogleTest and GMock comprehensively
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        # Try pkg-config approach
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GTEST IMPORTED_TARGET gtest)
            pkg_check_modules(GTEST_MAIN IMPORTED_TARGET gtest_main)
            pkg_check_modules(GMOCK IMPORTED_TARGET gmock)
            pkg_check_modules(GMOCK_MAIN IMPORTED_TARGET gmock_main)
            
            if(GTEST_FOUND AND GTEST_MAIN_FOUND AND GMOCK_FOUND AND GMOCK_MAIN_FOUND)
                # Create standard GTest targets
                add_library(GTest::gtest ALIAS PkgConfig::GTEST)
                add_library(GTest::gtest_main ALIAS PkgConfig::GTEST_MAIN)
                add_library(GTest::gmock ALIAS PkgConfig::GMOCK)
                add_library(GTest::gmock_main ALIAS PkgConfig::GMOCK_MAIN)
                set(GTest_FOUND TRUE)
            endif()
        endif()
    endif()

    if(GTest_FOUND)
        set(GTEST_FOUND TRUE)
        message(STATUS "GoogleTest found - tests will be built")
        message(STATUS "GTest found - additional testing enabled")
        
        # Ensure all targets exist
        if(NOT TARGET GTest::gmock)
            find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
                PATHS /usr/include
            )
            
            find_library(GMOCK_LIBRARY 
                NAMES gmock
                PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
            )
            find_library(GMOCK_MAIN_LIBRARY 
                NAMES gmock_main
                PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib
            )
            
            if(GMOCK_LIBRARY AND GMOCK_INCLUDE_DIR)
                add_library(GTest::gmock UNKNOWN IMPORTED)
                set_target_properties(GTest::gmock PROPERTIES 
                    IMPORTED_LOCATION "${GMOCK_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIR}"
                )
                # GMock depends on GTest - ensure it's properly linked
                target_link_libraries(GTest::gmock INTERFACE GTest::gtest pthread)
            endif()
            
            if(GMOCK_MAIN_LIBRARY AND TARGET GTest::gmock)
                add_library(GTest::gmock_main UNKNOWN IMPORTED)
                set_target_properties(GTest::gmock_main PROPERTIES 
                    IMPORTED_LOCATION "${GMOCK_MAIN_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIR}"
                )
                # GMock main depends on both GMock and GTest main
                target_link_libraries(GTest::gmock_main INTERFACE GTest::gmock GTest::gtest_main GTest::gtest)
            endif()
        endif()
    else()
        message(WARNING "GoogleTest not found - tests will be disabled")
        message(STATUS "GTest not found - testing disabled")
        set(SCORE_BUILD_TESTING OFF)
    endif()
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS container system)
message(STATUS "Boost found - memory shared features enabled")

# Find nlohmann/json for JSON support
# Since nlohmann_json is now required, we need a fallback if CMake find fails
find_package(nlohmann_json REQUIRED)
if(NOT nlohmann_json_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(NLOHMANN_JSON REQUIRED IMPORTED_TARGET nlohmann_json)
        # Create alias for consistent usage
        add_library(nlohmann_json::nlohmann_json ALIAS PkgConfig::NLOHMANN_JSON)
    else()
        message(FATAL_ERROR "nlohmann_json is required but not found. Please install nlohmann_json development package.")
    endif()
endif()
message(STATUS "nlohmann/json found - JSON support enabled")

# Find optional dependencies for third-party libraries
pkg_check_modules(ACL IMPORTED_TARGET libacl)
pkg_check_modules(LIBCAP IMPORTED_TARGET libcap)
pkg_check_modules(LIBSECCOMP IMPORTED_TARGET libseccomp)

# Create a common interface library for compiler flags
add_library(score_compiler_flags INTERFACE)
target_compile_options(score_compiler_flags INTERFACE ${SCORE_WARNING_FLAGS})
target_include_directories(score_compiler_flags INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Create an interface library for additional warnings
add_library(score_additional_warnings INTERFACE)
target_compile_options(score_additional_warnings INTERFACE ${SCORE_ADDITIONAL_WARNING_FLAGS})

# Create an interface library for strict warnings (combines both)
add_library(score_strict_warnings INTERFACE)
target_link_libraries(score_strict_warnings INTERFACE
    score_compiler_flags
    score_additional_warnings
)

# Helper function to link GoogleTest to a target
function(score_link_gtest target_name)
    if(GTest_FOUND)
        target_link_libraries(${target_name} PRIVATE GTest::gtest_main)
    elseif(GTEST_FOUND)
        target_link_libraries(${target_name} PRIVATE ${GTEST_LIBRARIES})
        target_include_directories(${target_name} PRIVATE ${GTEST_INCLUDE_DIRS})
        target_compile_options(${target_name} PRIVATE ${GTEST_CFLAGS_OTHER})
    endif()
endfunction()

# Add subdirectories for each score module
add_subdirectory(score/analysis)
add_subdirectory(score/bitmanipulation)
add_subdirectory(score/concurrency)
add_subdirectory(score/containers)
add_subdirectory(score/datetime_converter)
add_subdirectory(score/filesystem)
add_subdirectory(score/json)
add_subdirectory(score/language)
add_subdirectory(score/memory)
add_subdirectory(score/mw)
add_subdirectory(score/os)  # Re-enabled - fixing OS library properly
add_subdirectory(score/quality)
add_subdirectory(score/result)
add_subdirectory(score/static_reflection_with_serialization)
add_subdirectory(score/utils)

# Optional: Add third-party subdirectories if needed
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
    add_subdirectory(third_party)
endif()

# Create a meta-target that includes all score libraries
add_library(score_all INTERFACE)
target_link_libraries(score_all INTERFACE
    score_bitmanipulation
    score_bitmask_operators
    score_concurrency
    score_concurrency_future
    score_thread_load_tracking
    score_timed_executor
    score_containers
    score_datetime_converter
    score_filesystem
    score_json
    score_language
    score_memory
    score_mw
    score_os
    score_quality
    score_result
    score_static_reflection_with_serialization
    score_utils
)

# Install configuration
include(GNUInstallDirs)

# Install the meta-target and interface libraries
install(TARGETS 
    score_all
    score_compiler_flags
    score_additional_warnings
    score_strict_warnings
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Create aliases for commonly used targets (without score:: namespace)
# This allows projects to use score_* targets directly
if(TARGET score_memory_shared_resource)
    add_library(score-baselibs::score_memory_shared_resource ALIAS score_memory_shared_resource)
endif()
if(TARGET score_executor)
    add_library(score-baselibs::score_executor ALIAS score_executor)
endif()
if(TARGET score_filesystem)
    add_library(score-baselibs::score_filesystem ALIAS score_filesystem)
endif()
if(TARGET score_analysis_tracing_common_interface_types)
    add_library(score-baselibs::score_analysis_tracing_common_interface_types ALIAS score_analysis_tracing_common_interface_types)
endif()

# Export targets
install(EXPORT score-baselibsTargets
    FILE score-baselibsTargets.cmake
    NAMESPACE score::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/score-baselibs
)

# Create and install package config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/score-baselibsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/score-baselibsConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/score-baselibs
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/score-baselibsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/score-baselibsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/score-baselibsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/score-baselibs
)
