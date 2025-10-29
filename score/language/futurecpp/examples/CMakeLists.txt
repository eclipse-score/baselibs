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

# FutureCPP Examples (optional - only build if examples are requested)
if(SCORE_BUILD_EXAMPLES)
    # Expected Example
    add_executable(expected_example
        expected_example.cpp
    )
    
    target_link_libraries(expected_example
        PRIVATE
            score_futurecpp
            score_compiler_flags
    )
    
    # Multi Span Example
    add_executable(multi_span_example
        multi_span_example.cpp
    )
    
    target_link_libraries(multi_span_example
        PRIVATE
            score_futurecpp
            score_compiler_flags
    )
    
    # Install examples (optional)
    install(TARGETS 
        expected_example
        multi_span_example
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
        OPTIONAL
    )
endif()