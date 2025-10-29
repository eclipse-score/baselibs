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

# Create safecpp library with core implementation
add_library(score_safecpp
    aborts_upon_exception/aborts_upon_exception.cpp
    coverage_termination_handler/coverage_termination_handler.cpp
    safe_math/safe_math.cpp
    safe_math/error.cpp
    safe_math/details/floating_point_environment.cpp
    safe_math/details/absolute/absolute.cpp
    safe_math/details/addition_subtraction/addition_subtraction.cpp
    safe_math/details/cast/cast.cpp
    safe_math/details/comparison/comparison.cpp
    safe_math/details/division/division.cpp
    safe_math/details/multiplication/multiplication.cpp
    safe_math/details/negate/negate.cpp
    safe_math/details/type_traits/type_traits.cpp
    scoped_function/move_only_scoped_function.cpp
    scoped_function/copyable_scoped_function.cpp
    scoped_function/scope.cpp
    scoped_function/details/scope_state.cpp
    scoped_function/details/type_erasure_pointer.cpp
    scoped_function/details/allocator_aware_erased_type.cpp
    scoped_function/details/function_wrapper.cpp
    scoped_function/details/allocator_aware_type_erasure_pointer.cpp
    scoped_function/details/invoker.cpp
    scoped_function/details/scoped_function_invoker.cpp
    scoped_function/details/allocator_wrapper.cpp
    scoped_function/details/call_and_return_modified.cpp
    scoped_function/details/instrumented_memory_resource.cpp
    scoped_function/details/is_callable_from.cpp
    scoped_function/details/modify_return_type.cpp
    scoped_function/details/modify_signature.cpp
    scoped_function/details/parametrization_helper.cpp
    string_view/null_termination_check.cpp
    string_view/null_termination_violation_policies.cpp
    string_view/zspan.cpp
    string_view/zstring_view.cpp
)

target_include_directories(score_safecpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(score_safecpp
    PUBLIC
        score_compiler_flags
        score_futurecpp
)

# Install target
install(TARGETS score_safecpp
    EXPORT score-baselibsTargets
)