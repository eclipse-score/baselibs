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

# File stream library
add_library(score_filesystem_filestream
    file_buf.cpp
    file_buf.h
    file_factory.cpp
    file_factory.h
    file_stream.cpp
    file_stream.h
    i_file_factory.cpp
    i_file_factory.h
    stdio_filebuf_base.h
)

target_include_directories(score_filesystem_filestream
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_filesystem_filestream
    PUBLIC
        score_compiler_flags
        score_filesystem_error
        score_filesystem_path
        score_language_futurecpp
        score_result
        score_os  # Added OS library dependency
        # score_filesystem_file_status  # Disabled due to OS dependencies
)

# Create alias target
add_library(score::filesystem_filestream ALIAS score_filesystem_filestream)

# Test and mock support libraries (only built when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score_filesystem_filestream_fake
        bad_string_stream_collection.cpp
        bad_string_stream_collection.h
        file_factory_fake.cpp
        file_factory_fake.h
        file_factory_mock.h
        i_string_stream_collection.cpp
        i_string_stream_collection.h
        simple_string_stream_collection.cpp
        simple_string_stream_collection.h
    )

    target_include_directories(score_filesystem_filestream_fake
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_filesystem_filestream_fake
        PUBLIC
            score_filesystem_filestream
            GTest::gtest
    )

    # Create alias target
    add_library(score::score_filesystem_filestream_fake ALIAS score_filesystem_filestream_fake)
endif()

# Install targets
set(FILESTREAM_TARGETS score_filesystem_filestream)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND FILESTREAM_TARGETS score_filesystem_filestream_fake)
endif()

install(TARGETS ${FILESTREAM_TARGETS}
    EXPORT score-baselibsTargets
)