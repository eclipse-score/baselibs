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
#include "score/os/string_impl.h"

#include <string.h>

#include "gtest/gtest.h"

namespace score
{
namespace os
{
namespace test
{

TEST(StringImplTest, StrMemcpy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringImplTest Str Memcpy");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StringImpl str;
    char str1[] = "mem1";
    char str2[] = "mem2";
    str.memcpy(str1, str2, sizeof(str2));
    EXPECT_STREQ(str1, "mem2");
}

TEST(StringImplTest, StrStderr)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringImplTest Str Stderr");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StringImpl str;
    EXPECT_STREQ(str.strerror(EINVAL), "Invalid argument");
}

TEST(StringImplTest, StrMemset)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringImplTest Str Memset");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    StringImpl str;
    char str1[20] = "Sample";
    str.memset(str1, '1', 6);
    EXPECT_STREQ(str1, "111111");
}

}  // namespace test
}  // namespace os
}  // namespace score
