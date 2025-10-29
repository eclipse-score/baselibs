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

# File lock mutex library for shared memory
add_library(score_memory_shared_flock
    exclusive_flock_mutex.cpp
    flock_mutex.cpp
    flock_mutex_and_lock.cpp
    shared_flock_mutex.cpp
)

target_include_directories(score_memory_shared_flock
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_flock
    PUBLIC
        score_compiler_flags
        score_os
        score_language_futurecpp
        score_mw_log_core
)

# Create alias target
add_library(score::memory_shared_flock ALIAS score_memory_shared_flock)

# Install target
install(TARGETS score_memory_shared_flock
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    exclusive_flock_mutex.h
    flock_mutex.h
    flock_mutex_and_lock.h
    shared_flock_mutex.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/memory/shared/flock
)