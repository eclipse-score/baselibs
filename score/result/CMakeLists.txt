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

# Create result library
add_library(score_result
    error.cpp
    error_code.cpp
    error_domain.cpp
    result.cpp
    details/expected/expected.cpp
    details/expected/extensions.cpp
    details/expected/test_types.cpp
    details/expected/test_type_traits.cpp
)

target_include_directories(score_result
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_result
    PUBLIC
        score_compiler_flags
        score_futurecpp
)
# Create alias target
add_library(score::result ALIAS score_result)

# Install targets
install(TARGETS score_result
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    error.h
    error_code.h
    error_domain.h
    result.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/result
)

# Tests
if(SCORE_BUILD_TESTING)
    add_executable(result_test
        error_test.cpp
        result_test.cpp
    )
    
    target_link_libraries(result_test
        PRIVATE
            score_result
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    
    target_compile_options(result_test PRIVATE -fexceptions)
    
    add_test(NAME ResultTest COMMAND result_test)
endif()
