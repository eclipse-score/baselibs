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

# Create datetime converter library with implementations
add_library(score_datetime_converter
    DateTimeConverter.cpp
    time_conversion.cpp
)

target_include_directories(score_datetime_converter
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_datetime_converter
    PUBLIC
        score_compiler_flags
        score_language_futurecpp
)

# Create alias target
add_library(score::datetime_converter ALIAS score_datetime_converter)

# Install target
install(TARGETS score_datetime_converter
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    DateTimeConverter.h
    time_conversion.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/datetime_converter
)