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
#ifndef SCORE_LIB_MEMORY_STRING_LITERAL_H
#define SCORE_LIB_MEMORY_STRING_LITERAL_H

// MIGRATION SHIM: StringLiteral moved to score/string_manipulation/string_literal.h. This header is kept so that
// existing bazel targets and include paths ("score/memory/string_literal.h") keep working unchanged. The symbol
// lives in the score namespace both before and after the move, so no namespace adaptation is required. New code
// should include "score/string_manipulation/string_literal.h" directly.
#include "score/string_manipulation/string_literal.h"

#endif  // SCORE_LIB_MEMORY_STRING_LITERAL_H
