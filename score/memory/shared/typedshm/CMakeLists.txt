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

# Typed shared memory wrapper library
add_library(score_memory_shared_typedshm
    typedshm_wrapper/typed_memory.cpp
)

target_include_directories(score_memory_shared_typedshm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_memory_shared_typedshm
    PUBLIC
        score_compiler_flags
        score_os
        score_language_futurecpp
)

# Create alias target
add_library(score::memory_shared_typedshm ALIAS score_memory_shared_typedshm)

# Install target
install(TARGETS score_memory_shared_typedshm
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    typedshm_wrapper/typed_memory.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/memory/shared/typedshm/typedshm_wrapper
)