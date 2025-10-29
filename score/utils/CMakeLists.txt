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

# Create utils library with implementations
add_library(score_utils
    pimpl_ptr.cpp
    string_hash.cpp
    src/PayloadValidation.cpp
)

target_include_directories(score_utils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_utils
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
)
# Create alias target
add_library(score::utils ALIAS score_utils)

# Install target
install(TARGETS score_utils
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    pimpl_ptr.h
    string_hash.h
    src/PayloadValidation.h
    src/scoped_operation.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/utils
)
