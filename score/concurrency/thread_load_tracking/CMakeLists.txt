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

# Thread load tracking library
add_library(score_thread_load_tracking
    thread_load_tracking.cpp
    thread_load_tracking_token.cpp
)

target_include_directories(score_thread_load_tracking
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_thread_load_tracking
    PUBLIC
        score_compiler_flags
        score_futurecpp
)

# Tests
if(SCORE_BUILD_TESTING)
    add_executable(thread_load_tracking_test
        thread_load_tracking_test.cpp
    )
    
    target_link_libraries(thread_load_tracking_test
        PRIVATE
            score_thread_load_tracking
            score_strict_warnings
            GTest::gtest_main
    )
    
    target_compile_options(thread_load_tracking_test PRIVATE -fexceptions)
    
    add_test(NAME ThreadLoadTrackingTest COMMAND thread_load_tracking_test)
endif()

# Create alias
add_library(score::thread_load_tracking ALIAS score_thread_load_tracking)

# Install targets
install(TARGETS score_thread_load_tracking
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    thread_load_tracking.h
    thread_load_tracking_state.h
    thread_load_tracking_token.h
    work_load.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/concurrency/thread_load_tracking
)