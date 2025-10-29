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

# Timed executor library
add_library(score_timed_executor
    timed_executor.cpp
    timed_task.cpp
    delayed_task.cpp
    periodic_task.cpp
)

target_include_directories(score_timed_executor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_timed_executor
    PUBLIC
        score_compiler_flags
        score_futurecpp
        score_safecpp
        score_concurrency_future
        score_executor
)

# Concurrent timed executor library
add_library(score_concurrent_timed_executor
    concurrent_timed_executor.cpp
)

target_include_directories(score_concurrent_timed_executor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_concurrent_timed_executor
    PUBLIC
        score_compiler_flags
        score_timed_executor
)

# Timed executor mock (only built when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score_timed_executor_mock
        timed_executor_mock.cpp
    )

    target_include_directories(score_timed_executor_mock
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_timed_executor_mock
        PUBLIC
            score_compiler_flags
            score_timed_executor
            GTest::gmock
    )
endif()

# Tests
if(SCORE_BUILD_TESTING)
    # Concurrent timed executor tests
    add_executable(concurrent_timed_executor_test
        concurrent_timed_executor_test.cpp
    )
    
    target_link_libraries(concurrent_timed_executor_test
        PRIVATE
            score_concurrent_timed_executor
            score_timed_executor_mock
            score_concurrency
            score_strict_warnings
            GTest::gmock_main
            GTest::gtest_main
    )
    
    target_compile_options(concurrent_timed_executor_test PRIVATE -fexceptions)
    add_test(NAME ConcurrentTimedExecutorTest COMMAND concurrent_timed_executor_test)

    # Delayed task tests
    add_executable(timed_delayed_task_test
        delayed_task_test.cpp
    )
    
    target_link_libraries(timed_delayed_task_test
        PRIVATE
            score_timed_executor
            score_concurrency_future
            score_timed_executor_mock
            score_strict_warnings
            GTest::gmock_main
            GTest::gtest_main
    )
    
    target_compile_options(timed_delayed_task_test PRIVATE -fexceptions)
    add_test(NAME TimedDelayedTaskTest COMMAND timed_delayed_task_test)

    # Periodic task tests
    add_executable(timed_periodic_task_test
        periodic_task_test.cpp
    )
    
    target_link_libraries(timed_periodic_task_test
        PRIVATE
            score_timed_executor
            score_concurrency_future
            score_timed_executor_mock
            score_strict_warnings
            GTest::gmock_main
            GTest::gtest_main
    )
    
    target_compile_options(timed_periodic_task_test PRIVATE -fexceptions)
    add_test(NAME TimedPeriodicTaskTest COMMAND timed_periodic_task_test)
endif()

# Create aliases
add_library(score::timed_executor ALIAS score_timed_executor)
add_library(score::concurrent_timed_executor ALIAS score_concurrent_timed_executor)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score::timed_executor_mock ALIAS score_timed_executor_mock)
endif()

# Install targets
set(TIMED_EXECUTOR_TARGETS
    score_timed_executor 
    score_concurrent_timed_executor
)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND TIMED_EXECUTOR_TARGETS score_timed_executor_mock)
endif()

install(TARGETS ${TIMED_EXECUTOR_TARGETS}
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
set(TIMED_EXECUTOR_HEADERS
    timed_executor.h
    timed_task.h
    delayed_task.h
    periodic_task.h
    concurrent_timed_executor.h
)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND TIMED_EXECUTOR_HEADERS timed_executor_mock.h)
endif()

install(FILES ${TIMED_EXECUTOR_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/concurrency/timed_executor
)