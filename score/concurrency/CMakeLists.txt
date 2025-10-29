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

# Add subdirectories
add_subdirectory(thread_load_tracking)
add_subdirectory(timed_executor)

# Clock library (testonly)
add_library(score_clock
    clock.cpp
)

target_include_directories(score_clock
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_clock
    PUBLIC
        score_compiler_flags
)

# Destruction guard library
add_library(score_destruction_guard
    destruction_guard.cpp
)

target_include_directories(score_destruction_guard
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_destruction_guard
    PUBLIC
        score_compiler_flags
)

# Condition variable library
add_library(score_condition_variable
    condition_variable.cpp
)

target_include_directories(score_condition_variable
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_condition_variable
    PUBLIC
        score_compiler_flags
        score_destruction_guard
        score_futurecpp
)

# Interruptible wait library
add_library(score_interruptible_wait
    interruptible_wait.cpp
)

target_include_directories(score_interruptible_wait
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_interruptible_wait
    PUBLIC
        score_compiler_flags
        score_condition_variable
)

# Add future subdirectory now that dependencies are available
add_subdirectory(future)

# Notification library
add_library(score_notification
    notification.cpp
)

target_include_directories(score_notification
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_notification
    PUBLIC
        score_compiler_flags
        score_destruction_guard
        score_interruptible_wait
)

# Interruptible interprocess condition variable
add_library(score_interruptible_interprocess_condition_variable
    interruptible_interprocess_condition_variable.cpp
)

target_include_directories(score_interruptible_interprocess_condition_variable
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_interruptible_interprocess_condition_variable
    PUBLIC
        score_compiler_flags
        score_condition_variable
)

# Executor and task libraries
add_library(score_executor
    executor.cpp
    simple_task.cpp
    task.cpp
    task_result_base.cpp
    task_result.cpp
    shared_task_result.cpp
)

target_include_directories(score_executor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_executor
    PUBLIC
        score_compiler_flags
        score_notification
        score_futurecpp
        score_safecpp
)

# Mock libraries (only built when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    # Executor mock
    add_library(score_executor_mock
        executor_mock.cpp
    )

    target_include_directories(score_executor_mock
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_executor_mock
        PUBLIC
            score_compiler_flags
            score_executor
            GTest::gmock
    )

    # Interruptible conditional variable mock
    add_library(score_interruptible_conditional_variable_mock
        interruptible_conditional_variable_mock.cpp
    )

    target_include_directories(score_interruptible_conditional_variable_mock
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_link_libraries(score_interruptible_conditional_variable_mock
        PUBLIC
            score_compiler_flags
            score_futurecpp
            GTest::gmock
    )
endif()

# Delayed task library
add_library(score_delayed_task
    delayed_task.cpp
)

target_include_directories(score_delayed_task
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_delayed_task
    PUBLIC
        score_compiler_flags
        score_executor
)

# Periodic task library
add_library(score_periodic_task
    periodic_task.cpp
)

target_include_directories(score_periodic_task
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_periodic_task
    PUBLIC
        score_compiler_flags
        score_delayed_task
)

# Long running threads container
add_library(score_long_running_threads_container
    long_running_threads_container.cpp
)

target_include_directories(score_long_running_threads_container
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_long_running_threads_container
    PUBLIC
        score_compiler_flags
        score_futurecpp
        score_safecpp
)

# Synchronized queue
add_library(score_synchronized_queue
    synchronized_queue.cpp
)

target_include_directories(score_synchronized_queue
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_synchronized_queue
    PUBLIC
        score_compiler_flags
        score_condition_variable
)

# Thread pool
add_library(score_thread_pool
    thread_pool.cpp
)

target_include_directories(score_thread_pool
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_thread_pool
    PUBLIC
        score_compiler_flags
        score_synchronized_queue
        score_notification
        score_concurrency_future
        score_executor
        pthread
)

# Create a combined concurrency library
add_library(score_concurrency INTERFACE)
target_link_libraries(score_concurrency INTERFACE
    score_clock
    score_destruction_guard
    score_condition_variable
    score_interruptible_wait
    score_notification
    score_interruptible_interprocess_condition_variable
    score_executor
    score_delayed_task
    score_periodic_task
    score_long_running_threads_container
    score_synchronized_queue
    score_thread_pool
    score_concurrency_future
)

# Install interface target
install(TARGETS score_concurrency
    EXPORT score-baselibsTargets
)

# Create alias targets
add_library(score::clock ALIAS score_clock)
add_library(score::destruction_guard ALIAS score_destruction_guard)
add_library(score::condition_variable ALIAS score_condition_variable)
add_library(score::interruptible_wait ALIAS score_interruptible_wait)
add_library(score::notification ALIAS score_notification)
add_library(score::interruptible_interprocess_condition_variable ALIAS score_interruptible_interprocess_condition_variable)
add_library(score::executor ALIAS score_executor)
add_library(score::delayed_task ALIAS score_delayed_task)
add_library(score::periodic_task ALIAS score_periodic_task)
add_library(score::long_running_threads_container ALIAS score_long_running_threads_container)
add_library(score::synchronized_queue ALIAS score_synchronized_queue)
add_library(score::thread_pool ALIAS score_thread_pool)
add_library(score::concurrency ALIAS score_concurrency)

# Mock library aliases (only when testing is enabled)
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    add_library(score::executor_mock ALIAS score_executor_mock)
    add_library(score::interruptible_conditional_variable_mock ALIAS score_interruptible_conditional_variable_mock)
endif()

# Tests
if(SCORE_BUILD_TESTING)
    # Clock tests
    add_executable(clock_test
        clock_test.cpp
    )
    target_link_libraries(clock_test
        PRIVATE
            score_clock
            score_concurrency_future
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(clock_test PRIVATE -fexceptions)
    add_test(NAME ClockTest COMMAND clock_test)

    # Condition variable tests
    add_executable(condition_variable_test
        condition_variable_test.cpp
    )
    target_link_libraries(condition_variable_test
        PRIVATE
            score_clock
            score_condition_variable
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(condition_variable_test PRIVATE -fexceptions)
    add_test(NAME ConditionVariableTest COMMAND condition_variable_test)

    # Interruptible wait tests
    add_executable(interruptible_wait_test
        interruptible_wait_test.cpp
    )
    target_link_libraries(interruptible_wait_test
        PRIVATE
            score_interruptible_wait
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(interruptible_wait_test PRIVATE -fexceptions)
    add_test(NAME InterruptibleWaitTest COMMAND interruptible_wait_test)

    # Notification tests
    add_executable(notification_test
        notification_test.cpp
    )
    target_link_libraries(notification_test
        PRIVATE
            score_notification
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(notification_test PRIVATE -fexceptions)
    add_test(NAME NotificationTest COMMAND notification_test)

    # Additional tests for other components...
    # (Adding all tests would make this very long, but following the same pattern)
    
    # Executor tests
    add_executable(executor_test
        executor_test.cpp
    )
    target_link_libraries(executor_test
        PRIVATE
            score_executor
            score_executor_mock
            score_concurrency_future
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(executor_test PRIVATE -fexceptions)
    add_test(NAME ExecutorTest COMMAND executor_test)

    # Task tests
    add_executable(task_test
        task_test.cpp
    )
    target_link_libraries(task_test
        PRIVATE
            score_executor
            score_executor_mock
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(task_test PRIVATE -fexceptions)
    add_test(NAME TaskTest COMMAND task_test)

    # Simple task tests
    add_executable(simple_task_test
        simple_task_test.cpp
    )
    target_link_libraries(simple_task_test
        PRIVATE
            score_executor
            score_concurrency_future
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(simple_task_test PRIVATE -fexceptions)
    add_test(NAME SimpleTaskTest COMMAND simple_task_test)

    # Task result tests
    add_executable(task_result_test
        task_result_test.cpp
    )
    target_link_libraries(task_result_test
        PRIVATE
            score_executor
            score_executor_mock
            score_concurrency_future
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(task_result_test PRIVATE -fexceptions)
    add_test(NAME TaskResultTest COMMAND task_result_test)

    # Delayed task tests
    add_executable(delayed_task_test
        delayed_task_test.cpp
    )
    target_link_libraries(delayed_task_test
        PRIVATE
            score_delayed_task
            score_concurrency_future
            score_executor
            score_safecpp
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(delayed_task_test PRIVATE -fexceptions)
    add_test(NAME DelayedTaskTest COMMAND delayed_task_test)

    # Periodic task tests
    add_executable(periodic_task_test
        periodic_task_test.cpp
    )
    target_link_libraries(periodic_task_test
        PRIVATE
            score_periodic_task
            score_concurrency_future
            score_strict_warnings
            GTest::gtest_main
            GTest::gmock
    )
    target_compile_options(periodic_task_test PRIVATE -fexceptions)
    add_test(NAME PeriodicTaskTest COMMAND periodic_task_test)

    # Long running threads container tests
    add_executable(long_running_threads_container_test
        long_running_threads_container_test.cpp
    )
    target_link_libraries(long_running_threads_container_test
        PRIVATE
            score_long_running_threads_container
            score_concurrency_future
            score_executor
            score_safecpp
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(long_running_threads_container_test PRIVATE -fexceptions)
    add_test(NAME LongRunningThreadsContainerTest COMMAND long_running_threads_container_test)

    # Synchronized queue tests
    add_executable(synchronized_queue_test
        synchronized_queue_test.cpp
    )
    target_link_libraries(synchronized_queue_test
        PRIVATE
            score_synchronized_queue
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(synchronized_queue_test PRIVATE -fexceptions)
    add_test(NAME SynchronizedQueueTest COMMAND synchronized_queue_test)

    # Thread pool tests
    add_executable(thread_pool_test
        thread_pool_test.cpp
    )
    target_link_libraries(thread_pool_test
        PRIVATE
            score_thread_pool
            score_concurrency_future
            score_executor
            score_safecpp
            score_strict_warnings
            GTest::gtest_main
    )
    target_compile_options(thread_pool_test PRIVATE -fexceptions)
    add_test(NAME ThreadPoolTest COMMAND thread_pool_test)
endif()

# Install targets
# Install main library targets
set(CONCURRENCY_TARGETS
    score_clock
    score_destruction_guard
    score_condition_variable
    score_interruptible_wait
    score_notification
    score_interruptible_interprocess_condition_variable
    score_executor
    score_delayed_task
    score_periodic_task
    score_long_running_threads_container
    score_synchronized_queue
    score_thread_pool
)

# Add mock targets if testing is enabled
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND CONCURRENCY_TARGETS
        score_executor_mock
        score_interruptible_conditional_variable_mock
    )
endif()

install(TARGETS ${CONCURRENCY_TARGETS}
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install main headers
set(CONCURRENCY_HEADERS
    clock.h
    destruction_guard.h
    condition_variable.h
    interruptible_wait.h
    notification.h
    interruptible_interprocess_condition_variable.h
    executor.h
    simple_task.h
    task.h
    task_result_base.h
    task_result.h
    shared_task_result.h
    delayed_task.h
    periodic_task.h
    long_running_threads_container.h
    thread_pool.h
)

# Add mock headers if testing is enabled
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    list(APPEND CONCURRENCY_HEADERS
        executor_mock.h
        interruptible_conditional_variable_mock.h
    )
endif()

install(FILES ${CONCURRENCY_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/concurrency
)