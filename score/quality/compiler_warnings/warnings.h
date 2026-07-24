/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
/**
 * Portable way to disable warnings, however this should be the last option, warnings should be fixed and analyzed
 * Extracted from https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
 */
#if defined(__GNUC__) || defined(__clang__)
// clang-format off
#define DO_PRAGMA(X) _Pragma(#X)  // NOLINT(score-banned-preprocessor-directives) : required due to false positive compiler warning
// clang-format on
#define DISABLE_WARNING_PUSH DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName) DO_PRAGMA(GCC diagnostic ignored #warningName)

// clang-format off
#define DISABLE_WARNING_SUGGEST_OVERRIDE                        DISABLE_WARNING(-Wsuggest-override)
#define DISABLE_WARNING_ZERO_AS_NULL_POINTER_CONSTANT           DISABLE_WARNING(-Wzero-as-null-pointer-constant)
#define DISABLE_WARNING_USELESS_CAST                            DISABLE_WARNING(-Wuseless-cast)
#define DISABLE_WARNING_CONVERSION                              DISABLE_WARNING(-Wconversion)
#define DISABLE_WARNING_CLASS_MEM_ACCESS                        DISABLE_WARNING(-Wclass-memaccess)
#define DISABLE_WARNING_OLD_STYLE_CAST                          DISABLE_WARNING(-Wold-style-cast)
#define DISABLE_WARNING_SHADOW                                  DISABLE_WARNING(-Wshadow)
#define DISABLE_WARNING_TYPE_LIMITS                             DISABLE_WARNING(-Wtype-limits)
#define DISABLE_WARNING_ARRAY_BOUNDS                            DISABLE_WARNING(-Warray-bounds)
#define DISABLE_WARNING_UNUSED_VALUE                            DISABLE_WARNING(-Wunused-value)
#define DISABLE_WARNING_PACKED                                  DISABLE_WARNING(-Wpacked)
#define DISABLE_WARNING_ATTRIBUTES                              DISABLE_WARNING(-Wattributes)
#define DISABLE_WARNING_SIGN_CONVERSION                         DISABLE_WARNING(-Wsign-conversion)

#if defined(__clang__)
#define DISABLE_WARNING_SELF_ASSIGN_OVERLOADED                  DISABLE_WARNING(-Wself-assign-overloaded)
#define DISABLE_WARNING_C99_EXTENSIONS                          DISABLE_WARNING(-Wc99-extensions)
#define DISABLE_WARNING_SELF_MOVE                               DISABLE_WARNING(-Wself-move)
#define DISABLE_WARNING_MISSING_DESIGNATED_FIELD_INITIALIZERS   DISABLE_WARNING(-Wmissing-designated-field-initializers)
#define DISABLE_WARNING_MAYBE_UNINITIALIZED
#else
#define DISABLE_WARNING_SELF_ASSIGN_OVERLOADED
#define DISABLE_WARNING_C99_EXTENSIONS
#define DISABLE_WARNING_SELF_MOVE
#define DISABLE_WARNING_MISSING_DESIGNATED_FIELD_INITIALIZERS
#define DISABLE_WARNING_MAYBE_UNINITIALIZED                     DISABLE_WARNING(-Wmaybe-uninitialized)
#endif

// clang-format on

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP

#define DISABLE_WARNING_SUGGEST_OVERRIDE
#define DISABLE_WARNING_ZERO_AS_NULL_POINTER_CONSTANT
#define DISABLE_WARNING_USELESS_CAST
#define DISABLE_WARNING_CONVERSION
#define DISABLE_WARNING_CLASS_MEM_ACCESS
#define DISABLE_WARNING_OLD_STYLE_CAST
#define DISABLE_WARNING_SHADOW
#define DISABLE_WARNING_TYPE_LIMITS
#define DISABLE_WARNING_SELF_ASSIGN_OVERLOADED
#define DISABLE_WARNING_MAYBE_UNINITIALIZED
#define DISABLE_WARNING_ARRAY_BOUNDS
#define DISABLE_WARNING_SELF_MOVE
#define DISABLE_WARNING_UNUSED_VALUE
#define DISABLE_WARNING_PACKED
#define DISABLE_WARNING_ATTRIBUTES
#define DISABLE_WARNING_C99_EXTENSIONS
#define DISABLE_WARNING_SIGN_CONVERSION
#define DISABLE_WARNING_MISSING_DESIGNATED_FIELD_INITIALIZERS

#endif

// clang-format off
// Emit deprecation warnings from macro aliases by referencing a deprecated type alias.
#ifndef SCORE_DEPRECATE_MACRO_USE
#define SCORE_DEPRECATE_MACRO_USE(MESSAGE) SCORE_DEPRECATE_MACRO_USE_IMPL(MESSAGE, __COUNTER__)
#define SCORE_DEPRECATE_MACRO_USE_IMPL(MESSAGE, ID)                                                      \
    using score_deprecated_macro_def_##ID [[deprecated(MESSAGE)]] = void;                               \
    using score_deprecated_macro_use_##ID [[maybe_unused]] = score_deprecated_macro_def_##ID
#endif
// clang-format on
