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
add_subdirectory(visitor)
add_subdirectory(serialization)

# Main static reflection library
add_library(score_static_reflection_with_serialization INTERFACE)

target_link_libraries(score_static_reflection_with_serialization
    INTERFACE
        score_compiler_flags
        score_static_reflection_visitor
)

# Create alias target
add_library(score::static_reflection_with_serialization ALIAS score_static_reflection_with_serialization)

# Install target
install(TARGETS score_static_reflection_with_serialization
    EXPORT score-baselibsTargets
)
