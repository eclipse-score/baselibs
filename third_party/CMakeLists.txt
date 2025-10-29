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

# Third-party dependencies
# These are external libraries that the project depends on

# ACL library
if(ACL_FOUND)
    add_library(score_third_party_acl INTERFACE)
    target_link_libraries(score_third_party_acl INTERFACE PkgConfig::ACL)
    
    # Create alias to match the bazel target structure
    add_library(acl ALIAS score_third_party_acl)
else()
    message(STATUS "ACL library not found. Some features may not be available.")
endif()

# LibCap library
if(LIBCAP_FOUND)
    add_library(score_third_party_libcap INTERFACE)
    target_link_libraries(score_third_party_libcap INTERFACE PkgConfig::LIBCAP)
    
    # Create alias to match the bazel target structure
    add_library(libcap2 ALIAS score_third_party_libcap)
else()
    message(STATUS "LibCap library not found. Some features may not be available.")
endif()

# LibSeccomp library
if(LIBSECCOMP_FOUND)
    add_library(score_third_party_libseccomp INTERFACE)
    target_link_libraries(score_third_party_libseccomp INTERFACE PkgConfig::LIBSECCOMP)
    
    # Create alias to match the bazel target structure
    add_library(libseccomp2 ALIAS score_third_party_libseccomp)
else()
    message(STATUS "LibSeccomp library not found. Some features may not be available.")
endif()

# ITF - This seems to be a custom testing framework
# For now, create a placeholder interface library
add_library(score_third_party_itf INTERFACE)
add_library(itf ALIAS score_third_party_itf)

# Valgrind - typically used for development/debugging, not runtime
# This would usually be found via FindValgrind module
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    message(STATUS "Valgrind found: ${VALGRIND_EXECUTABLE}")
    add_library(score_third_party_valgrind INTERFACE)
    target_compile_definitions(score_third_party_valgrind INTERFACE HAVE_VALGRIND=1)
    add_library(valgrind ALIAS score_third_party_valgrind)
else()
    message(STATUS "Valgrind not found. Memory debugging features may not be available.")
    add_library(score_third_party_valgrind INTERFACE)
    add_library(valgrind ALIAS score_third_party_valgrind)
endif()

# Export available third-party libraries
if(TARGET score_third_party_acl)
    install(TARGETS score_third_party_acl
        EXPORT score-baselibsTargets
    )
endif()

if(TARGET score_third_party_libcap)
    install(TARGETS score_third_party_libcap
        EXPORT score-baselibsTargets
    )
endif()

if(TARGET score_third_party_libseccomp)
    install(TARGETS score_third_party_libseccomp
        EXPORT score-baselibsTargets
    )
endif()

install(TARGETS score_third_party_itf score_third_party_valgrind
    EXPORT score-baselibsTargets
)