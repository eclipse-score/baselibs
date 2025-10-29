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

# Add subdirectory
add_subdirectory(json_serialize)

# Create alias for easier access
add_library(score_json_writer INTERFACE)

target_link_libraries(score_json_writer
    INTERFACE
        score_json_writer_serialize
)

# Install target
install(TARGETS score_json_writer
    EXPORT score-baselibsTargets
)