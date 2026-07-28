/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_QUALITY_COMPILER_WARNINGS_DEPRECATION_H
#define SCORE_QUALITY_COMPILER_WARNINGS_DEPRECATION_H

// clang-format off
// Emit deprecation warnings from macro aliases by referencing a deprecated type alias.
#ifndef SCORE_DEPRECATE_MACRO_USE
#define SCORE_DEPRECATE_MACRO_USE(MESSAGE) SCORE_DEPRECATE_MACRO_USE_IMPL(MESSAGE, __COUNTER__)
#define SCORE_DEPRECATE_MACRO_USE_IMPL(MESSAGE, ID)                                                      \
    using score_deprecated_macro_def_##ID [[deprecated(MESSAGE)]] = int;                               \
    static_assert(sizeof(score_deprecated_macro_def_##ID) > 0, "")
#endif
// clang-format on
#endif  // SCORE_QUALITY_COMPILER_WARNINGS_DEPRECATION_H
