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

# JSON Examples (optional - only build if examples are requested)
option(SCORE_BUILD_EXAMPLES "Build example programs" OFF)

if(SCORE_BUILD_EXAMPLES)
    # JSON Buffer Example
    add_executable(json_buffer_example
        json_buffer.cpp
    )
    
    target_link_libraries(json_buffer_example
        PRIVATE
            score_json
            score_compiler_flags
    )
    
    # JSON List Example  
    add_executable(json_list_example
        json_list.cpp
    )
    
    target_link_libraries(json_list_example
        PRIVATE
            score_json
            score_compiler_flags
    )
    
    # JSON Object Example
    add_executable(json_object_example
        json_object.cpp
    )
    
    target_link_libraries(json_object_example
        PRIVATE
            score_json
            score_compiler_flags
    )
    
    # Install examples (optional)
    install(TARGETS 
        json_buffer_example
        json_list_example
        json_object_example
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
        OPTIONAL
    )
endif()