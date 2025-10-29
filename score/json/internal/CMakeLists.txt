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

# Add subdirectories for internal JSON components
add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(writer)

# Create interface library that combines all internal components
add_library(score_json_internal INTERFACE)

target_link_libraries(score_json_internal
    INTERFACE
        score_json_internal_model
        score_json_parser
        score_json_writer
)

# Install target
install(TARGETS score_json_internal
    EXPORT score-baselibsTargets
)