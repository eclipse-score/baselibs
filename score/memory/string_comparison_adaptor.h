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
#ifndef SCORE_LIB_MEMORY_STRING_COMPARISON_ADAPTOR_H
#define SCORE_LIB_MEMORY_STRING_COMPARISON_ADAPTOR_H

// MIGRATION SHIM: StringComparisonAdaptor moved to score/string_manipulation/string_comparison_adaptor.h. This
// header is kept so that existing bazel targets, include paths ("score/memory/string_comparison_adaptor.h") and
// the score::memory namespace keep working unchanged. New code should include
// "score/string_manipulation/string_comparison_adaptor.h" directly and use the score::string_manipulation
// namespace.
#include "score/string_manipulation/string_comparison_adaptor.h"

namespace score::memory
{

using score::string_manipulation::StringComparisonAdaptor;

}  // namespace score::memory

#endif  // SCORE_LIB_MEMORY_STRING_COMPARISON_ADAPTOR_H
