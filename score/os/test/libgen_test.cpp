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
#include "score/os/libgen.h"
#include "score/os/libgen_impl.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(LibgenImplTest, GetBaseName)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LibgenImplTest Get Base Name");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    char input1[] = "usr";
    EXPECT_STREQ(score::os::Libgen::instance().base_name(input1), "usr");

    char input2[] = "usr/";
    EXPECT_STREQ(score::os::Libgen::instance().base_name(input2), "usr");

    char input3[] = "";
    EXPECT_STREQ(score::os::Libgen::instance().base_name(input3), ".");
}

TEST(LibgenImplTest, GetDirName)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LibgenImplTest Get Dir Name");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    char input1[] = "/foo/bar";
    EXPECT_STREQ(score::os::Libgen::instance().dirname(input1), "/foo");

    char input2[] = "foo";
    EXPECT_STREQ(score::os::Libgen::instance().dirname(input2), ".");

    char input3[] = "";
    EXPECT_STREQ(score::os::Libgen::instance().dirname(input3), ".");

    char input4[] = "//foo";
    // Leading "//" is implementation-defined. May return either "//" or '/' (but nothing else).
    EXPECT_THAT(score::os::Libgen::instance().dirname(input4), ::testing::MatchesRegex("\\/|\\/\\/"))
        << "may return either '//' or '/', but nothing else";
}

TEST(LibgenTest, PMRDefaultShallReturnImplInstance)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "LibgenTest PMRDefault Shall Return Impl Instance");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    score::cpp::pmr::memory_resource* memory_resource = score::cpp::pmr::get_default_resource();
    const auto instance = score::os::Libgen::Default(memory_resource);
    ASSERT_TRUE(instance != nullptr);
    EXPECT_NO_THROW(std::ignore = dynamic_cast<score::os::LibgenImpl*>(instance.get()));
}
