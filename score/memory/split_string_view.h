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
#ifndef SCORE_LIB_MEMORY_SPLIT_STRING_VIEW_H
#define SCORE_LIB_MEMORY_SPLIT_STRING_VIEW_H

// MIGRATION SHIM: LazySplitStringView moved to score/string_manipulation/split_string_view.h. This header is kept
// so that existing bazel targets, include paths ("score/memory/split_string_view.h") and the score::memory
// namespace keep working unchanged. New code should include "score/string_manipulation/split_string_view.h"
// directly and use the score::string_manipulation namespace.
#include "score/string_manipulation/split_string_view.h"

namespace score::memory
{

using score::string_manipulation::LazySplitStringView;

}  // namespace score::memory

#endif  // SCORE_LIB_MEMORY_SPLIT_STRING_VIEW_H
