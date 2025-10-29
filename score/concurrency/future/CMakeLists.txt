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

# Future library
add_library(score_concurrency_future
    base_interruptible_future.cpp
    base_interruptible_promise.cpp
    base_interruptible_state.cpp
    error.cpp
    interruptible_future.cpp
    interruptible_promise.cpp
    interruptible_shared_future.cpp
    interruptible_state.cpp
)

target_include_directories(score_concurrency_future
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_concurrency_future
    PUBLIC
        score_compiler_flags
        score_interruptible_wait
        score_futurecpp
        score_safecpp
)

# Test types library
add_library(score_concurrency_future_test_types
    test_types.cpp
)

target_include_directories(score_concurrency_future_test_types
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_concurrency_future_test_types
    PUBLIC
        score_compiler_flags
)

# Tests
if(SCORE_BUILD_TESTING)
    add_executable(future_unit_test
        base_interruptible_future_test.cpp
        base_interruptible_promise_test.cpp
        base_interruptible_state_test.cpp
        error_test.cpp
        interruptible_future_test.cpp
        interruptible_promise_test.cpp
        interruptible_shared_future_test.cpp
        interruptible_state_test.cpp
    )
    
    target_link_libraries(future_unit_test
        PRIVATE
            score_concurrency_future
            score_concurrency_future_test_types
            score_result
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    
    target_compile_options(future_unit_test PRIVATE -fexceptions)
    
    add_test(NAME FutureUnitTest COMMAND future_unit_test)
endif()

# Create aliases
add_library(score::concurrency_future ALIAS score_concurrency_future)
add_library(score::concurrency_future_test_types ALIAS score_concurrency_future_test_types)

# Install targets
install(TARGETS score_concurrency_future score_concurrency_future_test_types
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    base_interruptible_future.h
    base_interruptible_promise.h
    base_interruptible_state.h
    error.h
    interruptible_future.h
    interruptible_promise.h
    interruptible_shared_future.h
    interruptible_state.h
    test_types.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/concurrency/future
)