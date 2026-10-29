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
#include "score/quality/compiler_warnings/warnings.h"

#include <gtest/gtest.h>

namespace
{

[[deprecated("This addition function is deprecated for demonstration purposes.")]]
int Add(const int lhs, const int rhs)
{
    return lhs + rhs;
}

TEST(CompilerWarningsDemo, SuppressesDeprecatedCallInExpectedScope)
{
    DISABLE_WARNING_PUSH
    DISABLE_WARNING_DEPRECATED
    const int result{Add(2, 3)};
    DISABLE_WARNING_POP

    EXPECT_EQ(result, 5);
}

}  // namespace
