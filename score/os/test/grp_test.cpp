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
#include "score/os/grp.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(GetGrNam, ReturnsCorrectBuffer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetGrNam Returns Correct Buffer");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto group_name{"root"};
    const auto result = Grp::instance().getgrnam(group_name);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().gid, 0);
    EXPECT_STREQ(result.value().name, group_name);
}

TEST(GetGrNam, SecondCallDoesNotOverwriteBuffer)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetGrNam Second Call Does Not Overwrite Buffer");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto group_name_root{"root"};
    const auto result_root = Grp::instance().getgrnam(group_name_root);

    // Our QNX does not have any groups matching with default linux groups. Hence we need to pick a different one for
    // this test.
#if defined(__linux__)
    const auto group_name_2{"daemon"};
    const auto group_number_2{1};
#elif defined(__QNX__)
    const auto group_name_2{"autosar"};
    const auto group_number_2{1054};
#else
#error "Unsupported OS"
#endif
    const auto result_daemon = Grp::instance().getgrnam(group_name_2);

    ASSERT_TRUE(result_root.has_value());
    EXPECT_EQ(result_root.value().gid, 0);
    EXPECT_STREQ(result_root.value().name, group_name_root);

    ASSERT_TRUE(result_daemon.has_value());
    EXPECT_EQ(result_daemon.value().gid, group_number_2);
    EXPECT_STREQ(result_daemon.value().name, group_name_2);
}

TEST(GetGrNam, ReturnsErrorWhenGroupUnknown)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetGrNam Returns Error When Group Unknown");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto group_name{"invalid"};
    const auto result = Grp::instance().getgrnam(group_name);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::createFromErrno(ENOENT));
}

TEST(GetGrNam, ReturnsErrorWhenGroupNameSizeBiggerThanSupported)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "GetGrNam Returns Error When Group Name Size Bigger Than Supported");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const auto group_name{"VeryVeryVeryVeryGroup"};
    const auto result = Grp::instance().getgrnam(group_name);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Error::createFromErrno(ENOMEM));
}

}  // namespace
}  // namespace os
}  // namespace score
