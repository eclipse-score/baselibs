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
#include "score/os/string.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(StringTest, StringMemcpy)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringTest String Memcpy");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    char src[] = "ABCDEFGHIJ";
    char dest[11];

    score::os::String::instance().memcpy(dest, src, 10);
    dest[10] = '\0';
    EXPECT_STREQ(dest, "ABCDEFGHIJ");
    score::os::String::instance().memcpy(dest, "1234567890", 5);
    dest[10] = '\0';
    EXPECT_STREQ(dest, "12345FGHIJ");
}

TEST(StringTest, StringMemset)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringTest String Memset");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    char dest[11];
    score::os::String::instance().memset(dest, 'a', 10);
    dest[10] = '\0';
    EXPECT_STREQ(dest, "aaaaaaaaaa");
}

TEST(StringTest, StringStrError)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "StringTest String Str Error");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "Generation and analysis of equivalence classes");

    for (int i = 0; i < 10; ++i)
    {
        const char* error_str = score::os::String::instance().strerror(i);
        EXPECT_NE(error_str, nullptr);
        EXPECT_STRNE(error_str, "");
    }

    EXPECT_NE(strstr(score::os::String::instance().strerror(EINVAL), "Invalid argument"), nullptr);
}

}  // namespace
}  // namespace os
}  // namespace score
