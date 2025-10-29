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
add_subdirectory(futurecpp)
add_subdirectory(safecpp)

# Create aliases for easier access
add_library(score_language_futurecpp ALIAS score_futurecpp)
add_library(score_language_safecpp ALIAS score_safecpp)

# Create a combined language library
add_library(score_language INTERFACE)
target_link_libraries(score_language INTERFACE
    score_futurecpp
    score_safecpp
)

# Create score:: namespace aliases
add_library(score::language_futurecpp ALIAS score_futurecpp)
add_library(score::language_safecpp ALIAS score_safecpp)
add_library(score::language ALIAS score_language)

# Install target
install(TARGETS score_language
    EXPORT score-baselibsTargets
)