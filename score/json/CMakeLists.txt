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

# Add internal components first
add_subdirectory(internal)

# JSON library
add_library(score_json
    i_json_parser.cpp
    i_json_writer.cpp
    json_parser.cpp
    json_writer.cpp
    json_serializer.cpp
)

target_include_directories(score_json
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/internal>
)

target_link_libraries(score_json
    PUBLIC
        score_compiler_flags
        score_result
        score_json_internal
        score_filesystem_filestream
        score_static_reflection_with_serialization
        score_language_futurecpp
)

# nlohmann_json is now a required dependency
message(STATUS "score_json: Building with nlohmann/json support")

# Create alias target
add_library(score::json ALIAS score_json)

# Add examples subdirectory
add_subdirectory(examples)

# Tests
if(SCORE_BUILD_TESTING)
    # JSON tests
    add_executable(json_test
        json_test.cpp
    )
    
    target_link_libraries(json_test
        PRIVATE
            score_json
            score_strict_warnings
            GTest::gtest_main
    )
    
    target_compile_options(json_test PRIVATE -fexceptions)
    add_test(NAME JsonTest COMMAND json_test)

    # JSON serializer tests
    add_executable(json_serializer_test
        json_serializer_test.cpp
    )
    
    target_link_libraries(json_serializer_test
        PRIVATE
            score_json
            score_strict_warnings
            GTest::gtest_main
    )
    
    target_compile_options(json_serializer_test PRIVATE -fexceptions)
    add_test(NAME JsonSerializerTest COMMAND json_serializer_test)

    # JSON writer tests
    add_executable(json_writer_test
        json_writer_test.cpp
    )
    
    target_link_libraries(json_writer_test
        PRIVATE
            score_json
            score_filesystem_filestream_fake
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    
    target_compile_options(json_writer_test PRIVATE -fexceptions)
    add_test(NAME JsonWriterTest COMMAND json_writer_test)
endif()

# Install target
install(TARGETS score_json
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    i_json_parser.h
    i_json_writer.h
    json_parser.h
    json_writer.h
    json_serializer.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/json
)
