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
#include "score/os/mount.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

TEST(MountTest, MountTestMountFail)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MountTest Mount Test Mount Fail");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ::mkdir("/mnt/home", S_IRWXU | S_IRWXG | S_IRWXO);

    score::cpp::expected_blank<Error> result =
        score::os::Mount::instance().mount(NULL, "/mnt/home", "network", score::os::Mount::Flag::kReadOnly, NULL);
    EXPECT_FALSE(result.has_value());
}

TEST(MountTest, MountTestUmountFail)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MountTest Mount Test Umount Fail");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    const char* dir = "/mnt/home";
    score::cpp::expected_blank<Error> result = score::os::Mount::instance().umount(dir);
    EXPECT_FALSE(result.has_value());
}

TEST(MountTest, MountTestConvertFlag)
{
    RecordProperty("Verifies", "SCR-46010294");
    RecordProperty("ASIL", "B");
    RecordProperty("Description", "MountTest Mount Test Convert Flag");
    RecordProperty("TestType", "Interface test");
    RecordProperty("DerivationTechnique", "equivalence-classes");

    ::mkdir("/mnt/home", S_IWUSR | S_IWGRP | S_IWOTH);

    score::cpp::expected_blank<Error> result =
        score::os::Mount::instance().mount(NULL, "/mnt/home", "network", score::os::Mount::Flag::kNone, NULL);
    EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace os
}  // namespace score
