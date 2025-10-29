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

# Static reflection serialization library - header only
add_library(score_static_reflection_serialization INTERFACE)

# Copy headers to build directory with desired structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/serialization/for_logging.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization/for_logging.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/serialization/skip_deserialize.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization/skip_deserialize.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/serialization/visit_serialize.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization/visit_serialize.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/serialization/visit_size.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization/visit_size.h
    COPYONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/serialization/visit_type_traits.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization/serialization/visit_type_traits.h
    COPYONLY
)

target_include_directories(score_static_reflection_serialization
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_static_reflection_serialization
    INTERFACE
        score_compiler_flags
)

# Create alias
add_library(score::static_reflection_serialization ALIAS score_static_reflection_serialization)

# Install target
install(TARGETS score_static_reflection_serialization
    EXPORT score-baselibsTargets
)

# Install ONLY from the copied location with the desired structure
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/static_reflection_with_serialization
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
