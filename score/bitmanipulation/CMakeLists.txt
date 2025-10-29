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

# Bit manipulation library
add_library(score_bitmanipulation
    bit_manipulation.cpp
)

target_link_libraries(score_bitmanipulation
    PUBLIC
        score_compiler_flags
)

# Bitmask operators library
add_library(score_bitmask_operators
    bitmask_operators.cpp
)

target_link_libraries(score_bitmask_operators
    PUBLIC
        score_compiler_flags
)

# Create alias targets
add_library(score::bitmanipulation ALIAS score_bitmanipulation)
add_library(score::bitmask_operators ALIAS score_bitmask_operators)

# Tests
if(SCORE_BUILD_TESTING AND GTEST_FOUND)
    # Bit manipulation tests
    add_executable(bit_manipulation_test
        bit_manipulation_test.cpp
    )
    
    target_link_libraries(bit_manipulation_test
        PRIVATE
            score_bitmanipulation
            score_strict_warnings
    )
    
    score_link_gtest(bit_manipulation_test)
    target_compile_options(bit_manipulation_test PRIVATE -fexceptions)
    add_test(NAME BitManipulationTest COMMAND bit_manipulation_test)

    # Bitmask operators tests
    add_executable(bitmask_operators_test
        bitmask_operators_test.cpp
    )
    
    target_link_libraries(bitmask_operators_test
        PRIVATE
            score_bitmask_operators
            score_strict_warnings
    )
    
    score_link_gtest(bitmask_operators_test)
    target_compile_options(bitmask_operators_test PRIVATE -fexceptions)
    add_test(NAME BitmaskOperatorsTest COMMAND bitmask_operators_test)
endif()

# Install targets
install(TARGETS score_bitmanipulation score_bitmask_operators
    EXPORT score-baselibsTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(FILES
    bit_manipulation.h
    bitmask_operators.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/score/bitmanipulation
)