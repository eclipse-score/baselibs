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

# Core logging functionality
add_library(score_mw_log_core
    log_common.cpp
    log_level.cpp
    log_stream.cpp
    log_stream_error.cpp
    log_stream_factory.cpp
    irecorder_factory.cpp
    recorder.cpp
    slot_handle.cpp
)

target_include_directories(score_mw_log_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log_core
    PUBLIC
        score_compiler_flags
        score_result
        score_concurrency
        score_static_reflection_with_serialization
)

# Logger and runtime
add_library(score_mw_log_runtime
    logger.cpp
    logger_container.cpp
    logging.cpp
    runtime.cpp
)

target_include_directories(score_mw_log_runtime
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log_runtime
    PUBLIC
        score_compiler_flags
        score_mw_log_core
)

# Detail implementation
add_library(score_mw_log_detail
    detail/backend.cpp
    detail/backend_mock.cpp
    detail/circular_allocator.cpp
    detail/composite_recorder.cpp
    detail/dlt_argument_counter.cpp
    detail/dlt_format.cpp
    detail/empty_recorder.cpp
    detail/empty_recorder_factory.cpp
    detail/error.cpp
    detail/initialization_reporter.cpp
    detail/istatistics_reporter.cpp
    detail/log_entry.cpp
    detail/log_record.cpp
    detail/logging_identifier.cpp
    detail/recorder_factory.cpp
    detail/recorder_factory_stub.cpp
    detail/statistics_reporter.cpp
    detail/thread_local_guard.cpp
    detail/verbose_payload.cpp
    detail/file_logging/console_recorder_factory.cpp
    detail/file_logging/dlt_message_builder.cpp
    detail/file_logging/file_output_backend.cpp
    detail/file_logging/file_recorder.cpp
    detail/file_logging/file_recorder_factory.cpp
    detail/file_logging/imessage_builder.cpp
    detail/file_logging/non_blocking_writer.cpp
    detail/file_logging/slot_drainer.cpp
    detail/file_logging/text_format.cpp
    detail/file_logging/text_message_builder.cpp
    detail/file_logging/text_recorder.cpp
)


option(ENABLE_KCONSOLE_LOGGING "Enable console logging" ON)

if(ENABLE_KCONSOLE_LOGGING)
    target_compile_definitions(score_mw_log_detail PRIVATE KCONSOLE_LOGGING)
endif()

option(ENABLE_KFILE_LOGGING "Enable file logging" ON)

if(ENABLE_KFILE_LOGGING)
    target_compile_definitions(score_mw_log_detail PRIVATE KFILE_LOGGING)
endif()

target_include_directories(score_mw_log_detail
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log_detail
    PUBLIC
        score_static_reflection_with_serialization
        score_compiler_flags
        score_mw_log_core
        score_mw_log_configuration
        score_memory
        score_os
)

# File logging functionality (only include files that exist, exclude dlt_message_builder.cpp due to missing serialization headers)
add_library(score_mw_log_file
    detail/file_logging/console_recorder_factory.cpp
    # detail/file_logging/dlt_message_builder.cpp  # Excluded: missing serialization/for_logging.h
    detail/file_logging/file_output_backend.cpp
    detail/file_logging/file_recorder.cpp
    detail/file_logging/file_recorder_factory.cpp
    detail/file_logging/imessage_builder.cpp
    detail/file_logging/non_blocking_writer.cpp
    detail/file_logging/slot_drainer.cpp
    detail/file_logging/text_format.cpp
    detail/file_logging/text_message_builder.cpp
    detail/file_logging/text_recorder.cpp
)

target_include_directories(score_mw_log_file
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log_file
    PUBLIC
        score_compiler_flags
        score_mw_log_detail
        score_filesystem
        score_os
)

# SLOG backend (QNX-specific, excluded for Linux builds)
# add_library(score_mw_log_slog
#     detail/slog/slog_backend.cpp
#     detail/slog/slog_recorder_factory.cpp
# )
# 
# target_include_directories(score_mw_log_slog
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )
# 
# target_link_libraries(score_mw_log_slog
#     PUBLIC
#         score_compiler_flags
#         score_mw_log_detail
#         score_os
# )

# Configuration (only include files that exist)
add_library(score_mw_log_configuration
    configuration/configuration.cpp
    configuration/target_config_reader.cpp
    configuration/target_config_reader_mock.cpp
    configuration/configuration_file_discoverer.cpp
    configuration/configuration_file_discoverer_mock.cpp
    configuration/iconfiguration_file_discoverer.cpp
    configuration/itarget_config_reader.cpp
    configuration/invconfig.cpp
    configuration/nvconfig.cpp
)

target_include_directories(score_mw_log_configuration
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log_configuration
    PUBLIC
        score_compiler_flags
        score_mw_log_core
        score_filesystem
        score_json
)

# Main logging library that includes everything
add_library(score_mw_log INTERFACE)

target_include_directories(score_mw_log
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_mw_log
    INTERFACE
        score_compiler_flags
        score_mw_log_core
        score_mw_log_runtime
        score_mw_log_detail
        score_mw_log_file
        # score_mw_log_slog  # Excluded for Linux builds
        score_mw_log_configuration
)

# Create aliases
add_library(score::mw_log_core ALIAS score_mw_log_core)
add_library(score::mw_log_runtime ALIAS score_mw_log_runtime)
add_library(score::mw_log_detail ALIAS score_mw_log_detail)
add_library(score::mw_log_file ALIAS score_mw_log_file)
add_library(score::mw_log_configuration ALIAS score_mw_log_configuration)
add_library(score::mw_log ALIAS score_mw_log)

# Install all targets
install(TARGETS 
    score_mw_log_core
    score_mw_log_runtime
    score_mw_log_detail
    score_mw_log_file
    # score_mw_log_slog  # Excluded for Linux builds
    score_mw_log_configuration
    score_mw_log
    EXPORT score-baselibsTargets
)

# Install all headers recursively with directory exclusions
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/mw/log
    FILES_MATCHING 
    PATTERN "*.h"
    # Exclude directories
    PATTERN "design" EXCLUDE
    PATTERN "rust" EXCLUDE
    PATTERN "test" EXCLUDE
)
